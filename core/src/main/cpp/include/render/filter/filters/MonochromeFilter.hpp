//
// Created by caolong on 2020/8/10.
//

#pragma once

#include <render/filter/FilterCom.hpp>

namespace clt {
  class MonochromeFilter : public FilterWithShader {
  ClassDeclare(MonochromeFilter)
  VarDeclare(filter_color)
  VarDeclare(intensity)
  public:
    MonochromeFilter();

    ~MonochromeFilter() = default;

  private:
    void loadShader() override;

    void updateValue() override;

    void registerVar() override;

    MonochromeFilter &setFilterColor(const Float &color);

    MonochromeFilter &setIntensity(const Float &intensity);

  private:
    Color m_filterColor;
    Float1 m_intensity;

    GLint m_uniformLocFilterColor{};
    GLint m_uniformLocIntensity{};
  };
}
