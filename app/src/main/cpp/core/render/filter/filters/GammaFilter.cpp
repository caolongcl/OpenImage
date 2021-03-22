//
// Created by caolong on 2020/7/21.
//

#include <res/ResManager.hpp>
#include <render/filter/filters/GammaFilter.hpp>
#include <utils/Utils.hpp>

using namespace clt;

GammaFilter::GammaFilter()
        : FilterWithShader(target),
          m_gamma(1.0f) {

}

void GammaFilter::loadShader() {
    m_shader = ResManager::Self()->LoadShader("gamma");
    assert(m_shader != nullptr);

    m_uniformLocGamma = glGetUniformLocation(m_shader->ProgramId(), "uGamma");
}

void GammaFilter::updateValue() {
    m_shader->SetFloat1(m_uniformLocGamma, m_gamma);
}

void GammaFilter::registerVar() {
    m_varGroup->VarRegister(var_gamma,
                            {
                                    [this](const Var &var) {
                                        setGamma(var.ToFloat());
                                    },
                                    Float1TypeTag{}
                            });
}

GammaFilter &GammaFilter::setGamma(const Float &gamma) {
    m_gamma = Utils::Clamp(gamma.x, 0.0f, 3.0f);
    return *this;
}