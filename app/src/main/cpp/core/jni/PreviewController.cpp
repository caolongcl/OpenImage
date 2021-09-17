//
// Created by caolong on 2019/7/7.
//

#include <res/ResManager.hpp>
#include <res/Printer.hpp>
#include <jni/JniUtils.hpp>
#include <jni/PreviewController.hpp>
#include <softarch/ComType.hpp>
#include <softarch/ThreadPool.hpp>
#include <render/GLRender.hpp>
#include <softarch/VarType.hpp>
#include <render/Flow.hpp>
#include <process/base/WorkerFlow.hpp>
#include <softarch/ffmpeglib.hpp>

using namespace clt;

PreviewController::PreviewController()
        : m_previewing(false),
          m_rendering(false),
          m_render(new GLRender()),
          m_renderDelayCount(0) {
}

void PreviewController::Create(JavaVM *jvm, jobject thiz) {
    Log::v(Log::PREVIEW_CTRL_TAG, "PreviewController::Create");

    // 初始化渲染线程和工作线程
    Flow::Self()->Init();

    m_JavaVM = jvm;
    m_Thiz = thiz;

    Flow::Self()->PostToRender([this]() {
        JniUtils::DoOutCurJvm(m_JavaVM, [this](JNIEnv *env) {
            // 缓存需要用到的类、方法、域
            cacheJniMethod(env);

            // 加载资源
            if (m_LoadAssetsMethod != nullptr) {
                auto assetsPath = (jstring) env->CallObjectMethod(m_Thiz, m_LoadAssetsMethod);
                std::string path = JniUtils::GetString(env, assetsPath);

                // 在共享渲染线程加载资源
//                Flow::Self()->PostToShared([this, path]() {
                Log::v(Log::PREVIEW_CTRL_TAG, "LoadResource %s", path.c_str());
                ResManager::Self()->Init();
                ResManager::Self()->ScanAndLoad(path);
//                });
            }
        }, "PreviewController::Init");
    });

    // 初始化耗时工作线程
    WorkerFlow::Self()->Init();
}

void PreviewController::Destroy() {
    Log::v(Log::PREVIEW_CTRL_TAG, "PreviewController::Destroy");

    // 等待所有耗时任务停止
    WorkerFlow::Self()->DeInit();

    Flow::Self()->PostToRender([this]() {
        // 请除缓存的JNI资源
        JniUtils::DoOutCurJvm(m_JavaVM, [this](JNIEnv *env) {
            releaseJni(env);
        });

        // 销毁字体文件等资源
        ResManager::Self()->DeInit();
    });

    // 等待所有线程都停止
    Flow::Self()->DeInit();

    m_JavaVM = nullptr;
    m_Thiz = nullptr;
}

bool PreviewController::Init() {
    Flow::Self()->PostToRender(
            [this]() {
                Log::v(Log::PREVIEW_CTRL_TAG, "PreviewController::Init");

                // 初始化渲染器
                m_render->Init(shared_from_this());
            });

    return true;
}

void PreviewController::DeInit() {
    Flow::Self()->PostToRender(
            [this]() {
                Log::v(Log::PREVIEW_CTRL_TAG, "DeInit");

                m_render->DeInit();
            });
}

void PreviewController::SetPreviewMode(int rotate, int ratio,
                                       bool vFlip, bool hFlip,
                                       int width, int height) {
    Flow::Self()->PostToRender(
            [this, rotate, ratio, vFlip, hFlip, width, height]() {
                Log::d(Log::PREVIEW_CTRL_TAG,
                       "SetPreviewMode rotate %d ratio %d vFlip %d hFlip %d previewW %d previewH %d",
                       rotate, ratio, vFlip, hFlip, width, height);
                // 设置预览基本参数
                m_render->SetPreviewMode(rotate, ratio, vFlip, hFlip, width, height);

                // 将预览纹理设置给View
                JniUtils::DoOutCurJvm(m_JavaVM, [this](JNIEnv *env) {
                    Log::d(Log::PREVIEW_CTRL_TAG, "CreatePreviewTex :%d",
                           m_render->GetPreviewTexId());

                    if (m_SetPreviewTextureMethod != nullptr)
                        env->CallVoidMethod(m_Thiz, m_SetPreviewTextureMethod,
                                            m_render->GetPreviewTexId());
                }, "PreviewController::SetPreviewMode");
            });
}

