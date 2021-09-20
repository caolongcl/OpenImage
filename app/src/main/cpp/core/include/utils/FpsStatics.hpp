//
// Created by caolong on 2021/9/19.
//

#pragma once

#include <softarch/IComFunc.hpp>

namespace clt {
  /**
   * 统计帧率
   */
  struct FpsStatics : public IComFunc<> {
    FpsStatics() : m_lastFrameTime(-1),
                   m_minFps(9999),
                   m_maxFps(0),
                   m_avgFps(0),
                   m_framesCount(-1),
                   m_framesStartPeriod(-1),
                   m_fps("") {}

    virtual ~FpsStatics() = default;

    bool Init() override { return true; }

    void DeInit() override {
      m_lastFrameTime = -1;
      m_minFps = 9999;
      m_maxFps = 0;
      m_avgFps = 0;
      m_framesCount = -1;
      m_framesStartPeriod = -1;
      m_fps = "";
    }

    void UpdateStatistics();

    long m_lastFrameTime;
    int m_minFps;
    int m_maxFps;
    int m_avgFps;
    long m_framesCount;
    long m_framesStartPeriod;

    std::string m_fps;
  };
}
