//
// Created by caolong on 2021/3/10.
//

#include <process/base/WorkerFlow.hpp>

using namespace clt;

std::mutex WorkerFlow::s_mutex;

bool WorkerFlow::Init() {
  Log::v(target, "WorkerFlow::Init");
  m_pool = std::make_shared<ThreadPool>("WorkerFlow", POOL_SIZE);
  m_pool->Start();
  return true;
}

void WorkerFlow::DeInit() {
  m_pool->Limit();
  m_pool->Clear();
  m_pool->Stop();
  Log::v(target, "WorkerFlow::DeInit");
}

void WorkerFlow::Post(const Task &t) {
  m_pool->AddTaskByLimit(t);
}

void WorkerFlow::Post(Task &&t) {
  m_pool->AddTaskByLimit(std::forward<Task>(t));
}
