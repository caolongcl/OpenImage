//
// Created by caolong on 2020/8/28.
//

#include <process/base/ProcessPipe.hpp>
#include <render/Flow.hpp>
#include <utils/Utils.hpp>
#include <process/base/BufferProcessTask.hpp>
#include <process/facedetector/FaceDetector.hpp>
#include <process/base/WorkerFlow.hpp>
#include <res/ResManager.hpp>

using namespace clt;

ProcessPipe::ProcessPipe()
    : m_textures(new ProcessTextures()),
      m_pixelReader(new PixelReaderPbo()),
      m_buffers(new ProcessBuffers()) {

}

bool ProcessPipe::Init() {
  Log::v(Log::PROCESSOR_TAG, "ProcessPipe::Init");

  auto thread = Flow::Self()->CreateThread(s_pipeThread, true);
  if (thread != nullptr) {
    thread->Post([this]() {
      m_textures->Init();
      m_pixelReader->Init();
      m_buffers->Init();
    });
  }
  Flow::Self()->CreateThread(s_readTextureThread, true);
  Flow::Self()->CreateThread(s_dispatchThread);

  return true;
}

void ProcessPipe::DeInit() {
  auto thread = Flow::Self()->GetThread(s_pipeThread);
  if (thread != nullptr) {
    thread->Post([this]() {
      m_buffers->DeInit();
      m_pixelReader->DeInit();
      m_textures->DeInit();
    });
  }

  Flow::Self()->DestroyThread(s_pipeThread);
  Flow::Self()->DestroyThread(s_dispatchThread);
  Flow::Self()->DestroyThread(s_readTextureThread);

  Log::v(Log::PROCESSOR_TAG, "ProcessPipe::DeInit");
}

void ProcessPipe::Update(const std::size_t width, const std::size_t height) {
  std::size_t smallWidth = width;
  std::size_t smallHeight = height;

  Log::d(Log::PROCESSOR_TAG, "ProcessPipe::Update width %d height %d", smallWidth, smallHeight);

  // 缩小以减少计算量
  float bufRate = 1.0f;
  const int minSize = 180;
  if (smallWidth > minSize) {
    bufRate = (float) minSize / (float) smallWidth;
    smallWidth = minSize;
    smallHeight = Utils::RoundToEven(smallHeight * bufRate);
  }

  // 实际处理的图像大小和实际预览大小的比例
  ResManager::Self()->UpdateResParamsBufRatio(bufRate);

  auto thread = Flow::Self()->GetThread(s_pipeThread);
  if (thread != nullptr) {
    thread->Post([this, width = smallWidth, height = smallHeight]() {
      Log::d(Log::PROCESSOR_TAG,
             "ProcessPipe::Update in pipe width %d height %d",
             width, height);
      m_textures->Update(width, height);
      m_pixelReader->Update(width, height);
      m_buffers->Update(width, height);
    });
  }
}

void ProcessPipe::ClearProcessTasks() {
  clearProcessTasks();
}

void ProcessPipe::Process() {
  auto thread = Flow::Self()->GetThread(s_pipeThread);
  if (thread != nullptr) {
    thread->Post([this]() {
      readTexture();
      dispatchBuffer();
    });
  }
}

void ProcessPipe::EnableProcess(const std::string &name, bool enable) {
  if (enable) {
    pushTask(name);
  } else {
    popTask(name);
  }
}

std::shared_ptr<Texture> ProcessPipe::PopWriteTexture() {
  return m_textures->PopWriteTexture();
}

void ProcessPipe::PushReadTexture(std::shared_ptr<Texture> tex) {
  if (tex != nullptr) {
    m_textures->PushReadTexture(tex);
  }
}

