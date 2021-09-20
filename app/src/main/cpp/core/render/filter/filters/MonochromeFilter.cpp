//
// Created by caolong on 2020/7/21.
//

#include <res/ResManager.hpp>
#include <render/filter/filters/MonochromeFilter.hpp>
#include <utils/Utils.hpp>

using namespace clt;

MonochromeFilter::MonochromeFilter()
    : FilterWithShader(target),
      m_filterColor({0.6f, 0.45f, 0.3f, 1.0f}),
      m_intensity(1.0f) {

}

void MonochromeFilter::loadShader() {
  m_shader = ResManager::Self()->LoadShader("monochrome");
  assert(m_shader != nullptr);

  m_uniformLocFilterColor = glGetUniformLocation(m_shader->ProgramId(), "uFilterColor");
  m_intensity = glGetUniformLocation(m_shader->ProgramId(), "uIntensity");
}

void MonochromeFilter::updateValue() {
  m_shader->SetFloat4(m_uniformLocFilterColor, m_filterColor);
  m_shader->SetFloat1(m_uniformLocIntensity, m_intensity);
}

void MonochromeFilter::registerVar() {
  m_varGroup->VarRegister(var_filter_color,
                          {
                              [this](const Var &var) {
                                setFilterColor(var.ToFloat());
                              },
                              Float3TypeTag{}
                          });

  m_varGroup->VarRegister(var_intensity,
                          {
                              [this](const Var &var) {
                                setIntensity(var.ToFloat());
                              },
                              Float1TypeTag{}
                          });
}

MonochromeFilter &MonochromeFilter::setFilterColor(const Float &color) {
  float r = Utils::Clamp(color.r, 0.0f, 1.0f);
  float g = Utils::Clamp(color.g, 0.0f, 1.0f);
  float b = Utils::Clamp(color.b, 0.0f, 1.0f);
  m_filterColor = {r, g, b, 1.0f};
  return *this;
}

MonochromeFilter &MonochromeFilter::setIntensity(const Float &intensity) {
  m_intensity = {Utils::Clamp(intensity.x, 0.0f, 1.0f)};
  return *this;
}