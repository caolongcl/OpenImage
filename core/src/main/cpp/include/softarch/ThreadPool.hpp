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

    virtual ~ThreadPool() = default;

    void Start() override;

    void Stop() override;

    void StopSync() override;

    void StopSync(const Task &t) override;

    void StopSync(Task &&t) override;

    void AddTask(const Task &t) override;

    void AddTask(Task &&t) override;

    void AddTaskByLimit(const Task &t) override;

    void AddTaskByLimit(Task &&t) override;

    void Limit() override;

    void UnLimit() override;

    void Clear() override;

    const std::string Name() const { return m_name; }

  protected:
    std::thread::id Id() override;

  private:
    void start(int numThreads);

    void run();

    void runOne();

    void stop();

    void stopSync();

    void stopSync(const Task &t);

  private:
    const int MAX_TASKS = 100;
    int m_numThreads;
    std::list<std::shared_ptr<std::thread>> m_threadGroup;
    SyncQueue<Task> m_taskQueue;
    std::atomic_bool m_running;
    std::once_flag m_flag;
    std::string m_name;
    std::atomic_bool m_limit;
  };

}