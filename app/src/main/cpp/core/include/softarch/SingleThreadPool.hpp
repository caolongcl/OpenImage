//
// Created by caolong on 2020/8/13.
//

#pragma once

#include <softarch/ThreadPool.hpp>

namespace clt {

  /**
   * 单线程循环处理任务
   */
  class SingleThreadPool final : public ThreadPool {
  public:
    explicit SingleThreadPool(const std::string &name);

    explicit SingleThreadPool(const std::string &name, bool syncStop);

    ~SingleThreadPool() = default;

    std::thread::id Id() override;
  };

}
