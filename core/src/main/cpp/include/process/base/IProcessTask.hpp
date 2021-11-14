//
// Created by caolong on 2020/8/29.
//

#pragma once

#include <softarch/std.hpp>
#include <softarch/IComFunc.hpp>
#include <softarch/Task.hpp>

namespace clt {

  class Buffer;

  enum class ProcessTaskType : int {
    eBufferTask,
    eNormalTask,
    eSingleTask,
    eUnknownTask
  };

#define DefineProcessType(type) public:constexpr static const ProcessTaskType s_type = type;

  struct IProcessTask : public IComFunc<> {
    IProcessTask() = default;

    virtual ~IProcessTask() = default;

    // 需要处理数据的任务
    virtual void Process(std::shared_ptr<Buffer> buf) = 0;

    virtual void Process(std::shared_ptr<Buffer> buf, Task &&task) = 0;

    // 普通任务
    virtual void Process() = 0;

    virtual void Process(Task &&task) = 0;

    virtual bool IsBufferProcess() const { return false; }

    virtual bool IsNormalProcess() const { return false; }

    virtual bool IsSingleProcess() const { return false; }
  };


  /**
   * 持续处理 Buffer 的任务
   */
  struct IBufferProcessTask : public IProcessTask {
  DefineProcessType(ProcessTaskType::eBufferTask);

    IBufferProcessTask() = default;

    virtual ~IBufferProcessTask() = default;

    bool IsBufferProcess() const override { return true; }

  private:
    void Process() override {}

    void Process(Task &&task) override {}
  };

  /**
   * 任务
   */
  struct INormalProcessTask : public IProcessTask {
  DefineProcessType(ProcessTaskType::eNormalTask);

    INormalProcessTask() = default;

    virtual ~INormalProcessTask() = default;

    bool IsNormalProcess() const override { return true; }

  private:
    void Process(std::shared_ptr<Buffer> Buffer) override {}

    void Process(std::shared_ptr<Buffer> buf, Task &&task) override {}
  };

  /**
 * 一次性任务
 */
  struct ISingleProcessTask : public IProcessTask {
  DefineProcessType(ProcessTaskType::eSingleTask);

    ISingleProcessTask() = default;

    virtual ~ISingleProcessTask() = default;

    bool IsSingleProcess() const override { return true; }

  private:
    void Process(std::shared_ptr<Buffer> Buffer) override {}

    void Process(std::shared_ptr<Buffer> buf, Task &&task) override {}
  };

  /**
   * 根据唯一的名字创建处理任务
   */
  class ProcessFactory {
  public:
    static std::shared_ptr<IProcessTask> Create(const std::string &name);

    static ProcessTaskType Type(const std::string &name);
  };

}
