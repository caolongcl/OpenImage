//
// Created by caolong on 2020/8/10.
//

#include <res/ResManager.hpp>
#include <utils/Utils.hpp>
#include <render/filter/filters/FireFilter.hpp>

using namespace clt;

FireFilter::FireFilter()
        : FilterWithShader(target),
          m_startTime(Utils::CurTimeMilli()) {

}

void FireFilter::loadShader() {
    m_shader = ResManager::Self()->LoadShader("fire");
    assert(m_shader != nullptr);

    m_uniformLocInputSize = glGetUniformLocation(m_shader->ProgramId(), "uInputSize");
    m_uniformTime = glGetUniformLocation(m_shader->ProgramId(), "uTime");
}

void FireFilter::updateValue() {
    assert(m_input != nullptr);

    m_shader->SetFloat2(m_uniformLocInputSize, Float2(m_input->Width(), m_input->Height()));

    long curTime = Utils::CurTimeMilli();
    m_shader->SetFloat1(m_uniformTime, {(float) (curTime - m_startTime) / 1000.0f});
}

void FireFilter::registerVar() {

}