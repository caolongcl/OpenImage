//
// Created by caolong on 2020/8/11.
//

#pragma once

#include <render/filter/FilterCom.hpp>

namespace clt {

  class PolygonDrawer : public FilterCom {
  public:
    PolygonDrawer(const std::string &type, const int priority = PriorityNormal)
      : FilterCom(type, priority) {};

    virtual ~PolygonDrawer() = default;

    virtual bool Init() override;

    virtual bool Update() override { return true; }

    virtual void DeInit() override;

    void Filter() override;

    bool VarIn(const std::string &name) override;

    void VarSet(const std::string &varName, const Var &var) override;

  protected:
    virtual void loadShader() = 0;

    virtual void updateValue() = 0;

    virtual void registerVar() = 0;

    virtual void draw() = 0;

    std::shared_ptr<Shader> m_shader;
    std::shared_ptr<VarGroup> m_varGroup;
  };

}