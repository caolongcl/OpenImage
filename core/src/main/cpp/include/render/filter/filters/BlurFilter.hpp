//
// Created by caolong on 2020/8/10.
//

#pragma once

#include <render/filter/FilterCom.hpp>

namespace clt {
  class BlurFilter : public FilterWithShader {
  ClassDeclare(BlurFilter)
  public:
    BlurFilter();

    ~BlurFilter() = default;

  private:
    void loadShader() override;

    void updateValue() override;

    void registerVar() override {}

  private:
    GLint m_uniformLocResolution{};
  };
}
