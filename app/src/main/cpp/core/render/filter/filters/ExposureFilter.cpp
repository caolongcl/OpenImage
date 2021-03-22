//
// Created by caolong on 2020/7/20.
//

#include <res/ResManager.hpp>
#include <render/filter/filters/ExposureFilter.hpp>
#include <res/Shader.hpp>
#include <utils/Utils.hpp>

using namespace clt;

ExposureFilter::ExposureFilter()
        : FilterWithShader(target),
          m_exposure(0.0f) {
}

void ExposureFilter::loadShader() {
    m_shader = ResManager::Self()->LoadShader("exposure");
    assert(m_shader != nullptr);

    m_uniformLocExposure = glGetUniformLocation(m_shader->ProgramId(), "uExposure");
}

void ExposureFilter::updateValue() {
    m_shader->SetFloat1(m_uniformLocExposure, m_exposure);
}

void ExposureFilter::registerVar() {
    m_varGroup->VarRegister(var_exposure, {
            [this](const Var &var) {
                setExposure(var.ToFloat());
            },
            Float1TypeTag{}
    });
}

ExposureFilter &ExposureFilter::setExposure(const Float &exposure) {
    m_exposure = Utils::Clamp(exposure.x, -10.f, 10.f);
    return *this;
}