//
// Created by caolong on 2020/8/28.
//

#pragma once

#include <softarch/std.hpp>
#include <softarch/IComFunc.hpp>
#include <softarch/GLThread.hpp>
#include <process/base/IProcessTask.hpp>
#include <process/base/ProcessTextures.hpp>
#include <process/base/ProcessBuffers.hpp>
#include <process/base/PixelReader.hpp>
#include <softarch/Observer.hpp>
#include <process/base/ProcessSource.hpp>

namespace clt {

  /**
   * 管理需要处理从纹理中产生的数据的任务
   * 两个纹理队列，写队列是复制 OESCopier 到处理纹理，读队列是给特定任务读取数据的
   * 两个数据队列，从纹理队列中读出来的处理
   */
  class ProcessPipe final : public IComFunc<>,
                            public IComUpdate<const std::size_t, const std::size_t>,
                            public IFeeder,
                            public Observer<OPreviewSize> {
  ClassDeclare(ProcessPipe)
  public:
    ProcessPipe();

    ~ProcessPipe() = default;

    bool Init() override;

    void DeInit() override;

    void Update(const std::size_t width, const std::size_t height) override;

    void EnableProcess(const std::string &name, bool enable);

    void Feed(const Feeder &feeder) override;

    void ClearProcessTasks();

    void OnUpdate(OPreviewSize &&t) override {
      Update(t.width, t.height);
    }

  private:
    void Reset() override {}

    void Signal(bool status) override {}

    bool eat(const IEater::Eater &eater);

    void pushTask(const std::string &name);

    void popTask(const std::string &name);

    static void dispatch(const std::shared_ptr<IProcessTask> &task);

    static void dispatch(const std::shared_ptr<IProcessTask> &task,
                         const std::shared_ptr<Buffer> &buf);

    static void dispatch(const std::shared_ptr<IProcessTask> &task, Task &&signal);

    static void dispatch(const std::shared_ptr<IProcessTask> &task,
                         const std::shared_ptr<Buffer> &buf, Task &&signal);

    static void dispatchSingle(const std::shared_ptr<IProcessTask> &task);

    void clearProcessTasks();

    bool hasBufferTask();

    bool hasNormalTask();

    constexpr static const char *s_pipeThread = "ProcessPipe::pipe";
    constexpr static const int s_sourceSize = 8;

    using TaskWithName = std::function<void(const std::string &name)>;

  private:
    std::unordered_map<std::string, std::shared_ptr<IProcessTask>> m_tasks;
    std::unordered_map<ProcessTaskType, std::unordered_map<std::string, std::shared_ptr<IProcessTask>>> m_tasksByClass;
    std::shared_ptr<ProcessSource> m_source;
    std::shared_ptr<PixelReaderPbo> m_pixelReader;
    std::mutex m_mutex;
    TaskWithName m_eatTask;
    TaskWithName m_normalTask;
  };
}
