//
// Created by caolong on 2020/8/10.
//

#pragma once

#include <softarch/std.hpp>

namespace clt {

/**
 * 定向Log到__android_log_print
 * ^(?!.*(Indicator|NcLib|SDM|CameraService|CamX|TCPBufferManager)).*$
 */
    class Log {
    public:
        template<typename ...Args>
        static void d(const std::string &tag, const std::string &format, Args &&... args) {
            print(Debug, tag, format, std::forward<Args>(args)...);
        }

        template<typename ...Args>
        static void w(const std::string &tag, const std::string &format, Args &&... args) {
            print(Warn, tag, format, std::forward<Args>(args)...);
        }

        template<typename ...Args>
        static void e(const std::string &tag, const std::string &format, Args &&... args) {
            print(Error, tag, format, std::forward<Args>(args)...);
        }

        template<typename ...Args>
        static void i(const std::string &tag, const std::string &format, Args &&... args) {
            print(Info, tag, format, std::forward<Args>(args)...);
        }

        template<typename ...Args>
        static void v(const std::string &tag, const std::string &format, Args &&... args) {
            print(Verbose, tag, format, std::forward<Args>(args)...);
        }

        static void tag(const std::string &point) {
            Log::d("InvokePoint", "%s", point.c_str());
        }

        // 空，用来替代调试时打印
        template<typename ...Args>
        static void n(const std::string &tag, const std::string &format, Args &&... args) {
        }

        // 只在调试版本出现
        using DebugTask = std::function<void()>;

        static void debug(DebugTask &&d) {
#ifdef __CLT_DEBUG__
            std::forward<DebugTask>(d)();
#endif
        }

    public:
        static const char *RENDER_TAG;
        static const char *JNI_TAG;
        static const char *PREVIEW_CTRL_TAG;
        static const char *EGL_TAG;
        static const char *GUI_TAG;
        static const char *RES_TAG;
        static const char *PROCESSOR_TAG;
        static const char *GLTHREAD_TAG;

    private:
        enum LogLevel {
            Null,
            Verbose = ANDROID_LOG_VERBOSE,
            Debug = ANDROID_LOG_DEBUG,
            Info = ANDROID_LOG_INFO,
            Warn = ANDROID_LOG_WARN,
            Error = ANDROID_LOG_ERROR,
            Fatal = ANDROID_LOG_FATAL
        };

        static const char *TAG;

        template<typename ...Args>
        static void print(LogLevel l,
                          const std::string &tag,
                          const std::string &format,
                          Args &&... args) {
#ifdef __LOG__
            std::string info = "[" + tag + "] " + format;
            __android_log_print(l, TAG, info.c_str(), std::forward<Args>(args)...);
#endif
        }

        static void print(LogLevel l,
                          const std::string &tag,
                          const std::string &format) {
#ifdef __LOG__
            std::string info = "[" + tag + "] " + format;
            __android_log_print(l, TAG, info.c_str(), "");
#endif
        }
    };

}
