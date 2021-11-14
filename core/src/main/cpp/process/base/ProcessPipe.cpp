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
#include <process/base/TextureBuffer.hpp>

using namespace clt;

ProcessPipe::ProcessPipe()
  : m_source(new ProcessSource(s_sourceSize)),
    m_pixelReader(new PixelReaderPbo()),
    m_eatTask(),
    m_normalTask() {
}

bool ProcessPipe::Init() {
  Log::v(target, "ProcessPipe::Init");

  m_source->Init();
  m_pixelReader->Init();

  auto thread = Flow::Self()->CreateThread(s_pipeThread, true);

  m_eatTask = [this, thread](const std::string &name) {
    Log::d(target, "pipe task1 %s", name.c_str());
    if (thread != nullptr) {
      Log::d(target, "pipe task2 %s", name.c_str());
      bool ret = eat([this, thread, name](SourceItem &item) {
        Log::d(target, "pipe task3 %s", name.c_str());
        auto &tasks = m_tasksByClass[ProcessTaskType::eBufferTask];
        auto task = tasks.find(name);
        if (task != tasks.end()) {
          m_pixelReader->Read(item->GetTexture(), item->GetBuffer());
          dispatch(task->second, item->GetBuffer(), [this, thread, name]() {
            if (thread != nullptr) {
              thread->PostByLimit([this, name]() {
                m_eatTask(name);
              });
            }
          });
        }
      });
      if (!ret) {
        thread->PostByLimit([this, name]() {
          m_eatTask(name);
        });
      }
    }
  };

  m_normalTask = [this, thread](const std::string &name) {
    if (thread != nullptr) {
      auto &tasks = m_tasksByClass[ProcessTaskType::eNormalTask];
      auto task = tasks.find(name);
      if (task != tasks.end()) {
        dispatch(task->second, [this, thread, name]() {
          if (thread != nullptr) {
            thread->PostByLimit([this, name]() {
              m_normalTask(name);
            });
          }
        });
      }
    }
  };

  return true;
}

void ProcessPipe::DeInit() {
  auto thread = Flow::Self()->GetThread(s_pipeThread);
  if (thread != nullptr) {
    thread->Limit();
    thread->Clear();
    Flow::Self()->DestroyThread(s_pipeThread);
  }

  m_source->DeInit();
  m_pixelReader->DeInit();

  Log::v(target, "ProcessPipe::DeInit");
}

void ProcessPipe::Update(const std::size_t width, const std::size_t height) {
  std::size_t smallWidth = width;
  std::size_t smallHeight = height;

  Log::d(target, "ProcessPipe::Update width %d height %d", smallWidth, smallHeight);

  // 缩小以减少计算量
  float bufRate = 1.0f;
  const int minSize = 640;
  if (smallHeight > minSize) {
    bufRate = (float) minSize / (float) smallHeight;
    smallHeight = minSize;
    smallWidth = Utils::RoundToEven(smallWidth * bufRate);
  }

  // 实际处理的图像大小和实际预览大小的比例
  ResManager::Self()->UpdateResParamsBufRatio(bufRate);

  auto thread = Flow::Self()->GetThread(s_pipeThread);
  if (thread != nullptr) {
    thread->Post([this, width, height]() {
      Log::d(target, "ProcessPipe::Update in pipe width %d height %d", width, height);
      m_source->Update(width, height);
      m_pixelReader->Update(width, height);
    });
  }
}

void ProcessPipe::ClearProcessTasks() {
  clearProcessTasks();
}

void ProcessPipe::Feed(const Feeder &feeder) {
  {
    std::lock_guard<std::mutex> locker(m_mutex);
    if (!hasBufferTask()) {
      return;
    }
  }

  m_source->Feed(feeder);
}

void ProcessPipe::EnableProcess(const std::string &name, bool enable) {
  if (enable) {
    pushTask(name);
  } else {
    popTask(name);
  }
}

