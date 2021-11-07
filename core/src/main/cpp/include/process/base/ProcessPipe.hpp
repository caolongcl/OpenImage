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
#include <process/base/IProcessTextureReader.hpp>

namespace clt {

  /**
   * 管理需要处理从纹理中产生的数据的任务
   * 两个纹理队列，写队列是复制 OESCopier 到处理纹理，读队列是给特定任务读取数据的
   * 两个数据队列，从纹理队列中读出来的处理
   */
  class ProcessPipe final : public IComFunc<>,
                            public IComUpdate<const std::size_t, const std::size_t>,
                            public IProcessTextureReader,
                            public Observer<OPreviewSize> {
  public:
    ProcessPipe();

    ~ProcessPipe() = default;

    bool Init() override;

    void DeInit() override;

    void Update(const std::size_t width, const std::size_t height) override;

    void EnableProcess(const std::string &name, bool enable);

    std::shared_ptr<Texture> PopWriteTexture() override;

    void PushReadTexture(std::shared_ptr<Texture> tex) override;

    void Process() override;

    void ClearProcessTasks();

    void OnUpdate(OPreviewSize &&t) override {
      Update(t.width, t.height);
    }

  private:
    void pushTask(const std::string &name);

    void popTask(const std::string &name);

    static void postNormalToWorker(const std::shared_ptr<IProcessTask> &task);

    static void postBufferToWorker(const std::shared_ptr<IProcessTask> &task,
                                   const std::shared_ptr<Buffer> &buf);

    void clearProcessTasks();

    void readTexture();

    void dispatchBuffer();

    bool hasBufferTasks();

    constexpr static const char *s_pipeThread = "ProcessPipe::pipe";
    constexpr static const char *s_readTextureThread = "ProcessPipe::read";
    constexpr static const char *s_dispatchThread = "ProcessPipe::dispatch";

  private:
    // buffer 任务队列
    std::unordered_map<std::string, std::shared_ptr<IProcessTask>> m_tasks;

    std::mutex m_dispatchMutex;

    // 管理纹理队列
    std::shared_ptr<ProcessTextures> m_textures;

    std::shared_ptr<PixelReaderPbo> m_pixelReader;

    // 管理数据队列
    std::shared_ptr<ProcessBuffers> m_buffers;
  };
}
