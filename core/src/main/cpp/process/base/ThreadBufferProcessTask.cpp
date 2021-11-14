//
// Created by caolong on 2021/11/10.
//

#include <softarch/Buffer.hpp>
#include <process/base/BufferProcessTask.hpp>
#include <process/base/ThreadBufferProcessTask.hpp>
#include <utils/Log.hpp>
#include <utils/Utils.hpp>
#include <render/Flow.hpp>

using namespace clt;

ThreadBufferProcessTask::ThreadBufferProcessTask(std::string name, std::shared_ptr<IProcessTask> task)
  : m_task(new BufferProcessTask(std::move(name), std::move(task))) {
  m_workerName = "thread_buffer_process||" + std::to_string(Utils::CurTimeMilli());
}

ThreadBufferProcessTask::~ThreadBufferProcessTask() = default;

bool ThreadBufferProcessTask::Init() {
  m_worker = Flow::Self()->CreateThread(m_workerName);
  m_worker->Post([this]() { m_task->Init(); });
  return true;
}

void ThreadBufferProcessTask::DeInit() {
  m_worker->Limit();
  m_worker->Clear();
  m_worker->Post([this]() { m_task->DeInit(); });
  Flow::Self()->DestroyThread(m_workerName);
}

void ThreadBufferProcessTask::Process(std::shared_ptr<Buffer> buf) {
  if (buf != nullptr) {
    m_worker->PostByLimit([this, buf]() { m_task->Process(buf); });
  }
}