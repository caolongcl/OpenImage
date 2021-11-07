//
// Created by caolong on 2019/7/7.
//
#pragma once

#include <softarch/std.hpp>

namespace clt {

#define CheckMethodIDExit(id, name) \
    if ((id)==nullptr) {\
        Log::e(Log::JNI_TAG, "CheckMethodIDExit, method:%s", #name);\
        exit(EXIT_FAILURE);}

  class JniUtils final {
  public:
    /**
     * 在其他线程执行jvm方法
     * @param vm
     * @param task
     * @param info
     * @return
     */
    static bool DoOutCurJvm(JavaVM *vm,
                            const std::function<void(JNIEnv *)> &task,
                            const char *info = "") {
      if (vm == nullptr) {
        Log::e(Log::JNI_TAG, "vm==null %s", info);
        return false;
      }

      JNIEnv *env;
      if (vm->AttachCurrentThread(&env, nullptr) != JNI_OK) {
        Log::e(Log::JNI_TAG, "AttachCurrentThread failed  %s", info);
        return false;
      }
      if (env == nullptr) {
        Log::e(Log::JNI_TAG, "env==null %s", info);
        return false;
      }

      task(env);

      if (vm->DetachCurrentThread() != JNI_OK) {
        Log::e(Log::JNI_TAG, "DetachCurrentThread failed %s", info);
        return false;
      }

      return true;
    }

    /**
     * 在JVM线程执行jvm方法
     * @param vm
     * @param task
     * @param info
     */
    static void DoInCurJvm(JavaVM *vm,
                           const std::function<void(JNIEnv *)> &task,
                           const char *info = "") {
      if (vm == nullptr) {
        Log::e(Log::JNI_TAG, "vm==null %s", info);
        return;
      }

      JNIEnv *env;
      vm->GetEnv((void **) &env, JNI_VERSION_1_6);
      if (env != nullptr) {
        task(env);
      }
    }

    static std::string GetString(JNIEnv *env, jstring name) {
      jboolean isCopy = JNI_TRUE;
      const char *nameChar = env->GetStringUTFChars(name, &isCopy);
      const int nameCharLen = env->GetStringUTFLength(name);
      std::string nameStr(nameChar, nameCharLen);
      env->ReleaseStringUTFChars(name, nameChar);

      return nameStr;
    }

    static void UseString(JNIEnv *env,
                          jstring name,
                          std::function<void(const std::string &varName)> &&task) {
      task(GetString(env, name));
    }
  };

}