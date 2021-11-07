//
// Created by caolong on 2020/8/10.
//

#pragma once

#include <render/filter/FilterCom.hpp>

namespace clt {

  class WhiteBalanceFilter : public FilterWithShader {
  ClassDeclare(WhiteBalanceFilter)
  VarDeclare(temperature)
  VarDeclare(tint)
  public:
    WhiteBalanceFilter();

    ~WhiteBalanceFilter() = default;

  private:
    void loadShader() override;

    void updateValue() override;

    void registerVar() override;

    WhiteBalanceFilter &setTemperature(const Float &temperature);

    WhiteBalanceFilter &setTint(const Float &tint);

  private:
    Float1 m_temperature;
    Float1 m_tint;

    GLint m_uniformLocTemperature{};
    GLint m_uniformLocTint{};
  };
}
