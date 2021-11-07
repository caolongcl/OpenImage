//
// Created by caolong on 2020/8/10.
//

#pragma once

#include <render/filter/FilterCom.hpp>

namespace clt {
  /**
   * brightness : -1.0f ~ 1.0f, default 0.0f
   * contrast : 0.0f ~ 4.0f, default 1.0f
   * saturation : 0.0f ~ 2.0f, default 1.0f
   */
  class BaseFilter : public FilterWithShader {
  ClassDeclare(BaseFilter)
  VarDeclare(brightness)
  VarDeclare(contrast)
  VarDeclare(saturation)
  public:
    BaseFilter();

    ~BaseFilter() = default;

  private:
    void loadShader() override;

    void updateValue() override;

    void registerVar() override;

    BaseFilter &setBrightness(const Float &brightness);

    BaseFilter &setContrast(const Float &contrast);

    BaseFilter &setSaturation(const Float &saturation);

  private:
    Float1 m_brightness;
    Float1 m_contrast;
    Float1 m_saturation;

    GLint m_uniformLocBase{-1};
  };
}
