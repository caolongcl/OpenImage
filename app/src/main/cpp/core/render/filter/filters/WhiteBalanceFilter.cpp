//
// Created by caolong on 2020/7/21.
//

#include <res/ResManager.hpp>
#include <render/filter/filters/WhiteBalanceFilter.hpp>

using namespace clt;

WhiteBalanceFilter::WhiteBalanceFilter()
        : FilterWithShader(target),
          m_temperature(5000.0f),
          m_tint(0.0f) {

}

void WhiteBalanceFilter::loadShader() {
    m_shader = ResManager::Self()->LoadShader("white_balance");
    assert(m_shader != nullptr);

    m_uniformLocTemperature = glGetUniformLocation(m_shader->ProgramId(), "uTemperature");
    m_uniformLocTint = glGetUniformLocation(m_shader->ProgramId(), "uTint");
}

void WhiteBalanceFilter::updateValue() {
    float temperature = m_temperature.x;
    if (temperature < 5000.0f) {
        temperature = (float) ((temperature - 5000.0f) * 0.0004f);
    } else {
        temperature = (float) ((temperature - 5000.0f) * 0.00006f);
    }
    m_shader->SetFloat1(m_uniformLocTemperature, temperature);
    m_shader->SetFloat1(m_uniformLocTint, m_tint);
}

void WhiteBalanceFilter::registerVar() {
    m_varGroup->VarRegister(var_temperature,
                            {
                                    [this](const Var &var) {
                                        setTemperature(var.ToFloat());
                                    },
                                    Float1TypeTag{}
                            });
    m_varGroup->VarRegister(var_tint,
                            {
                                    [this](const Var &var) {
                                        setTint(var.ToFloat());
                                    },
                                    Float1TypeTag{}
                            });
}

WhiteBalanceFilter &WhiteBalanceFilter::setTemperature(const Float &temperature) {
    m_temperature.x = temperature.x;
    return *this;
}

WhiteBalanceFilter &WhiteBalanceFilter::setTint(const Float &tint) {
    m_tint.x = (float) (tint.x / 100.0);
    return *this;
}