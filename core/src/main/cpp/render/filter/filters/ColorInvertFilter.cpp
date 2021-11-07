//
// Created by caolong on 2020/7/20.
//

#include <res/ResManager.hpp>
#include <render/filter/filters/ColorInvertFilter.hpp>

using namespace clt;

ColorInvertFilter::ColorInvertFilter()
    : FilterWithShader(target) {

}

void ColorInvertFilter::loadShader() {
  m_shader = ResManager::Self()->LoadShader("color_invert");
  assert(m_shader != nullptr);
}