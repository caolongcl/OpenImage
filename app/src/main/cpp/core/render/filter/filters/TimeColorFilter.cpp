//
// Created by caolong on 2020/7/23.
//

#include <res/ResManager.hpp>
#include <render/filter/filters/TimeColorFilter.hpp>
#include <utils/Utils.hpp>

using namespace clt;

TimeColorFilter::TimeColorFilter()
        : FilterWithShader(target),
          m_startTime(Utils::CurTimeMilli()) {
}

void TimeColorFilter::loadShader() {
    m_shader = ResManager::Self()->LoadShader("time_color");
    assert(m_shader != nullptr);

    m_uniformTime = glGetUniformLocation(m_shader->ProgramId(), "uTime");
}

void TimeColorFilter::updateValue() {
    long curTime = Utils::CurTimeMilli();
    m_shader->SetFloat1(m_uniformTime, {(float) (curTime - m_startTime) / 1000.0f});
}