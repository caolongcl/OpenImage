//
// Created by caolong on 2021/3/9.
//

#include <softarch/Buffer.hpp>
#include <process/base/BufferProcessTask.hpp>
#include <utils/Log.hpp>

using namespace clt;

BufferProcessTask::BufferProcessTask(std::string name, std::shared_ptr<IProcessTask> task)
    : m_name(std::move(name)), m_task(std::move(task)) {

  Log::v(Log::PROCESSOR_TAG, "BufferProcessTask %s", m_name.c_str());

  m_task->Init();
}

BufferProcessTask::~BufferProcessTask() {
  m_task->DeInit();

  Log::v(Log::PROCESSOR_TAG, "~BufferProcessTask %s", m_name.c_str());
}

bool BufferProcessTask::Init() { return true; }

void BufferProcessTask::DeInit() {}

void BufferProcessTask::Process(std::shared_ptr<Buffer> buf) {
  if (buf != nullptr) {
    m_task->Process(buf);
  }
}