void PreviewController::Start() {
    Flow::Self()->PostToRender(
            [this]() {
                Log::d(Log::PREVIEW_CTRL_TAG, "Start");

                m_previewing = true;

                JniUtils::DoOutCurJvm(m_JavaVM, [this](JNIEnv *env) {
                    if (m_StartPreviewMethod != nullptr)
                        env->CallVoidMethod(m_Thiz, m_StartPreviewMethod);
                }, "PreviewController::Start");
            });
}

void PreviewController::Resume() {
    Flow::Self()->PostToRender(
            [this]() {
                Log::d(Log::PREVIEW_CTRL_TAG, "Resume");
                m_rendering = true;
            });
}

void PreviewController::Pause() {
    Flow::Self()->PostToRender(
            [this]() {
                Log::d(Log::PREVIEW_CTRL_TAG, "Pause");
                m_rendering = false;

                // 清空当前滤镜和任务
                m_render->ClearFilters();
                m_render->ClearProcessTasks();
            });
}

void PreviewController::NotifyFrameAvailable() {
    Flow::Self()->PostToRender(
            [this]() {
                if (!m_previewing)
                    return;

                // SurfaceTexture.updateTexImage，采集的图像到了PreviewTexture
                JniUtils::DoOutCurJvm(m_JavaVM, [this](JNIEnv *env) {
                    if (m_UpdateFrameMethod != nullptr)
                        env->CallVoidMethod(m_Thiz, m_UpdateFrameMethod);
                }, "PreviewController::NotifyFrameAvailable");

                if (!m_rendering || m_renderDelayCount++ < 1)
                    return;

                // 将PreviewTexture绘制到SurfaceView中显示出来
                m_render->Render();
            });
}

void PreviewController::Stop() {
    Flow::Self()->PostToRender(
            [this]() {
                Log::d(Log::PREVIEW_CTRL_TAG, "Stop");

                m_renderDelayCount = 0;
                m_previewing = false;

                JniUtils::DoOutCurJvm(m_JavaVM, [this](JNIEnv *env) {
                    if (m_StopPreviewMethod != nullptr)
                        env->CallVoidMethod(m_Thiz, m_StopPreviewMethod);
                }, "PreviewController::Stop");

                // 停止录制
                int fps, bitrate;
                bool recording = m_render->GetRecordState(fps, bitrate);
                if (recording) {
                    Log::d(Log::PREVIEW_CTRL_TAG, "PreviewController::Stop stop record");

                    m_render->Record(false, 0, 0);
                    JniUtils::DoOutCurJvm(m_JavaVM, [this](JNIEnv *env) {
                        env->CallVoidMethod(m_Thiz, m_ConfigEncoderMethod, false, 0, 0, 0, 0);
                    });
                }
            });
}

void PreviewController::SetSurface(int type, ANativeWindow *window) {
    Flow::Self()->PostToRender(
            [this, type, window]() {
                Log::d(Log::PREVIEW_CTRL_TAG, "SetSurface %s's surface %s",
                       Constants::SurfaceTypeName(type).c_str(),
                       Utils::GetBoolStr(window != nullptr).c_str());
                m_render->SetSurface(type, window);
            });
}

void PreviewController::SetSurfaceSize(int width, int height) {
    Flow::Self()->PostToRender(
            [this, width, height]() {
                Log::d(Log::PREVIEW_CTRL_TAG, "SetSurfaceSize %d %d",
                       width, height);
                m_render->SetSurfaceSize(width, height);
            });
}

void PreviewController::Capture() {
    Flow::Self()->PostToRender(
            [this]() {
                Log::d(Log::PREVIEW_CTRL_TAG, "Capture");

                if (m_previewing && m_rendering) {
                    m_render->Capture();
                }
            });
}

void PreviewController::Record(bool start, int fps, float factor) {
    Flow::Self()->PostToRender(
            [this, start, fps, factor]() {
                int bitrate = 0;
                int width, height;
                m_render->GetCaptureSize(width, height);
                bitrate = static_cast<int>(static_cast<float >(width * height * fps) * factor);

                Log::d(Log::PREVIEW_CTRL_TAG,
                       "Record %s fps %d factor %f bitrate %d",
                       (start ? "start" : "stop"), fps, factor, bitrate);
                m_render->Record(start, fps, bitrate);

                // 创建录制编码器
                JniUtils::DoOutCurJvm(m_JavaVM, [&, this](JNIEnv *env) {
                    if (start) {
                        Log::d(Log::PREVIEW_CTRL_TAG,
                               "start config encoder wh(%d %d) fps %d bitrate %d",
                               width, height, fps, bitrate);
                        env->CallVoidMethod(m_Thiz, m_ConfigEncoderMethod, true, width, height, fps,
                                            bitrate);
                    } else {
                        Log::d(Log::PREVIEW_CTRL_TAG, "stop config encoder");
                        env->CallVoidMethod(m_Thiz, m_ConfigEncoderMethod, false, 0, 0, 0, 0);
                    }
                });
            });
}

