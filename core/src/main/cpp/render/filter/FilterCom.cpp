//
// Created by caolong on 2020/8/10.
//

#include <render/filter/FilterCom.hpp>
#include <render/texture/Texture.hpp>

using namespace clt;

bool FilterWithShader::Init(std::shared_ptr<SquareModel> model) {
  assert(model != nullptr);

  m_model = model;
  m_varGroup = std::make_shared<VarGroup>();

  loadShader();
  registerVar();

  return true;
}

void FilterWithShader::DeInit() {
  m_shader = nullptr;
  m_varGroup = nullptr;
  m_model = nullptr;
}

bool FilterWithShader::VarIn(const std::string &name) {
  return m_varGroup->VarIn(name);
}

void FilterWithShader::VarSet(const std::string &varName, const Var &var) {
  m_varGroup->VarSet(varName, var);
}

void FilterWithShader::Filter() {
  assert(m_input != nullptr);

  m_model->Bind(m_shader->PositionAttributeLocation(),
                m_shader->TexCoordinateAttributeLocation());
  m_shader->Use();
  m_input->Bind(m_shader->SamplerUniformLocation());
  updateValue();
  m_model->Draw();
  m_input->UnBind();
}
