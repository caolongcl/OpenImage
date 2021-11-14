//
// Created by caolong on 2021/11/10.
//

#pragma once

#include <softarch/std.hpp>
#include <process/base/IProcessTask.hpp>

namespace clt {
  class GLThread;

  class BufferProcessTask;

  class ThreadBufferProcessTask final : public IBufferProcessTask {
  ClassDeclare(ThreadBufferProcessTask);
  public:
    ThreadBufferProcessTask(std::string name, std::shared_ptr<IProcessTask> task);

    ~ThreadBufferProcessTask();

    bool Init() override;

    void DeInit() override;

    void Process(std::shared_ptr<Buffer> buf) override;

    void Process(std::shared_ptr<Buffer> buf, Task &&task) override;

  private:
    std::shared_ptr<BufferProcessTask> m_task;
    std::shared_ptr<GLThread> m_worker;
    std::string m_workerName;
  };
}
