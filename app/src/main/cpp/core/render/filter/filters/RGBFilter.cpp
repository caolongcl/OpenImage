//
// Created by caolong on 2020/7/21.
//

#include <res/ResManager.hpp>
#include <render/filter/filters/RGBFilter.hpp>
#include <utils/Utils.hpp>

using namespace clt;

RGBFilter::RGBFilter()
        : FilterWithShader(target),
          m_color(0.0f, 0.0f, 0.0f) {

}

void RGBFilter::loadShader() {
    m_shader = ResManager::Self()->LoadShader("rgb");
    assert(m_shader != nullptr);

    m_uniformLocRGB = glGetUniformLocation(m_shader->ProgramId(), "uRGB");
}

void RGBFilter::updateValue() {
    m_shader->SetFloat3(m_uniformLocRGB, m_color);
}

void RGBFilter::registerVar() {
    m_varGroup->VarRegister(var_r,
                            {
                                    [this](const Var &var) {
                                        setR(var.ToFloat());
                                    },
                                    Float1TypeTag{}
                            });
    m_varGroup->VarRegister(var_g,
                            {
                                    [this](const Var &var) {
                                        setG(var.ToFloat());
                                    },
                                    Float1TypeTag{}
                            });
    m_varGroup->VarRegister(var_b,
                            {
                                    [this](const Var &var) {
                                        setB(var.ToFloat());
                                    },
                                    Float1TypeTag{}
                            });
}

RGBFilter &RGBFilter::setR(const Float &r) {
    m_color.r = Utils::Clamp(r.x, 0.0f, 1.0f);
    return *this;
}

RGBFilter &RGBFilter::setG(const Float &g) {
    m_color.g = Utils::Clamp(g.x, 0.0f, 1.0f);
    return *this;
}

RGBFilter &RGBFilter::setB(const Float &b) {
    m_color.b = Utils::Clamp(b.x, 0.0f, 1.0f);
    return *this;
}