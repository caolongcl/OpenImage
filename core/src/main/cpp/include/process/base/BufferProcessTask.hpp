//
// Created by caolong on 2021/3/9.
//

#pragma once

#include <softarch/std.hpp>
#include <process/base/IProcessTask.hpp>

namespace clt {

  class BufferProcessTask final : public IBufferProcessTask {
  ClassDeclare(BufferProcessTask);
  public:
    BufferProcessTask(std::string name, std::shared_ptr<IProcessTask> task);

    ~BufferProcessTask();

    bool Init() override;

    void DeInit() override;

    void Process(std::shared_ptr<Buffer> buf) override;

    void Process(std::shared_ptr<Buffer> buf, Task &&task) override;

  private:
    std::string m_name;
    std::shared_ptr<IProcessTask> m_task;
  };
};