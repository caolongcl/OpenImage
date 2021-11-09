//
// Created by caolong on 2020/6/7.
//

#pragma once

#include <softarch/SyncQueue.hpp>
#include <softarch/Task.hpp>
#include <utils/Log.hpp>
#include <softarch/IThreadFunc.hpp>
#include <softarch/IComFunc.hpp>

namespace clt {

  /**
   * 线程池
   */
  class ThreadPool : public IThreadFunc {
  ClassDeclare(ThreadPool)
  public:
    ThreadPool(std::string name, int numThreads = std::thread::hardware_concurrency());

    ThreadPool(std::string name,
               bool syncStop,
               int numThreads = std::thread::hardware_concurrency());

    virtual ~ThreadPool() = default;

    void Start() override;

    void Stop() override;

    void AddTask(const Task &t) override;

    void AddTask(Task &&t) override;

    const std::string Name() const { return m_name; }

  protected:
    std::thread::id Id() override;

  private:
    void start(int numThreads);

    void run();

    void stop();

    void stopSync();

  private:
    const int MAX_TASKS = 200;
    int m_numThreads;
    std::list<std::shared_ptr<std::thread>> m_threadGroup;
    SyncQueue<Task> m_taskQueue;
    std::atomic_bool m_running;
    std::once_flag m_flag;
    bool m_syncStop;
    std::string m_name;
  };

}