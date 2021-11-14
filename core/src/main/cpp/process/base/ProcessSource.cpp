//
// Created by caolong on 2021/11/14.
//

#include <process/base/ProcessSource.hpp>
#include <process/base/TextureBuffer.hpp>
#include <utils/Log.hpp>

using namespace clt;

ProcessSource::ProcessSource(int size)
  : m_size(size),
    m_noWaitSignal(false) {
}

bool ProcessSource::Init() {
  assert(m_size != 0);

  m_queue = std::make_shared<TextureBufferRingQueue>(m_size);
  m_queue->Init();

  return true;
}

void ProcessSource::DeInit() {
  m_queue->DeInit();
}

void ProcessSource::Update(const std::size_t width, const std::size_t height) {
  assert(width != 0 && height != 0);
  std::lock_guard<std::mutex> lockGuard(m_mutex);
  m_queue->Update(width, height);
}

void ProcessSource::Feed(const Feeder &feeder) {
  SourceItem textureBuffer;
  if (m_queue->Rear(textureBuffer)) {
    feeder(textureBuffer->GetTexture());
    m_queue->Push();

    std::unique_lock<std::mutex> locker(m_mutex);
    m_notEmpty.notify_one();
  }
}

bool ProcessSource::Eat(const Eater &eater) {
  SourceItem textureBuffer;

  std::unique_lock<std::mutex> locker(m_mutex);
  m_notEmpty.wait(locker, [this, &textureBuffer]() {
    return m_noWaitSignal || m_queue->Last(textureBuffer);
  });

  if (!m_noWaitSignal) {
    eater(textureBuffer);
    m_queue->Pop();
    return true;
  }
  return false;
}

void ProcessSource::Reset() {
  m_queue->Reset();
}

void ProcessSource::Signal(bool status) {
  std::unique_lock<std::mutex> locker(m_mutex);
  m_noWaitSignal = status;
  m_notEmpty.notify_all();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ProcessSource::TextureBufferRingQueue::TextureBufferRingQueue(int size)
  : SoureceQueue(size) {
}

bool ProcessSource::TextureBufferRingQueue::Init() {
  for (auto &item : m_data) {
    item = std::make_shared<TextureBuffer>();
    item->Init();
  }
  return true;
}

void ProcessSource::TextureBufferRingQueue::DeInit() {
  for (auto &item : m_data) {
    item->DeInit();
    item = nullptr;
  }
}

void ProcessSource::TextureBufferRingQueue::Update(const std::size_t width, const std::size_t height) {
  assert(width != 0 && height != 0);

  for (auto &item : m_data) {
    item->Update(width, height);
  }

  Reset();
}