//
// Created by caolong on 2020/8/10.
//

#pragma once

#include <softarch/std.hpp>

namespace clt {

  /**
   * 根据唯一的名字创建滤镜
   */
  class FilterWithShader;

  class FilterFactory {
  public:
    static std::shared_ptr<FilterWithShader> Create(const std::string &type);
  };

}
