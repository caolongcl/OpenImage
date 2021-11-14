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

  m_eatTask = [this, thread]() {
    if (thread != nullptr) {
      eat([this, thread](SourceItem &item) {
        auto &tasks = m_tasksByClass[TaskType::eBufferTask];
        if (!tasks.empty()) {
          m_pixelReader->Read(item->GetTexture(), item->GetBuffer());
          for (auto &task : tasks) {
            dispatch(task, item->GetBuffer());
          }
          thread->PostByLimit(m_eatTask);
        }
      });
    }
  };

  m_normalTask = [this, thread]() {
    if (thread != nullptr) {
      auto &tasks = m_tasksByClass[TaskType::eNormalTask];
      if (!tasks.empty()) {
        for (auto &task : tasks) {
          dispatch(task);
        }
        thread->PostByLimit(m_normalTask);
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
    thread->Post([this, name, thread]() {
      if (m_tasks.find(name) == m_tasks.cend()) {
        auto task = ProcessFactory::Create(name);
        if (task != nullptr) {
          task->Init();
          if (task->IsBufferProcess()) {
            {
              std::lock_guard<std::mutex> locker(m_mutex);
              m_tasks[name] = task;
              m_tasksByClass[TaskType::eBufferTask].push_back(task);
            }
            if (hasBufferTask()) {
              thread->PostByLimit(m_eatTask);
            }
          } else if (task->IsNormalProcess()) {
            {
              std::lock_guard<std::mutex> locker(m_mutex);
              m_tasks[name] = task;
              m_tasksByClass[TaskType::eNormalTask].push_back(task);
            }
            if (hasNormalTask()) {
              thread->PostByLimit(m_normalTask);
            }
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
            m_tasksByClass[TaskType::eBufferTask].remove(iterator->second);
          } else if (iterator->second->IsNormalProcess()) {
            m_tasksByClass[TaskType::eNormalTask].remove(iterator->second);
          }
        }
      }
    });
  }
}

void ProcessPipe::eat(const IEater::Eater &eater) {
  if (hasBufferTask()) {
    m_source->Eat(eater);
  }
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
  return !m_tasksByClass[TaskType::eBufferTask].empty();
}

bool ProcessPipe::hasNormalTask() {
  return !m_tasksByClass[TaskType::eNormalTask].empty();
}