//
// Created by caolong on 2020/8/29.
//

#pragma once

#include <softarch/std.hpp>
#include <softarch/IComFunc.hpp>

namespace clt {

  class Buffer;

  struct IProcessTask : public IComFunc<> {
    IProcessTask() = default;

    virtual ~IProcessTask() = default;

    // 需要处理数据的任务
    virtual void Process(std::shared_ptr<Buffer> buf) = 0;

    // 普通任务
    virtual void Process() = 0;

    virtual bool IsBufferProcess() const { return false; }

    virtual bool IsNormalProcess() const { return false; }
  };


  /**
   * 持续处理 Buffer 的任务
   */
  struct IBufferProcessTask : public IProcessTask {
    IBufferProcessTask() = default;

    virtual ~IBufferProcessTask() = default;

    bool IsBufferProcess() const override { return true; }

  private:

    void Process() override {}
  };

  /**
   * 一次性任务
   */
  struct INormalProcessTask : public IProcessTask {
    INormalProcessTask() = default;

    virtual ~INormalProcessTask() = default;

    bool IsNormalProcess() const override { return true; }

  private:

    void Process(std::shared_ptr<Buffer> Buffer) override {}
  };

  /**
   * 根据唯一的名字创建处理任务
   */
  class ProcessFactory {
  public:
    static std::shared_ptr<IProcessTask> Create(const std::string &type);
  };

}
