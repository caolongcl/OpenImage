//
// Created by caolong on 2020/7/20.
//

#include <res/ResManager.hpp>
#include <render/filter/filters/BlurFilter.hpp>

using namespace clt;

BlurFilter::BlurFilter()
        : FilterWithShader(target) {

}

void BlurFilter::loadShader() {
    m_shader = ResManager::Self()->LoadShader("blur");
    assert(m_shader != nullptr);

    m_uniformLocResolution = glGetUniformLocation(m_shader->ProgramId(), "uResolution");
}

void BlurFilter::updateValue() {
    Float3 value(m_output->Width(), m_output->Height(), 0.0f);
    m_shader->SetFloat3(m_uniformLocResolution, value);
}