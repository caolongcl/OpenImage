//
// Created by caolong on 2020/8/13.
//

#pragma once

namespace clt {
  /**
   * 定义 Copier 输出的类型，如输出到 ANativeWindow、Texture、pbbuffer
   * @tparam Args
   */
  template<typename ...Args>
  struct ICopierSurface {
    ICopierSurface() = default;

    virtual ~ICopierSurface() = default;

    /**
     * 注册Copier的输出Target
     * @param args
     */
    virtual void RegisterSurface(Args ...args) = 0;

    /**
     * Copier输出操作
     */
    virtual void Render() = 0;
  };
}
