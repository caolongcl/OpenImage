//
// Created by caolong on 2020/8/10.
//

#pragma once

#include <render/filter/FilterCom.hpp>

namespace clt {
  /**
   * 随时间变化的颜色
   */
  class TimeColorFilter : public FilterWithShader {
  ClassDeclare(TimeColorFilter)

  public:
    TimeColorFilter();

    ~TimeColorFilter() = default;

  private:
    void loadShader() override;

    void updateValue() override;

    void registerVar() override {};

  private:
    long m_startTime;

    GLint m_uniformTime{};
  };
}