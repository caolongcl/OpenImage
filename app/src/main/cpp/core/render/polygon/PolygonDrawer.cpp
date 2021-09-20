//
// Created by caolong on 2020/8/11.
//

#include <render/polygon/PolygonDrawer.hpp>

using namespace clt;

bool PolygonDrawer::Init() {
  m_varGroup = std::make_shared<VarGroup>();

  loadShader();
  registerVar();

  return true;
}

void PolygonDrawer::DeInit() {
  m_shader = nullptr;
  m_varGroup = nullptr;
}

bool PolygonDrawer::VarIn(const std::string &name) {
  return m_varGroup->VarIn(name);
}

void PolygonDrawer::VarSet(const std::string &varName, const Var &var) {
  m_varGroup->VarSet(varName, var);
}

void PolygonDrawer::Filter() {
  m_shader->Use();
  updateValue();
  draw();
}