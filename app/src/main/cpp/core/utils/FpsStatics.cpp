//
// Created by caolong on 2021/9/19.
//

#include <utils/Utils.hpp>
#include <utils/FpsStatics.hpp>

using namespace clt;

void FpsStatics::UpdateStatistics() {
  // 统计帧率
  if (m_lastFrameTime == -1) {
    m_lastFrameTime = Utils::CurTimeMilli();
    m_framesStartPeriod = m_lastFrameTime;
    m_framesCount = 0;
    return;
  }
  long curFrameTime = Utils::CurTimeMilli();

  int fps = static_cast<int>(1000.0f / static_cast<float>(curFrameTime - m_lastFrameTime));
  if (fps > m_maxFps)
    m_maxFps = fps;
  if (fps < m_minFps)
    m_minFps = fps;

  m_lastFrameTime = curFrameTime;

  m_framesCount++;

  std::stringstream ss;

  if (curFrameTime - m_framesStartPeriod >= 1000 * 5) { //5s计算一次平均值
    m_avgFps = static_cast<int>(static_cast<float>(m_framesCount) * 1000.0f /
                                static_cast<float>(curFrameTime - m_framesStartPeriod));
    m_framesStartPeriod = curFrameTime;
    m_framesCount = 0;
    ss << "fps:" << fps << " (5s min:" << m_minFps << " max:" << m_maxFps << " avg:" << m_avgFps
       << ")";

    m_fps = std::move(ss.str());

    m_minFps = 9999;
    m_maxFps = 0;
  } else {
    ss << "fps:" << fps << " (5s min:" << m_minFps << " max:" << m_maxFps << " avg:" << m_avgFps
       << ")";
    m_fps = std::move(ss.str());
  }
}
