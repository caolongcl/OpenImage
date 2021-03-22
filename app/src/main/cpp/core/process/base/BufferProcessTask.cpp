//
// Created by caolong on 2021/3/9.
//

#include <softarch/Buffer.hpp>
#include <process/base/BufferProcessTask.hpp>
#include <utils/Log.hpp>
#include <process/base/WorkerFlow.hpp>

using namespace clt;

BufferProcessTask::BufferProcessTask(std::string name, std::shared_ptr<IProcessTask> task)
        : m_name(std::move(name)), m_task(std::move(task)) {
    m_task->Init();
}

BufferProcessTask::~BufferProcessTask() {
    m_task->DeInit();
}

bool BufferProcessTask::Init() { return true; }

void BufferProcessTask::DeInit() {}

void BufferProcessTask::Process(std::shared_ptr<Buffer> buf) {
    if (buf != nullptr) {
        auto dataBuf = std::make_shared<Buffer>(buf->data, buf->width, buf->height, buf->channel);

        WorkerFlow::Self()->Post([this, dataBuf]() {
            m_task->Process(dataBuf);
        });
    }
}