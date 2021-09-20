//
// Created by caolong on 2020/8/10.
//

#pragma once

#include <render/filter/FilterCom.hpp>

namespace clt {

  /**
   * 曝光: -10.0 ~ 10.0, 默认为 0.0
   */
  class ExposureFilter : public FilterWithShader {
  ClassDeclare(ExposureFilter)
  VarDeclare(exposure)
  public:
    ExposureFilter();

    ~ExposureFilter() = default;

  private:
    void loadShader() override;

    void updateValue() override;

    void registerVar() override;

    ExposureFilter &setExposure(const Float &exposure);

  private:
    Float1 m_exposure;
    GLint m_uniformLocExposure{};
  };
}
