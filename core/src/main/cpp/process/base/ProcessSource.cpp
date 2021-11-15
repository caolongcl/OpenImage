//
// Created by caolong on 2021/11/14.
//

#include <process/base/ProcessSource.hpp>
#include <process/base/TextureBuffer.hpp>
#include <utils/Log.hpp>
#include <process/base/PixelReader.hpp>

using namespace clt;

ProcessSource::ProcessSource(int size)
  : m_size(size),
    m_pixelReader(new PixelReaderPbo()) {
}

bool ProcessSource::Init() {
  assert(m_size != 0);

  m_pixelReader->Init();

  m_queue = std::make_shared<TextureBufferRingQueue>(m_size);
  m_queue->Init();

  return true;
}

void ProcessSource::DeInit() {
  m_queue->DeInit();
  m_pixelReader->DeInit();
}

void ProcessSource::Update(const std::size_t width, const std::size_t height) {
  assert(width != 0 && height != 0);
  m_queue->Update(width, height);
  m_pixelReader->Update(width, height);
}

void ProcessSource::Feed(const Feeder &feeder) {
  SourceItem textureBuffer;
  if (m_queue->Rear(textureBuffer)) {
    feeder(textureBuffer->GetTexture());
    m_queue->Push();
  }
}

bool ProcessSource::Eat(const Eater &eater) {
  SourceItem textureBuffer;
  if (m_queue->Last(textureBuffer)) {
    eater(textureBuffer);
    m_queue->Pop();
    return true;
  }
  return false;
}

void ProcessSource::Read(std::shared_ptr<Texture> tex, std::shared_ptr<Buffer> buf) {
  m_pixelReader->Read(tex, buf);
}

void ProcessSource::Reset() {
  m_queue->Reset();
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