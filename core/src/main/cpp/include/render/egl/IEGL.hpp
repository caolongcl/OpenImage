//
// Created by caolong on 2020/7/29.
//

#pragma once

#include <softarch/std.hpp>
#include <softarch/GLUtils.hpp>

namespace clt {

  /**
  * 需要对外提供EGL功能对象实现
  */
  struct IEGL {
    IEGL() = default;

    virtual ~IEGL() = default;

    virtual void CreateWindowSurface(const std::string &name, ANativeWindow *window) = 0;

    virtual void CreateOffScreenSurface(const std::string &name) = 0;

    virtual void DestroyWindowSurface(const std::string &name) = 0;

    virtual void ActiveContext(const std::string &name) = 0;

    virtual void DeActiveContext() = 0;

    virtual void Update(const std::string &name) = 0;

    virtual void
    SetPresentationTime(const std::string &name, khronos_stime_nanoseconds_t nsecs) = 0;
  };

}