void ProcessPipe::pushTask(const std::string &name) {
  Log::v(target, "ProcessPipe::PushTask %s", name.c_str());

  if (name.empty()) {
    Log::w(target, "ProcessPipe::PushTask invalid name %s", name.c_str());
    return;
  }

  auto thread = Flow::Self()->GetThread(s_pipeThread);
  if (thread != nullptr) {
    if (ProcessFactory::Type(name) == ProcessTaskType::eBufferTask) {

    }
    
    thread->Post([this, name, thread]() {
      if (m_tasks.find(name) == m_tasks.cend()) {
        auto task = ProcessFactory::Create(name);
        if (task != nullptr) {
          task->Init();
          if (task->IsBufferProcess()) {
            {
              std::lock_guard<std::mutex> locker(m_mutex);
              m_tasks[name] = task;
              m_tasksByClass[ProcessTaskType::eBufferTask][name] = task;
            }

            thread->PostByLimit([this, name]() {
              Log::d(target, "pipe task0 %s", name.c_str());
              m_eatTask(name);
            });
          } else if (task->IsNormalProcess()) {
            {
              std::lock_guard<std::mutex> locker(m_mutex);
              m_tasks[name] = task;
              m_tasksByClass[ProcessTaskType::eNormalTask][name] = task;
            }

            thread->PostByLimit([this, name]() {
              m_normalTask(name);
            });
          } else {
            dispatch(task);
          }
        }
      }
    });
  }
}

void ProcessPipe::popTask(const std::string &name) {
  Log::v(target, "ProcessPipe::PopTask %s", name.c_str());

  auto thread = Flow::Self()->GetThread(s_pipeThread);
  if (thread != nullptr) {
    thread->Post([this, name]() {
      auto iterator = m_tasks.find(name);
      if (iterator != m_tasks.cend()) {
        iterator->second->DeInit();
        {
          std::lock_guard<std::mutex> locker(m_mutex);
          m_tasks.erase(name);
          if (iterator->second->IsBufferProcess()) {
            m_tasksByClass[ProcessTaskType::eBufferTask].erase(iterator->first);
            if (!hasBufferTask()) {
              m_source->Reset();
            }
          } else if (iterator->second->IsNormalProcess()) {
            m_tasksByClass[ProcessTaskType::eNormalTask].erase(iterator->first);
            if (!hasNormalTask()) {

            }
          }
        }
      }
    });
  }
}

bool ProcessPipe::eat(const IEater::Eater &eater) {
  return m_source->Eat(eater);
}

void ProcessPipe::dispatch(const std::shared_ptr<IProcessTask> &task) {
  if (task->IsSingleProcess()) {
    dispatchSingle(task);
  } else if (task->IsNormalProcess()) {
    task->Process();
  }
}

void ProcessPipe::dispatch(const std::shared_ptr<IProcessTask> &task,
                           const std::shared_ptr<Buffer> &buf) {
  if (task != nullptr && buf != nullptr) {
    auto dataBuf = std::make_shared<Buffer>(buf->data, buf->width, buf->height, buf->channel);
    task->Process(dataBuf);
  }
}

void ProcessPipe::dispatch(const std::shared_ptr<IProcessTask> &task, Task &&signal) {
  if (task->IsSingleProcess()) {
    dispatchSingle(task);
  } else if (task->IsNormalProcess()) {
    task->Process(std::move(signal));
  }
}

void ProcessPipe::dispatch(const std::shared_ptr<IProcessTask> &task,
                           const std::shared_ptr<Buffer> &buf, Task &&signal) {
  if (task != nullptr && buf != nullptr) {
    auto dataBuf = std::make_shared<Buffer>(buf->data, buf->width, buf->height, buf->channel);
    task->Process(dataBuf, std::move(signal));
  }
}

void ProcessPipe::dispatchSingle(const std::shared_ptr<IProcessTask> &task) {
  if (task != nullptr) {
    WorkerFlow::Self()->Post([task]() {
      task->Process();
    });
  }
}

void ProcessPipe::clearProcessTasks() {
  auto thread = Flow::Self()->GetThread(s_pipeThread);
  if (thread != nullptr) {
    thread->Post([this]() {
      for (auto &task:m_tasks) {
        task.second->DeInit();
      }
      {
        std::lock_guard<std::mutex> locker(m_mutex);
        m_tasks.clear();
        m_tasksByClass.clear();
      }
    });
  }
}

bool ProcessPipe::hasBufferTask() {
  auto tasks = m_tasksByClass.find(ProcessTaskType::eBufferTask);
  return tasks != m_tasksByClass.end() && !tasks->second.empty();
}

bool ProcessPipe::hasNormalTask() {
  auto tasks = m_tasksByClass.find(ProcessTaskType::eNormalTask);
  return tasks != m_tasksByClass.end() && !tasks->second.empty();
}