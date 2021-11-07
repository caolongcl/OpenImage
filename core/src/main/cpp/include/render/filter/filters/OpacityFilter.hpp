//
// Created by caolong on 2020/8/10.
//

#pragma once

#include <render/filter/FilterCom.hpp>

namespace clt {
  /**
   * opacity : 0.0f ~ 1.0f , default 1.0f
   */
  class OpacityFilter : public FilterWithShader {
  ClassDeclare(OpacityFilter)
  VarDeclare(opacity)
  public:
    OpacityFilter();

    ~OpacityFilter() = default;

  private:
    void loadShader() override;

    void updateValue() override;

    void registerVar() override;

    OpacityFilter &setOpacity(const Float &opacity);

  private:
    Float1 m_opacity;
    GLint m_uniformLocOpacity{};
  };

}