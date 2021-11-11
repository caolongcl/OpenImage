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
    m_running(false),
    m_stopAdd(false) {
}

ThreadPool::ThreadPool(std::string name,
                       bool syncStop,
                       int numThreads)
  : m_numThreads(numThreads),
    m_taskQueue(MAX_TASKS),
    m_syncStop(syncStop),
    m_name(std::move(name)),
    m_running(false),
    m_stopAdd(false) {
}

void ThreadPool::Start() {
  Log::v(target, "%s start", m_name.c_str());
  start(m_numThreads);
}

void ThreadPool::Stop() {
  std::call_once(m_flag, [this] { m_syncStop ? stopSync() : stop(); });
}

void ThreadPool::AddTask(const Task &t) {
  if (m_stopAdd) return;

  bool success = m_taskQueue.Add(t);
  if (!success) {
    Log::v(target, "%s can't add anymore", m_name.c_str());
  }
}

void ThreadPool::AddTask(Task &&t) {
  if (m_stopAdd) return;

  bool success = m_taskQueue.Add(std::forward<Task>(t));
  if (!success) {
    Log::v(target, "%s can't add anymore", m_name.c_str());
  }
}


void ThreadPool::Clear() {
  m_taskQueue.Clear();
}

void ThreadPool::ClearAndAddLast(const Task &t) {
  m_taskQueue.ClearAndAddLast([this, &t]() {
    t();
    m_running = false;
  });
}

void ThreadPool::ClearAndAddLast(Task &&t) {
  m_taskQueue.ClearAndAddLast([this, t1 = std::move(t)]() {
    t1();
    m_running = false;
  });
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
  Log::v(target, "%s stopping", m_name.c_str());

  m_stopAdd = true;
  m_running = false;

  m_taskQueue.Stop();

  for (auto &thread : m_threadGroup) {
    if (thread)
      thread->join();
  }

  m_threadGroup.clear();

  Log::v(target, "%s stopped", m_name.c_str());
}

void ThreadPool::stopSync() {
  m_stopAdd = true;
  m_taskQueue.Add(std::forward<Task>([this]() {
    m_running = false;
  }));

  Log::v(target, "%s stopSync wait all tasks finish begin", m_name.c_str());
  while (!m_taskQueue.Empty()) {
    std::this_thread::yield();
  }
  Log::v(target, "%s stopSync wait all tasks finish end", m_name.c_str());

  m_taskQueue.Stop();

  for (auto &thread : m_threadGroup) {
    if (thread) {
      thread->join();
    }
  }

  m_threadGroup.clear();
  Log::v(target, "%s stopSync stopped", m_name.c_str());
}

std::thread::id ThreadPool::Id() {
  return m_numThreads == 1 ? m_threadGroup.front()->get_id() : std::thread::id();
}