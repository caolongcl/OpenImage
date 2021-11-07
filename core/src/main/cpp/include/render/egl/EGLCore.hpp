//
// Created by caolong on 2019/7/1.
//

#pragma once

#include <softarch/std.hpp>
#include <render/egl/IEGL.hpp>
#include <softarch/GLUtils.hpp>
#include <softarch/IComFunc.hpp>

namespace clt {
  /**
   * 封装所有和 EGL 相关的
   */
  class EGLCore final : public IComFunc<>,
                        public IEGL {
  public:
    EGLCore();

    EGLCore(EGLDisplay, EGLConfig, EGLContext);

    ~EGLCore() noexcept;

    bool Init() override;

    void DeInit() override;

    ///
    void CreateWindowSurface(const std::string &name, ANativeWindow *window) override;

    void CreateOffScreenSurface(const std::string &name) override;

    void DestroyWindowSurface(const std::string &name) override;

    void ActiveContext(const std::string &name) override;

    void DeActiveContext() override;

    void Update(const std::string &name) override;

    void SetPresentationTime(const std::string &name, khronos_stime_nanoseconds_t nsecs) override;

    std::shared_ptr<EGLCore> SharedEGL();

  private:
    // 平台无关类型EGLDisplay表示窗口
    bool createDisplay(EGLDisplay &display);

    // 配置窗口buffer参数
    bool chooseConfig(EGLDisplay display, EGLConfig &config);

    // 创建OpenGL ES 上下文
    bool createContext(EGLDisplay display, EGLConfig config, EGLContext &context);

    // 创建渲染窗口surface
    bool createWindowSurface(ANativeWindow *window, EGLDisplay display,
                             EGLConfig config, EGLSurface &surface);

    // 创建离屏渲染surface
    static bool chooseOffScreenConfig(EGLDisplay display, EGLConfig &config);

    static bool createOffScreenSurface(EGLDisplay display,
                                       EGLConfig config, EGLSurface &surface);

    // 激活上下文
    static bool activeContext(EGLDisplay display, EGLSurface draw,
                              EGLSurface read, EGLContext context);

    // 交换前后缓冲，更新画面
    static bool swapBuffer(EGLDisplay display, EGLSurface surface);

    static bool querySurfaceAttrb(EGLContext context, EGLSurface surface,
                                  EGLint what, EGLint &value);

    static bool destroySurface(EGLDisplay display, EGLSurface surface);

    void printEglInfo();

    void destroyContext();

    bool setPresentationTime(EGLSurface &surface, khronos_stime_nanoseconds_t nsecs);

    EGLCore(const EGLCore &) = delete;

    EGLCore &operator=(const EGLCore &) = delete;

    EGLCore(EGLCore &&) = delete;

    EGLCore &operator=(EGLCore &&) = delete;

  private:
    EGLDisplay m_display;
    EGLConfig m_config;
    EGLContext m_context;

    /**
     * 可以创建多个 Surface 用于切换
     */
    std::unordered_map<std::string, EGLSurface> m_surfaces;
  };

}