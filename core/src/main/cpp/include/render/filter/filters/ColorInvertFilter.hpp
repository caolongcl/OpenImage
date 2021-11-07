//
// Created by caolong on 2020/8/10.
//

#pragma once

#include <render/filter/FilterCom.hpp>

namespace clt {

  /**
   * 反色
   */
  class ColorInvertFilter : public FilterWithShader {
  ClassDeclare(ColorInvertFilter)

  public:
    ColorInvertFilter();

    ~ColorInvertFilter() = default;

  private:
    void loadShader() override;

    void updateValue() override {}

    void registerVar() override {}
  };
}
