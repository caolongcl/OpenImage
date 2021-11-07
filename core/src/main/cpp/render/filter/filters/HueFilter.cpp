//
// Created by caolong on 2020/7/21.
//

#include <res/ResManager.hpp>
#include <render/filter/filters/HueFilter.hpp>

using namespace clt;

HueFilter::HueFilter()
    : FilterWithShader(target),
      m_hue(0.0f) {

}

void HueFilter::loadShader() {
  m_shader = ResManager::Self()->LoadShader("hue");
  assert(m_shader != nullptr);

  m_uniformLocHue = glGetUniformLocation(m_shader->ProgramId(), "uHue");
}

void HueFilter::updateValue() {
  m_shader->SetFloat1(m_uniformLocHue, m_hue);
}

void HueFilter::registerVar() {
  m_varGroup->VarRegister(var_hue,
                          {
                              [this](const Var &var) {
                                setHue(var.ToFloat());
                              },
                              Float1TypeTag{}
                          });
}

HueFilter &HueFilter::setHue(const Float &hue) {
  m_hue = (float) ((int) hue.x % 360) * (float) PI / 180.0f;
  return *this;
}