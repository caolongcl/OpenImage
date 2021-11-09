//
// Created by caolong on 2021/3/10.
//

#pragma once

#include <softarch/ThreadPool.hpp>
#include <softarch/IComFunc.hpp>

namespace clt {

  class WorkerFlow final : public IComFunc<> {
  ClassDeclare(WorkerFlow)
  public:
    bool Init() override;

    void DeInit() override;

    void Post(const Task &t);

    void Post(Task &&t);

  private:
    std::shared_ptr<ThreadPool> m_pool;

  public:
    WorkerFlow(const WorkerFlow &) = delete;

    WorkerFlow &operator=(const WorkerFlow &) = delete;

    WorkerFlow(WorkerFlow &&) = delete;

    WorkerFlow &operator=(WorkerFlow &&) = delete;

    ~WorkerFlow() = default;

    static std::shared_ptr<WorkerFlow> Self() {
      std::lock_guard<std::mutex> locker(s_mutex);
      static std::shared_ptr<WorkerFlow> instance(new WorkerFlow());
      return instance;
    }

  private:
    WorkerFlow() = default;

    static std::mutex s_mutex;
    constexpr static const int POOL_SIZE = 8;
  };

}
