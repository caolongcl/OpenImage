//
// Created by caolong on 2020/8/13.
//

#pragma once

#include <softarch/std.hpp>

namespace clt {

  /**
   * 提供线程池功能
   */
  class IThreadFunc {
  public:
    IThreadFunc() = default;

    virtual ~IThreadFunc() = default;

    virtual void Start() = 0;

    virtual void Stop() = 0;

    virtual void StopSync() = 0;

    virtual void StopSync(const Task &t) = 0;

    virtual void StopSync(Task &&t) = 0;

    virtual void AddTask(const Task &t) = 0;

    virtual void AddTask(Task &&t) = 0;

    virtual void AddTaskByLimit(const Task &t) = 0;

    virtual void AddTaskByLimit(Task &&t) = 0;

    virtual void Limit() = 0;

    virtual void UnLimit() = 0;

    virtual void Clear() = 0;

    virtual std::thread::id Id() = 0;
  };

}
