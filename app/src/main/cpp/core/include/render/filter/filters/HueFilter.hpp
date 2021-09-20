//
// Created by caolong on 2020/8/10.
//

#pragma once

#include <render/filter/FilterCom.hpp>

namespace clt {
  /**
   * 调节色调
   */
  class HueFilter : public FilterWithShader {
  ClassDeclare(HueFilter)
  VarDeclare(hue)
  public:
    HueFilter();

    ~HueFilter() = default;

  private:
    void loadShader() override;

    void updateValue() override;

    void registerVar() override;

    HueFilter &setHue(const Float &hue);

  private:
    Float1 m_hue;
    GLint m_uniformLocHue{};
  };
}