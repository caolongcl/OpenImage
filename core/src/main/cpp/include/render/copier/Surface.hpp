//
// Created by caolong on 2020/8/14.
//

#pragma once

#include <softarch/std.hpp>
#include <softarch/IComFunc.hpp>

namespace clt {

  class Surface final
    : public IComFunc<ANativeWindow *> {
  ClassDeclare(Surface)
  public:
    Surface(std::string name);

    ~Surface() = default;

    bool Init(ANativeWindow *window) override;

    void DeInit() override;

    /**
     * 对应 eglMakeCurrent
     */
    void Prepare();

    /**
     * 对应 eglSwapBuffers
     */
    void Swap();

    /**
     * 对应 eglPresentationTimeANDROID
     * @param nsecs
     */
    void SetPresentationTime(khronos_stime_nanoseconds_t nsecs);

  private:
    std::string m_name;
  };

}
