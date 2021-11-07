//
// Created by caolong on 2021/3/8.
//

#include <process/base/ProcessBuffers.hpp>
#include <utils/Log.hpp>
#include <softarch/Buffer.hpp>

using namespace clt;

ProcessBuffers::ProcessBuffers() = default;

bool ProcessBuffers::Init() {
  Log::v(Log::PROCESSOR_TAG, "ProcessPipe::Init");

//  resetBufferQueue();

  return true;
}

void ProcessBuffers::Update(const std::size_t width, const std::size_t height) {
  assert(width != 0 && height != 0);

  {
    std::lock_guard<std::mutex> locker(m_mutex);

    resetBufferQueue();

    m_buffers.clear();

    for (int i = 0; i < QUEUE_SIZE; ++i) {
      auto buf = std::make_shared<Buffer>(width, height, 4);
      m_buffers.push_back(buf);
      m_writer.push(buf);
    }
  }

  Log::v(Log::PROCESSOR_TAG,
         "ProcessBuffers::Update width %d height %d buf bytes %d",
         width, height, m_buffers[0]->bytes);
}

void ProcessBuffers::DeInit() {
  std::lock_guard<std::mutex> locker(m_mutex);

  resetBufferQueue();

  m_buffers.clear();

  Log::v(Log::PROCESSOR_TAG, "ProcessBuffers::DeInit");
}

std::shared_ptr<Buffer> ProcessBuffers::PopWriteBuffer() {
  std::lock_guard<std::mutex> locker(m_mutex);
  if (!m_writer.empty()) {
    auto ptr = m_writer.front();
    m_writer.pop();
    return ptr;
  }
  return nullptr;
}

std::shared_ptr<Buffer> ProcessBuffers::PopReadBuffer() {
  std::lock_guard<std::mutex> locker(m_mutex);
  if (!m_reader.empty()) {
    auto ptr = m_reader.front();
    m_reader.pop();
    return ptr;
  }
  return nullptr;
}

void ProcessBuffers::PushWriteBuffer(const std::shared_ptr<Buffer> &buffer) {
  std::lock_guard<std::mutex> locker(m_mutex);
  m_writer.push(buffer);
}

void ProcessBuffers::PushReadBuffer(const std::shared_ptr<Buffer> &buffer) {
  std::lock_guard<std::mutex> locker(m_mutex);
  m_reader.push(buffer);
}

void ProcessBuffers::resetBufferQueue() {
  while (!m_writer.empty())
    m_writer.pop();
  while (!m_reader.empty())
    m_reader.pop();
}