void ProcessPipe::pushTask(const std::string &name) {
  Log::v(Log::PROCESSOR_TAG, "ProcessPipe::PushTask %s", name.c_str());

  if (name.empty()) {
    Log::w(Log::PROCESSOR_TAG, "ProcessPipe::PushTask invalid name %s", name.c_str());
    return;
  }

  auto thread = Flow::Self()->GetThread(s_pipeThread);
  if (thread != nullptr) {
    thread->Post([this, name]() {
      if (m_tasks.find(name) == m_tasks.cend()) {
        auto task = ProcessFactory::Create(name);
        if (task != nullptr) {
          if (task->IsBufferProcess()) {
            std::lock_guard<std::mutex> locker(m_dispatchMutex);
            m_tasks[name] = task;
          } else if (task->IsNormalProcess()) {
            postNormalToWorker(task);
          }
        }
      }
    });
  }
}

void ProcessPipe::popTask(const std::string &name) {
  Log::v(Log::PROCESSOR_TAG, "ProcessPipe::PopTask %s", name.c_str());

  auto thread = Flow::Self()->GetThread(s_pipeThread);
  if (thread != nullptr) {
    thread->Post([this, name]() {
      if (m_tasks.find(name) != m_tasks.cend()) {
        std::lock_guard<std::mutex> locker(m_dispatchMutex);
        m_tasks.erase(name);
      }
    });
  }
}

void ProcessPipe::postNormalToWorker(const std::shared_ptr<IProcessTask> &task) {
  WorkerFlow::Self()->Post([task]() {
    task->Process();
  });
}

void ProcessPipe::postBufferToWorker(const std::shared_ptr<IProcessTask> &task,
                                     const std::shared_ptr<Buffer> &buf) {
  if (task != nullptr && buf != nullptr) {
    auto dataBuf = std::make_shared<Buffer>(buf->data, buf->width, buf->height, buf->channel);
    WorkerFlow::Self()->Post([task, dataBuf]() {
      task->Process(dataBuf);
    });
  }
}

void ProcessPipe::clearProcessTasks() {
  auto thread = Flow::Self()->GetThread(s_pipeThread);
  if (thread != nullptr) {
    thread->Post([this]() {
      std::lock_guard<std::mutex> locker(m_dispatchMutex);
      m_tasks.clear();
    });
  }
}

void ProcessPipe::readTexture() {
  auto thread = Flow::Self()->GetThread(s_readTextureThread);
  if (thread != nullptr) {
    thread->Post([this]() {
      auto tex = m_textures->PopReadTexture();
      auto buf = m_buffers->PopWriteBuffer();

      if (tex != nullptr && buf != nullptr) {
        bool read;
        {
          std::lock_guard<std::mutex> locker(m_dispatchMutex);
          read = hasBufferTasks();
        }

        if (read) {
          m_pixelReader->Read(tex, buf);
        }
      }

      if (tex != nullptr) {
        m_textures->PushWriteTexture(tex);
      }

      if (buf != nullptr) {
        m_buffers->PushReadBuffer(buf);
      }
    });
  }
}

void ProcessPipe::dispatchBuffer() {
  auto thread = Flow::Self()->GetThread(s_readTextureThread);
  if (thread != nullptr) {
    thread->Post([this]() {
      auto buf = m_buffers->PopReadBuffer();

      if (buf != nullptr) {
        bool dispatch;
        decltype(m_tasks) tmp;
        {
          std::lock_guard<std::mutex> locker(m_dispatchMutex);
          dispatch = hasBufferTasks();
          if (dispatch) {
            tmp = m_tasks;
          }
        }
        if (dispatch) {
          for (auto &task : tmp) {
            postBufferToWorker(task.second, buf);
          }
        }

        m_buffers->PushWriteBuffer(buf);
      }
    });
  }
}

bool ProcessPipe::hasBufferTasks() {
  return !m_tasks.empty() &&
         std::find_if(m_tasks.begin(), m_tasks.end(),
                      [](auto &elem) {
                        return elem.second->IsBufferProcess();
                      }) != m_tasks.end();
}