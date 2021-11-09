//
// Created by caolong on 2021/3/16.
//

#pragma once

#include <softarch/std.hpp>
#include <process/base/IProcessTask.hpp>

namespace clt {

  class NormalProcessTask final : public INormalProcessTask {
  ClassDeclare(NormalProcessTask)
  public:
    NormalProcessTask(std::string name, std::shared_ptr<IProcessTask> task);

    ~NormalProcessTask();

    bool Init() override;

    void DeInit() override;

    void Process() override;

  private:
    std::string m_name;
    // 具体任务
    std::shared_ptr<IProcessTask> m_task;
  };
};
