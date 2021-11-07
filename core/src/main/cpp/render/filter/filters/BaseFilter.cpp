//
// Created by caolong on 2020/6/22.
//

#include <res/ResManager.hpp>
#include <render/filter/filters/BaseFilter.hpp>
#include <res/Shader.hpp>
#include <utils/Utils.hpp>

using namespace clt;

BaseFilter::BaseFilter()
    : FilterWithShader(target),
      m_brightness(0.0f),
      m_contrast(1.0f),
      m_saturation(1.0f) {

}

void BaseFilter::loadShader() {
  m_shader = ResManager::Self()->LoadShader("base");
  assert(m_shader != nullptr);

  m_uniformLocBase = glGetUniformLocation(m_shader->ProgramId(), "uBase");
}

BaseFilter &BaseFilter::setBrightness(const Float &brightness) {
  m_brightness = Utils::Clamp(brightness.x, -1.0f, 1.0f);
  return *this;
}

BaseFilter &BaseFilter::setContrast(const Float &contrast) {
  m_contrast = Utils::Clamp(contrast.x, 0.0f, 4.0f);
  return *this;
}

BaseFilter &BaseFilter::setSaturation(const Float &saturation) {
  m_saturation = Utils::Clamp(saturation.x, 0.0f, 2.0f);
  return *this;
}

void BaseFilter::updateValue() {
  Float3 value{m_brightness.x, m_contrast.x, m_saturation.x};
  m_shader->SetFloat3(m_uniformLocBase, value);
}

void BaseFilter::registerVar() {
  m_varGroup->VarRegister(var_brightness, {
      [this](const Var &var) {
        setBrightness(var.ToFloat());
      },
      Float1TypeTag{}
  });

  m_varGroup->VarRegister(var_contrast, {
      [this](const Var &var) {
        setContrast(var.ToFloat());
      },
      Float1TypeTag{}
  });

  m_varGroup->VarRegister(var_saturation, {
      [this](const Var &var) {
        setSaturation(var.ToFloat());
      },
      Float1TypeTag{}
  });
}
