//
// Created by caolong on 2020/8/10.
//

#pragma once

#include <render/filter/FilterCom.hpp>

namespace clt {
  /**
   * 给每个颜色通道设置一个比例
   */
  class RGBFilter : public FilterWithShader {
  ClassDeclare(RGBFilter)
  VarDeclare(r)
  VarDeclare(g)
  VarDeclare(b)
  public:
    RGBFilter();

    ~RGBFilter() = default;

  private:
    void loadShader() override;

    void updateValue() override;

    void registerVar() override;

    RGBFilter &setR(const Float &r);

    RGBFilter &setG(const Float &g);

    RGBFilter &setB(const Float &b);

  private:
    Float3 m_color;

    GLint m_uniformLocRGB{};
  };
}