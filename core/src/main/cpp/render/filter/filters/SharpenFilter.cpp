//
// Created by caolong on 2020/7/21.
//

#include <res/ResManager.hpp>
#include <render/filter/filters/SharpenFilter.hpp>
#include <utils/Utils.hpp>

using namespace clt;

SharpenFilter::SharpenFilter()
    : FilterWithShader(target),
      m_sharpen(0.0f) {

}

void SharpenFilter::loadShader() {
  m_shader = ResManager::Self()->LoadShader("sharpen");
  assert(m_shader != nullptr);

  m_uniformLocSharpen = glGetUniformLocation(m_shader->ProgramId(), "uSharpen");
  m_uniformLocWidthFactor = glGetUniformLocation(m_shader->ProgramId(), "uWidthFactor");
  m_uniformLocHeightFactor = glGetUniformLocation(m_shader->ProgramId(), "uHeightFactor");
}

void SharpenFilter::updateValue() {
  m_shader->SetFloat1(m_uniformLocSharpen, m_sharpen);

  assert(m_output != nullptr);

  m_shader->SetFloat1(m_uniformLocWidthFactor, {1.0f / m_output->Width()});
  m_shader->SetFloat1(m_uniformLocHeightFactor, {1.0f / m_output->Height()});
}

void SharpenFilter::registerVar() {
  m_varGroup->VarRegister(var_sharpen,
                          {
                              [this](const Var &var) {
                                setSharpen(var.ToFloat());
                              },
                              Float1TypeTag{}
                          });
}

SharpenFilter &SharpenFilter::setSharpen(const Float &sharpen) {
  m_sharpen = {Utils::Clamp(sharpen.x, -4.0f, 4.0f)};
  return *this;
}