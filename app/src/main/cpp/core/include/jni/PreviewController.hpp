//
// Created by caolong on 2019/7/7.
//

#pragma once

#include <softarch/IComFunc.hpp>
#include <softarch/ComType.hpp>
#include <softarch/PreviewControllerCallback.hpp>
#include <softarch/VarType.hpp>
#include <softarch/Constants.hpp>

namespace clt {

    class GLRender;

    /**
     * 控制整个预览、拍照、录制等流程
     */
    class PreviewController final
            : public IComFunc<>,
              public PreviewControllerCallback,
              public std::enable_shared_from_this<PreviewControllerCallback> {
    public:
        PreviewController();

        ~PreviewController() = default;

        void Create(JavaVM *jvm, jobject thiz);

        void Destroy();

        bool Init() override;

        void DeInit() override;

        /**
         * 设置预览模式
         * @param rotate 预览正常显示旋转
         * @param ratio 预览宽高比
         * @param width 预览宽
         * @param height 预览高
         */
        void SetPreviewMode(int rotate, int ratio, bool vFlip, bool hFlip,
                            int width, int height);

        /**
         * 开启预览
         */
        void Start();

        /**
         * 恢复预览
         */
        void Resume();

        /**
         * 暂停预览
         */
        void Pause();

        /**
         * 停止预览
         */
        void Stop();

        /*
         * 获得一帧，绘制到界面上
         */
        void NotifyFrameAvailable();

        /**
         * 创建EGLSurface，如java传递下来的显示、拍照、录制的surface
         * @param window
         */
        void SetSurface(int type, ANativeWindow *window);

        void SetSurfaceSize(int width, int height);

        /**
         * 截取一张图片，拍照
         */
        void Capture();

        /**
         * 开启或关闭录制
         * @param start true 录制
         * @param fps 帧率
         * @param factor bitrate=w*h*fps*factor
         */
        void Record(bool start, int fps, float factor);

        /**
         * 传递封装参数，Var是统一的参数容器
         * @param name
         * @param var
         */
        void SetVar(const std::string &name, const Var &var);

        void SetString(const std::string &name, const std::string &var);

        /**
         * 开启或关闭某个滤镜
         * @param name
         * @param enable
         */
        void EnableFilter(const std::string &name, bool enable);


        /**
         * 开启或关闭某个处理任务
         * @param name
         * @param enable
         */
        void EnableProcess(const std::string &name, bool enable);

        /**
         * 更新触摸的位置，屏幕坐标
         * @param x
         * @param y
         */
        void UpdateTargetPos(int x, int y);

        /**
         * GLRender回调
         * @param type
         */
        void OnCallback(CallbackType type) override;

        static void SetFFmpegDebug(bool debug);

        void SetCalibrateParams(int boardSizeWidth, int boardSizeHeight,
                                float boardSquareSizeWidth,
                                float boardSquareSizeHeight,
                                float markerSizeWidth,
                                float markerSizeHeight);

        std::string GetParams(const std::string& paramsGroupName);

    private:
        /**
         * 缓存Jni方法，将从C/C++回调到Java的方法缓存下来
         * @param env
         */
        void cacheJniMethod(JNIEnv *env);

        void releaseJni(JNIEnv *env);

        static void ffmpegLogCallback(void *ptr, int level, const char *fmt, va_list vl);

    private:
        // jni
        JavaVM *m_JavaVM{};
        jobject m_Thiz{};
        jmethodID m_SetPreviewTextureMethod{};
        jmethodID m_StartPreviewMethod{};
        jmethodID m_StopPreviewMethod{};
        jmethodID m_UpdateFrameMethod{};
        jmethodID m_CreateImageReaderMethod{};
        jmethodID m_ConfigEncoderMethod{};
        jmethodID m_LoadAssetsMethod{};
        jmethodID m_PostInfoMethod{};

        std::atomic_bool m_previewing;
        std::atomic_bool m_rendering;

        // 渲染器
        std::shared_ptr<GLRender> m_render;

        int m_renderDelayCount;
    };

}