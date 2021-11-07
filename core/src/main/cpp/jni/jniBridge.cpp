//
// Created by caolong on 2019/7/7.
//

#include <jni/PreviewController.hpp>
#include <softarch/VarType.hpp>
#include <softarch/ComType.hpp>
#include <jni/JniUtils.hpp>
#include <ncnn/gpu.h>

using namespace clt;

/**
 * Preview Control native method
 */
static std::shared_ptr<PreviewController> gPreviewController = nullptr;
static jobject gThiz = nullptr;
static ANativeWindow *gWindow = nullptr;
static ANativeWindow *gWindowCapture = nullptr;
static ANativeWindow *gWindowRecord = nullptr;

#define CheckPreviewController(pos) \
if (gPreviewController == nullptr) {Log::w(Log::JNI_TAG, "PreviewController don`t exist in %s", #pos); return;}

#define CheckPreviewControllerRet(pos) \
if (gPreviewController == nullptr) {Log::w(Log::JNI_TAG, "PreviewController don`t exist in %s", #pos); return nullptr;}


extern "C" JNIEXPORT
void JNICALL NativeCreate(JNIEnv *env, jobject thiz) {
  if (gPreviewController == nullptr) {
    gPreviewController = std::make_shared<PreviewController>();
  }

  assert(gPreviewController != nullptr);

  JavaVM *javaVM = nullptr;
  env->GetJavaVM(&javaVM);
  gThiz = env->NewGlobalRef(thiz);

  gPreviewController->Create(javaVM, gThiz);
}

extern "C" JNIEXPORT
void JNICALL NativeDestroy(JNIEnv *env, jobject thiz) {
  CheckPreviewController(NativeDestroy);

  gPreviewController->Destroy();

  if (gThiz != nullptr) {
    env->DeleteGlobalRef(gThiz);
    gThiz = nullptr;
  }

  gPreviewController = nullptr;
}

extern "C" JNIEXPORT
void JNICALL NativeInit(JNIEnv *env, jobject thiz) {
  CheckPreviewController(NativeInit);

  gPreviewController->Init();
}

extern "C" JNIEXPORT
void JNICALL NativeDeInit(JNIEnv *env, jobject thiz) {
  CheckPreviewController(NativeDeInit);

  gPreviewController->DeInit();

  if (gWindowCapture != nullptr) {
    ANativeWindow_release(gWindowCapture);
    gWindowCapture = nullptr;
  }

  if (gWindowRecord != nullptr) {
    ANativeWindow_release(gWindowRecord);
    gWindowRecord = nullptr;
  }

  if (gWindow != nullptr) {
    ANativeWindow_release(gWindow);
    gWindow = nullptr;
  }
}

extern "C" JNIEXPORT
void JNICALL NativeSetPreviewMode(JNIEnv *env,
                                  jobject thiz,
                                  jint rotate, jint ratio,
                                  jboolean vFlip, jboolean vHlip,
                                  jint width, jint height) {
  CheckPreviewController(NativeSetPreviewMode);

  gPreviewController->SetPreviewMode(rotate, ratio,
                                     vFlip == JNI_TRUE, vHlip == JNI_TRUE,
                                     width, height);
}

extern "C" JNIEXPORT
void JNICALL NativeStart(JNIEnv *env, jobject thiz) {
  CheckPreviewController(NativeStart);

  gPreviewController->Start();
}

extern "C" JNIEXPORT
void JNICALL NativeResume(JNIEnv *env, jobject thiz) {
  CheckPreviewController(NativeResume);

  gPreviewController->Resume();
}

extern "C" JNIEXPORT
void JNICALL NativePause(JNIEnv *env, jobject thiz) {
  CheckPreviewController(NativePause);

  gPreviewController->Pause();
}

extern "C" JNIEXPORT
void JNICALL NativeStop(JNIEnv *env, jobject thiz) {
  CheckPreviewController(NativeStop);

  gPreviewController->Stop();
}

extern "C" JNIEXPORT
void JNICALL NativeNotifyFrameAvailable(JNIEnv *env,
                                        jobject thiz) {
  CheckPreviewController(NativeNotifyFrameAvailable);

  gPreviewController->NotifyFrameAvailable();
}

extern "C" JNIEXPORT
void JNICALL NativeSetSurface(JNIEnv *env,
                              jobject thiz,
                              jint type,
                              jobject surface) {
  CheckPreviewController(NativeSetSurface);

  // 创建本地 Window
  ANativeWindow *window = nullptr;
  if (type == Constants::NATIVE_WINDOW_MAIN_TYPE) {
    if (gWindow != nullptr) {
      ANativeWindow_release(gWindow);
      gWindow = nullptr;
    }

    if (surface != nullptr) {
      gWindow = ANativeWindow_fromSurface(env, surface);
      window = gWindow;
    }
  } else if (type == Constants::NATIVE_WINDOW_CAPTURE_TYPE) {
    if (gWindowCapture != nullptr) {
      ANativeWindow_release(gWindowCapture);
      gWindowCapture = nullptr;
    }

    if (surface != nullptr) {
      gWindowCapture = ANativeWindow_fromSurface(env, surface);
      window = gWindowCapture;
    }
  } else if (type == Constants::NATIVE_WINDOW_RECORD_TYPE) {
    if (gWindowRecord != nullptr) {
      ANativeWindow_release(gWindowRecord);
      gWindowRecord = nullptr;
    }

    if (surface != nullptr) {
      gWindowRecord = ANativeWindow_fromSurface(env, surface);
      window = gWindowRecord;
    }
  }

  if (window != nullptr) {
    gPreviewController->SetSurface(type, window);
  }
}

extern "C" JNIEXPORT
void JNICALL NativeSetSurfaceSize(JNIEnv *env,
                                  jobject thiz,
                                  jint width,
                                  jint height) {
  CheckPreviewController(NativeSetSurfaceSize);

  gPreviewController->SetSurfaceSize(width, height);
}

extern "C" JNIEXPORT
void JNICALL NativeCapture(JNIEnv *env,
                           jobject thiz) {
  CheckPreviewController(NativeCapture);

  gPreviewController->Capture();
}

extern "C" JNIEXPORT
void JNICALL NativeSetFloatVar(JNIEnv *env,
                               jobject thiz,
                               jstring name,
                               jfloatArray var) {
  CheckPreviewController(NativeSetFloatVar);

  if (name == nullptr || var == nullptr)
    return;

  int count = env->GetArrayLength(var);
  if (count <= 0)
    return;

  jboolean isCopy = JNI_TRUE;
  jfloat *elems = env->GetFloatArrayElements(var, &isCopy);

  JniUtils::UseString(env, name, [elems, count](const std::string &varName) {
    gPreviewController->SetVar(varName, Float{elems, count});
  });

  env->ReleaseFloatArrayElements(var, elems, 0);
}

extern "C" JNIEXPORT
void JNICALL NativeSetBoolVar(JNIEnv *env,
                              jobject thiz,
                              jstring name,
                              jboolean var) {
  CheckPreviewController(NativeSetBoolVar);

  if (name == nullptr)
    return;

  JniUtils::UseString(env, name, [var](const std::string &varName) {
    gPreviewController->SetVar(varName, Boolean{var == JNI_TRUE});
  });
}

extern "C" JNIEXPORT
void JNICALL NativeSetIntVar(JNIEnv *env,
                             jobject thiz,
                             jstring name,
                             jintArray var) {
  CheckPreviewController(NativeSetIntVar);

  if (name == nullptr || var == nullptr)
    return;

  int count = env->GetArrayLength(var);
  if (count <= 0)
    return;

  jboolean isCopy = JNI_TRUE;
  jint *elems = env->GetIntArrayElements(var, &isCopy);

  JniUtils::UseString(env, name, [elems, count](const std::string &varName) {
    gPreviewController->SetVar(varName, Integer{elems, count});
  });

  env->ReleaseIntArrayElements(var, elems, 0);
}

extern "C" JNIEXPORT
void JNICALL NativeSetStringVar(JNIEnv *env,
                                jobject thiz,
                                jstring name,
                                jstring var) {
  CheckPreviewController(NativeSetStringVar);

  if (name == nullptr || var == nullptr)
    return;

  JniUtils::UseString(env, name, [env, var](const std::string &varName) {
    gPreviewController->SetString(varName, JniUtils::GetString(env, var));
  });
}

extern "C" JNIEXPORT
void JNICALL NativeEnableFilter(JNIEnv *env,
                                jobject thiz,
                                jstring name,
                                jboolean enable) {
  CheckPreviewController(NativeEnableFilter);

  JniUtils::UseString(env, name, [enable](const std::string &varName) {
    gPreviewController->EnableFilter(varName, enable == JNI_TRUE);
  });
}

extern "C" JNIEXPORT
void JNICALL NativeEnableProcess(JNIEnv *env,
                                 jobject thiz,
                                 jstring name,
                                 jboolean enable) {
  CheckPreviewController(NativeEnableProcess);

  JniUtils::UseString(env, name, [enable](const std::string &varName) {
    gPreviewController->EnableProcess(varName, enable == JNI_TRUE);
  });
}

extern "C" JNIEXPORT
void JNICALL NativeRecord(JNIEnv *env,
                          jobject thiz,
                          jboolean start, jint fps, jfloat factor) {
  CheckPreviewController(NativeRecord);

  gPreviewController->Record(start == JNI_TRUE, fps, factor);
}

extern "C" JNIEXPORT
void JNICALL NativeUpdateTargetPos(JNIEnv *env,
                                   jobject thiz,
                                   jint x, jint y) {
  CheckPreviewController(NativeUpdateTargetPos);

  gPreviewController->UpdateTargetPos(x, y);
}

extern "C" JNIEXPORT
void JNICALL NativeSetFFmpegDebug(JNIEnv *env,
                                  jobject thiz,
                                  jboolean debug) {
  CheckPreviewController(NativeSetFFmpegDebug);

  gPreviewController->SetFFmpegDebug(debug == JNI_TRUE);
}

extern "C" JNIEXPORT
void JNICALL NativeSetCalibrateParams(JNIEnv *env,
                                      jobject thiz,
                                      jint boardSizeWidth, jint boardSizeHeight,
                                      jfloat boardSquareSizeWidth, jfloat boardSquareSizeHeight,
                                      jfloat markerSizeWidth, jfloat markerSizeHeight) {
  CheckPreviewController(NativeSetCalibrateParams);

  gPreviewController->SetCalibrateParams(boardSizeWidth, boardSizeHeight,
                                         boardSquareSizeWidth, boardSquareSizeHeight,
                                         markerSizeWidth, markerSizeHeight);
}

extern "C" JNIEXPORT
jstring JNICALL NativeGetParams(JNIEnv *env,
                                jobject thiz,
                                jstring paramGroupName) {
  CheckPreviewControllerRet(NativeGetParams);

  std::string params = gPreviewController->GetParams(JniUtils::GetString(env, paramGroupName));
  return env->NewStringUTF(params.c_str());
}

// 为注册的jni接口的类名
static const char *JNIREG_CLASS = "com/cwdx/core/preview/PreviewController";
static JNINativeMethod sMethods[] = {
    {"NativeInit",                 "()V",                                     (void *) NativeInit},
    {"NativeDeInit",               "()V",                                     (void *) NativeDeInit},
    {"NativeCreate",               "()V",                                     (void *) NativeCreate},
    {"NativeDestroy",              "()V",                                     (void *) NativeDestroy},
    {"NativeSetPreviewMode",       "(IIZZII)V",                               (void *) NativeSetPreviewMode},
    {"NativeStart",                "()V",                                     (void *) NativeStart},
    {"NativeResume",               "()V",                                     (void *) NativeResume},
    {"NativePause",                "()V",                                     (void *) NativePause},
    {"NativeStop",                 "()V",                                     (void *) NativeStop},
    {"NativeNotifyFrameAvailable", "()V",                                     (void *) NativeNotifyFrameAvailable},
    {"NativeSetSurface",           "(ILandroid/view/Surface;)V",              (void *) NativeSetSurface},
    {"NativeSetSurfaceSize",       "(II)V",                                   (void *) NativeSetSurfaceSize},
    {"NativeCapture",              "()V",                                     (void *) NativeCapture},
    {"NativeRecord",               "(ZIF)V",                                  (void *) NativeRecord},
    {"NativeSetFloatVar",          "(Ljava/lang/String;[F)V",                 (void *) NativeSetFloatVar},
    {"NativeSetBoolVar",           "(Ljava/lang/String;Z)V",                  (void *) NativeSetBoolVar},
    {"NativeSetIntVar",            "(Ljava/lang/String;[I)V",                 (void *) NativeSetIntVar},
    {"NativeSetStringVar",         "(Ljava/lang/String;Ljava/lang/String;)V", (void *) NativeSetStringVar},
    {"NativeEnableFilter",         "(Ljava/lang/String;Z)V",                  (void *) NativeEnableFilter},
    {"NativeEnableProcess",        "(Ljava/lang/String;Z)V",                  (void *) NativeEnableProcess},
    {"NativeUpdateTargetPos",      "(II)V",                                   (void *) NativeUpdateTargetPos},
    {"NativeSetCalibrateParams",   "(IIFFFF)V",                               (void *) NativeSetCalibrateParams},
    {"NativeGetParams",            "(Ljava/lang/String;)Ljava/lang/String;",  (void *) NativeGetParams},
};

static const char *JNIREG_FFMPEG_CLASS = "com/cwdx/core/preview/FFmpegUtils";
static JNINativeMethod sFFmpegMethods[] = {
    {"NativeSetFFmpegDebug", "(Z)V", (void *) NativeSetFFmpegDebug},
};

static int registerNativeMethods(JNIEnv *env, const char *className,
                                 JNINativeMethod *gMethods, int numMethods) {
  jclass clazz;
  clazz = env->FindClass(className);
  if (clazz == nullptr) {
    return JNI_FALSE;
  }
  if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
    return JNI_FALSE;
  }
  env->DeleteLocalRef(clazz);

  return JNI_TRUE;
}

// JNI_OnLoad
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
  JNIEnv *env = nullptr;
  jint result = -1;

  if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
    return -1;
  }
  assert(env != nullptr);

  if (!registerNativeMethods(env,
                             JNIREG_CLASS,
                             sMethods,
                             sizeof(sMethods) / sizeof(JNINativeMethod))) {
    return -1;
  }

  if (!registerNativeMethods(env,
                             JNIREG_FFMPEG_CLASS,
                             sFFmpegMethods,
                             sizeof(sFFmpegMethods) / sizeof(JNINativeMethod))) {
    return -1;
  }

  ncnn::create_gpu_instance();

  /* success -- return valid version number */
  result = JNI_VERSION_1_6;

  return result;
}

JNIEXPORT void JNI_OnUnload(JavaVM *vm, void *reserved) {
  ncnn::destroy_gpu_instance();
}
