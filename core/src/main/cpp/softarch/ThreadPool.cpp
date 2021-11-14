//
// Created by caolong on 2020/8/13.
//

#include <softarch/ThreadPool.hpp>
#include <softarch/std.hpp>

using namespace clt;

ThreadPool::ThreadPool(std::string name, int numThreads)
  : m_numThreads(numThreads),
    m_taskQueue(MAX_TASKS),
    m_name(std::move(name)),
    m_running(false),
    m_limit(false) {
}

void ThreadPool::Start() {
  Log::v(target, "%s start", m_name.c_str());
  start(m_numThreads);
}

void ThreadPool::Stop() {
  std::call_once(m_flag, [this] { stop(); });
}

void ThreadPool::StopSync() {
  std::call_once(m_flag, [this] { stopSync(); });
}

void ThreadPool::StopSync(const Task &t) {
  std::call_once(m_flag, [this, t] { stopSync(t); });
}

void ThreadPool::StopSync(Task &&t) {
  std::call_once(m_flag, [this, t1 = std::move(t)] { stopSync(t1); });
}

void ThreadPool::AddTask(const Task &t) {
  bool success = m_taskQueue.Add(t);
  if (!success) {
    Log::v(target, "%s can't add anymore", m_name.c_str());
  }
}

void ThreadPool::AddTask(Task &&t) {
  bool success = m_taskQueue.Add(std::forward<Task>(t));
  if (!success) {
    Log::v(target, "%s can't add anymore", m_name.c_str());
  }
}

void ThreadPool::AddTaskByLimit(const Task &t) {
  if (m_limit) return;
  AddTask(t);
}

void ThreadPool::AddTaskByLimit(Task &&t) {
  if (m_limit) return;
  AddTask(std::forward<Task>(t));
}

void ThreadPool::Limit() {
  m_limit = true;
}

void ThreadPool::UnLimit() {
  m_limit = false;
}

void ThreadPool::Clear() {
  m_taskQueue.Clear();
}

void ThreadPool::start(int numThreads) {
  m_running = true;

  for (size_t i = 0; i < numThreads; i++) {
    m_threadGroup.push_back(std::make_shared<std::thread>(&ThreadPool::runOne, this));
  }
}

void ThreadPool::run() {
  while (m_running) {
    std::list<Task> list;
    m_taskQueue.Get(list);

    if (!m_running) return;

    for (auto &task : list) {
      if (!m_running) break;
      task();
    }
  }
}

void ThreadPool::runOne() {
  while (m_running) {
    Task task;
    m_taskQueue.Get(task);

    if (!m_running) return;

    if (task) {
      task();
    }
  }
}

void ThreadPool::stop() {
  m_running = false;

  m_taskQueue.Stop();

  for (auto &thread : m_threadGroup) {
    if (thread)
      thread->join();
  }

  m_threadGroup.clear();
  Log::v(target, "%s stop stopped", m_name.c_str());
}

void ThreadPool::stopSync() {
  m_taskQueue.StopSync([this]() {
    m_running = false;
  });

  for (auto &thread : m_threadGroup) {
    if (thread) {
      thread->join();
    }
  }

  m_taskQueue.Stop();
  m_threadGroup.clear();
  Log::v(target, "%s stopSync stopped", m_name.c_str());
}

void ThreadPool::stopSync(const Task &t) {
  m_taskQueue.StopSync([this, t]() {
    t();
    m_running = false;
  });

  for (auto &thread : m_threadGroup) {
    if (thread) {
      thread->join();
    }
  }

  m_taskQueue.Stop();
  m_threadGroup.clear();
  Log::v(target, "%s stopSync stopped", m_name.c_str());
}

std::thread::id ThreadPool::Id() {
  return m_numThreads == 1 ? m_threadGroup.front()->get_id() : std::thread::id();
}