void PreviewController::SetVar(const std::string &name, const Var &var) {
    Flow::Self()->PostToRender(
            [this, name, var]() {
                Log::d(Log::PREVIEW_CTRL_TAG, "SetVar %s %s", name.c_str(), var.Dump().c_str());

                if (m_previewing) {
                    m_render->SetVar(name, var);
                }
            });
}

void PreviewController::SetString(const std::string &name, const std::string &str) {
    Flow::Self()->PostToRender(
            [this, name, str]() {
                Log::v(Log::PREVIEW_CTRL_TAG, "SetString %s %s", name.c_str(), str.c_str());
                if (name == "basedir") {
//                    Flow::Self()->PostToShared([str]() {
                    ResManager::Self()->RegisterBaseDir(str);
//                    });
                } else if (name == "funcdir") {
//                    Flow::Self()->PostToShared([str]() {
                    ResManager::Self()->RegisterFunctionDir(str);
//                    });
                }
            });
}

void PreviewController::EnableFilter(const std::string &name, bool enable) {
    Flow::Self()->PostToRender(
            [this, name, enable]() {
                Log::d(Log::PREVIEW_CTRL_TAG, "EnableFilter %s %s", name.c_str(),
                       Utils::GetBoolStr(enable).c_str());
                if (m_previewing) {
                    m_render->EnableFilter(name, enable);
                }
            });
}

void PreviewController::EnableProcess(const std::string &name, bool enable) {
    Flow::Self()->PostToRender(
            [this, name, enable]() {
                Log::d(Log::PREVIEW_CTRL_TAG, "EnableProcess %s %s", name.c_str(),
                       Utils::GetBoolStr(enable).c_str());
                if (m_previewing) {
                    m_render->EnableProcess(name, enable);
                }
            });
}

void PreviewController::UpdateTargetPos(int x, int y) {
    Flow::Self()->PostToRender(
            [this, x, y]() {
                if (m_previewing) {
                    // Log::d(Log::PREVIEW_CTRL_TAG, "UpdateTargetPosition x %d y %d", x, y);
                    m_render->UpdateTargetPosition(x, y);
                }
            });
}

/**
 * 处理Render的回调
 * @param type
 */
void PreviewController::OnCallback(CallbackType type) {
    switch (type) {
        case CallbackType::CaptureSizeChanged: {
            int width, height;
            m_render->GetCaptureSize(width, height);

            JniUtils::DoOutCurJvm(m_JavaVM, [this, width, height](JNIEnv *env) {
                Log::d(Log::PREVIEW_CTRL_TAG, "CaptureSizeChanged Update ImageReader (%d %d)",
                       width, height);

                env->CallVoidMethod(m_Thiz, m_CreateImageReaderMethod, width, height);
            });

            int fps, bitrate;
            bool recording = m_render->GetRecordState(fps, bitrate);

            if (recording) {
                Log::d(Log::PREVIEW_CTRL_TAG, "CaptureSizeChanged Reconfig Encoder (%d %d)", width,
                       height);

                m_render->Record(false, 0, 0);
                JniUtils::DoOutCurJvm(m_JavaVM, [this, width, height, fps, bitrate](JNIEnv *env) {
                    env->CallVoidMethod(m_Thiz, m_ConfigEncoderMethod, false, width, height, fps,
                                        bitrate);
                });

                // restart recording
                m_render->Record(true, fps, bitrate);
                JniUtils::DoOutCurJvm(m_JavaVM, [this, width, height, fps, bitrate](JNIEnv *env) {
                    env->CallVoidMethod(m_Thiz, m_ConfigEncoderMethod, true, width, height, fps,
                                        bitrate);
                });
            }
        }
            break;
        case CallbackType::PostInfo: {
            JniUtils::DoOutCurJvm(m_JavaVM, [this, info = m_render->GetInfoToJava()](JNIEnv *env) {
                jstring str = env->NewStringUTF(info.c_str());
                env->CallVoidMethod(m_Thiz, m_PostInfoMethod, str);
            });
        }
            break;
        default:
            break;
    }
}

void PreviewController::SetFFmpegDebug(bool debug) {
    if (debug) {
        av_log_set_callback(ffmpegLogCallback);
    } else {
        av_log_set_callback(nullptr);
    }
}

