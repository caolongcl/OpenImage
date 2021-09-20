//
// Created by caolong on 2020/8/10.
//

#pragma once

#include <render/filter/FilterCom.hpp>

namespace clt {
  /**
   * 灰度
   */
  class GrayscaleFilter : public FilterWithShader {
  ClassDeclare(GrayscaleFilter)

  public:
    GrayscaleFilter();

    ~GrayscaleFilter() = default;

  private:
    void loadShader() override;

    void updateValue() override {}

    void registerVar() override {}
  };

}
