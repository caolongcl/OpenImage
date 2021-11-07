//
// Created by caolong on 2020/7/20.
//

#include <res/ResManager.hpp>
#include <render/filter/filters/OpacityFilter.hpp>
#include <utils/Utils.hpp>

using namespace clt;

OpacityFilter::OpacityFilter()
    : FilterWithShader(target),
      m_opacity(1.0f) {
}

void OpacityFilter::loadShader() {
  m_shader = ResManager::Self()->LoadShader("opacity");
  assert(m_shader != nullptr);

  m_uniformLocOpacity = glGetUniformLocation(m_shader->ProgramId(), "uOpacity");
}

void OpacityFilter::updateValue() {
  m_shader->SetFloat1(m_uniformLocOpacity, m_opacity);
}

void OpacityFilter::registerVar() {
  m_varGroup->VarRegister(var_opacity,
                          {
                              [this](const Var &var) {
                                setOpacity(var.ToFloat());
                              },
                              Float1TypeTag{}
                          });
}

OpacityFilter &OpacityFilter::setOpacity(const Float &opacity) {
  m_opacity = {Utils::Clamp(opacity.x, 0.0f, 1.0f)};
  return *this;
}