//
// Created by caolong on 2020/8/10.
//

#pragma once

#include <render/filter/FilterCom.hpp>

namespace clt {
  /**
   * 锐化 -4.0 ~ 4.0, default 0.0
   */
  class SharpenFilter : public FilterWithShader {
  ClassDeclare(SharpenFilter)
  VarDeclare(sharpen)
  public:
    SharpenFilter();

    ~SharpenFilter() = default;

  private:
    void loadShader() override;

    void updateValue() override;

    void registerVar() override;

    SharpenFilter &setSharpen(const Float &sharpen);

  private:
    Float1 m_sharpen;

    GLint m_uniformLocSharpen{};
    GLint m_uniformLocWidthFactor{};
    GLint m_uniformLocHeightFactor{};
  };
}
