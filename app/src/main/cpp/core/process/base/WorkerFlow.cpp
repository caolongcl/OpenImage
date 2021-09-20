//
// Created by caolong on 2021/3/10.
//

#include <process/base/WorkerFlow.hpp>

using namespace clt;

std::mutex WorkerFlow::s_mutex;

bool WorkerFlow::Init() {
  Log::v(Log::PROCESSOR_TAG, "WorkerFlow::Init");

  {
    std::lock_guard<std::mutex> locker(s_mutex);
    m_pool = std::make_shared<ThreadPool>("WorkerFlow", false, POOL_SIZE);
  }

  m_pool->Start();

  return true;
}

void WorkerFlow::DeInit() {
  m_pool->Stop();
  m_pool = nullptr;
  Log::v(Log::PROCESSOR_TAG, "WorkerFlow::DeInit");
}

void WorkerFlow::Post(const Task &t) {
  if (m_pool != nullptr) {
    m_pool->AddTask(t);
  }
}

void WorkerFlow::Post(Task &&t) {
  if (m_pool != nullptr) {
    m_pool->AddTask(std::forward<Task>(t));
  }
}
