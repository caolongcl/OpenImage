//
// Created by caolong on 2020/8/13.
//

#include <softarch/ThreadPool.hpp>
#include <softarch/std.hpp>

using namespace clt;

ThreadPool::ThreadPool(std::string name, int numThreads)
    : m_numThreads(numThreads),
      m_taskQueue(MAX_TASKS),
      m_syncStop(true),
      m_name(std::move(name)),
      m_running(false) {
}

ThreadPool::ThreadPool(std::string name,
                       bool syncStop,
                       int numThreads)
    : m_numThreads(numThreads),
      m_taskQueue(MAX_TASKS),
      m_syncStop(syncStop),
      m_name(std::move(name)),
      m_running(false) {
}

void ThreadPool::Start() {
  Log::v("ThreadPool", "%s start", m_name.c_str());
  start(m_numThreads);
}

void ThreadPool::Stop() {
  std::call_once(m_flag, [this] { m_syncStop ? stopSync() : stop(); });
}

void ThreadPool::AddTask(const Task &t) {
  bool success = m_taskQueue.Add(t);
  if (!success) {
    Log::d("ThreadPool", "%s can't add anymore", m_name.c_str());
  }
}

void ThreadPool::AddTask(Task &&t) {
  bool success = m_taskQueue.Add(std::forward<Task>(t));
  if (!success) {
    Log::d("ThreadPool", "%s can't add anymore", m_name.c_str());
  }
}

void ThreadPool::start(int numThreads) {
  m_running = true;

  for (size_t i = 0; i < numThreads; i++) {
    m_threadGroup.push_back(std::make_shared<std::thread>(&ThreadPool::run, this));
  }
}

void ThreadPool::run() {
  while (m_running) {
    std::list<Task> list;
    m_taskQueue.Get(list);

    for (auto &task : list) {
      task();
    }
  }
}

void ThreadPool::stop() {
  Log::v("ThreadPool", "%s stopping", m_name.c_str());
  m_taskQueue.Stop();
  m_running = false;

  for (auto &thread : m_threadGroup) {
    if (thread)
      thread->join();
  }

  m_threadGroup.clear();

  Log::v("ThreadPool", "%s stopped", m_name.c_str());
}

void ThreadPool::stopSync() {
  AddTask([this]() {
    Log::v("ThreadPool", "%s stopSync set running=false", m_name.c_str());
    m_running = false;
  });

  Log::v("ThreadPool", "%s stopSync wait all tasks finish", m_name.c_str());

  for (auto &thread : m_threadGroup) {
    if (thread)
      thread->join();
  }

  m_taskQueue.Stop();
  m_threadGroup.clear();
  Log::v("ThreadPool", "%s stopSync stopped", m_name.c_str());
}

std::thread::id ThreadPool::Id() {
  return m_numThreads == 1 ? m_threadGroup.front()->get_id() : std::thread::id();
}