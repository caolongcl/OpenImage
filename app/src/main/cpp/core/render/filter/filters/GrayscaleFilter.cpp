//
// Created by caolong on 2020/7/21.
//

#include <res/ResManager.hpp>
#include <render/filter/filters/GrayscaleFilter.hpp>

using namespace clt;

GrayscaleFilter::GrayscaleFilter()
    : FilterWithShader(target) {

}

void GrayscaleFilter::loadShader() {
  m_shader = ResManager::Self()->LoadShader("grayscale");
  assert(m_shader != nullptr);
}