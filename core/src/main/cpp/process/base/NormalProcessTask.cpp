//
// Created by caolong on 2021/3/16.
//

#include <utils/Log.hpp>
#include <process/base/NormalProcessTask.hpp>

using namespace clt;

NormalProcessTask::NormalProcessTask(std::string name, std::shared_ptr<IProcessTask> task)
    : m_name(std::move(name)), m_task(std::move(task)) {
  m_task->Init();
}

NormalProcessTask::~NormalProcessTask() {
  m_task->DeInit();
}

bool NormalProcessTask::Init() { return true; }

void NormalProcessTask::DeInit() {}

void NormalProcessTask::Process() {
  m_task->Process();
}