//
// Created by caolong on 2020/7/21.
//

#include <res/ResManager.hpp>
#include <render/filter/filters/WhirlpoolFilter.hpp>
#include <utils/Utils.hpp>

using namespace clt;

WhirlpoolFilter::WhirlpoolFilter()
    : FilterWithShader(target),
      m_angle(30.f),
      m_radius(800.0f),
      m_center{800.0f, 800.0f},
      m_ratio{1.0, 1.0} {

}

void WhirlpoolFilter::loadShader() {
  m_shader = ResManager::Self()->LoadShader("whirlpool");
  assert(m_shader != nullptr);

  m_uniformLocAngle = glGetUniformLocation(m_shader->ProgramId(), "uAngle");
  m_uniformLocRadius = glGetUniformLocation(m_shader->ProgramId(), "uRadius");
  m_uniformLocInputSize = glGetUniformLocation(m_shader->ProgramId(), "uInputSize");
  m_uniformLocCenter = glGetUniformLocation(m_shader->ProgramId(), "uCenter");
}

void WhirlpoolFilter::updateValue() {
  assert(m_input != nullptr);

  m_shader->SetFloat1(m_uniformLocAngle, m_angle);
  m_shader->SetFloat1(m_uniformLocRadius, m_radius);
  m_shader->SetFloat2(m_uniformLocInputSize, Float2(m_input->Width(), m_input->Height()));
  m_shader->SetFloat2(m_uniformLocCenter, m_center);
}

void WhirlpoolFilter::registerVar() {
  m_varGroup->VarRegister(var_angle,
                          {
                              [this](const Var &var) {
                                setAngle(var.ToFloat());
                              },
                              Float1TypeTag{}
                          });
  m_varGroup->VarRegister(var_radius,
                          {
                              [this](const Var &var) {
                                setRadius(var.ToFloat());
                              },
                              Float1TypeTag{}
                          });
  m_varGroup->VarRegister(var_target_position,
                          {
                              [this](const Var &var) {
                                setCenter(var.ToFloat());
                              },
                              Float2TypeTag{}
                          });
  m_varGroup->VarRegister(var_position_ratio,
                          {
                              [this](const Var &var) {
                                setRatio(var.ToFloat());
                              },
                              Float2TypeTag{}
                          });
}

WhirlpoolFilter &WhirlpoolFilter::setAngle(const Float &angle) {
  m_angle.x = angle.x;
  return *this;
}

WhirlpoolFilter &WhirlpoolFilter::setRadius(const Float &radius) {
  assert(m_input != nullptr);

  int length = std::min(m_input->Width(), m_input->Height());
  m_radius.x = Utils::Clamp(radius.x * m_ratio.x, 64, (float) length);
  return *this;
}

WhirlpoolFilter &WhirlpoolFilter::setCenter(const Float &center) {
  assert(m_input != nullptr);

  m_center.x = Utils::Clamp(center.x * m_ratio.x, 10.0f, m_input->Width() - 10.f);
  m_center.y = Utils::Clamp(center.y * m_ratio.y, 10.0f, m_input->Height() - 10.f);
  return *this;
}

WhirlpoolFilter &WhirlpoolFilter::setRatio(const Float &ratio) {
  m_ratio.x = ratio.x;
  m_ratio.y = ratio.y;

  return *this;
}