void PreviewController::SetCalibrateParams(int boardSizeWidth, int boardSizeHeight,
                                           float boardSquareSizeWidth, float boardSquareSizeHeight,
                                           float markerSizeWidth, float markerSizeHeight) {
    Flow::Self()->PostToRender(
            [this, boardSizeWidth, boardSizeHeight,
                    boardSquareSizeWidth, boardSquareSizeHeight,
                    markerSizeWidth, markerSizeHeight]() {
                ResManager::Self()->SaveCalibrateParams(
                        {boardSizeWidth, boardSizeHeight},
                        {boardSquareSizeWidth, boardSquareSizeHeight},
                        {markerSizeWidth, markerSizeHeight});
            });
}

std::string PreviewController::GetParams(const std::string &paramsGroupName) {
    if (paramsGroupName == "calibrate_params") {
        CalibrateData calibrateData = ResManager::Self()->LoadCalibrateParams();
        return calibrateData.GetJsonString();
    }

    return "";
}

void PreviewController::ffmpegLogCallback(void *ptr, int level, const char *fmt, va_list vl) {
    switch (level) {
        case AV_LOG_DEBUG:
            Log::d(Log::FFMPEG_TAG, fmt, vl);
            break;
        case AV_LOG_VERBOSE:
            Log::v(Log::FFMPEG_TAG, fmt, vl);
            break;
        case AV_LOG_INFO:
            Log::i(Log::FFMPEG_TAG, fmt, vl);
            break;
        case AV_LOG_WARNING:
            Log::w(Log::FFMPEG_TAG, fmt, vl);
            break;
        case AV_LOG_ERROR:
            Log::e(Log::FFMPEG_TAG, fmt, vl);
            break;
        default:
            break;
    }
}

void PreviewController::cacheJniMethod(JNIEnv *env) {
    jclass previewClass = env->GetObjectClass(m_Thiz);
    assert(previewClass != nullptr);

    // 将纹理ID返给Java层
    m_SetPreviewTextureMethod = env->GetMethodID(previewClass, "FromNativeSetPreviewTexture",
                                                 "(I)V");
    CheckMethodIDExit(m_SetPreviewTextureMethod, FromNativeSetPreviewTexture);

    // 开启Java层预览
    m_StartPreviewMethod = env->GetMethodID(previewClass, "FromNativeStartPreview", "()V");
    CheckMethodIDExit(m_StartPreviewMethod, FromNativeStartPreview);

    // 停止Java层预览
    m_StopPreviewMethod = env->GetMethodID(previewClass, "FromNativeStopPreview", "()V");
    CheckMethodIDExit(m_StopPreviewMethod, FromNativeStopPreview);

    // 在GL线程更新Java层纹理
    m_UpdateFrameMethod = env->GetMethodID(previewClass, "FromNativeUpdateFrame", "()V");
    CheckMethodIDExit(m_UpdateFrameMethod, FromNativeUpdateFrame);

    // 创建Java层拍照surface
    m_CreateImageReaderMethod = env->GetMethodID(previewClass, "FromNativeCreateImageReader",
                                                 "(II)V");
    CheckMethodIDExit(m_CreateImageReaderMethod, FromNativeCreateImageReader);

    // 创建Java层录制surface及创建MediaCodec
    m_ConfigEncoderMethod = env->GetMethodID(previewClass, "FromNativeConfigEncoder", "(ZIIII)V");
    CheckMethodIDExit(m_ConfigEncoderMethod, FromNativeConfigEncoder);

    // 在Java层将资源拷贝到应用目录下
    m_LoadAssetsMethod = env->GetMethodID(previewClass, "FromNativeLoadAssets",
                                          "()Ljava/lang/String;");
    CheckMethodIDExit(m_LoadAssetsMethod, FromNativeLoadAssets);

    // 通过Java层显示信息
    m_PostInfoMethod = env->GetMethodID(previewClass, "FromNativePostInfo",
                                        "(Ljava/lang/String;)V");
    CheckMethodIDExit(m_PostInfoMethod, FromNativePostInfo);

    env->DeleteLocalRef(previewClass);
}

void PreviewController::releaseJni(JNIEnv *env) {
    m_SetPreviewTextureMethod = nullptr;
    m_StartPreviewMethod = nullptr;
    m_StopPreviewMethod = nullptr;
    m_UpdateFrameMethod = nullptr;
    m_LoadAssetsMethod = nullptr;
    m_PostInfoMethod = nullptr;
    m_ConfigEncoderMethod = nullptr;
    m_CreateImageReaderMethod = nullptr;
}