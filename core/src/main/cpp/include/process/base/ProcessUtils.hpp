//
// Created by caolong on 2021/3/17.
//

#pragma once

#include <utils/Log.hpp>
#include <softarch/std.hpp>

namespace clt {
  using TimeTask = std::function<void(long period)>;

  struct TimeStatics : public IComFunc<>,
                       public IComUpdate<long, TimeTask &&> {
    TimeStatics() = default;

    bool Init() override {
      avgProcessTime = 0;
      processCount = 0;
      return true;
    }
    void DeInit() override {}

    void Update(long lastTime, TimeTask &&task) override {
      float avg = (static_cast<float>(processCount) * avgProcessTime +
                   static_cast<float>( Utils::CurTimeMilli() - lastTime)) /
                  static_cast<float>(processCount + 1);
      avgProcessTime = avg;
      processCount += 1;

      task(avgProcessTime);

      if (processCount >= 10000) {
        processCount = 0;
        avgProcessTime = 0;
      }
    }

    float avgProcessTime;
    int processCount;
  };
}
