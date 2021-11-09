//
// Created by caolong on 2021/3/8.
//

#include <process/base/ProcessTextures.hpp>
#include <render/texture/NormalTexture.hpp>
#include <softarch/Constants.hpp>

using namespace clt;

ProcessTextures::ProcessTextures() = default;

bool ProcessTextures::Init() {
  Log::v(target, "ProcessTextures::Init");

  for (int i = 0; i < QUEUE_SIZE; ++i) {
    auto texture = std::make_shared<NormalTexture>(Constants::PROCESS_TEXTURE_UNIT);
    texture->Init();
    m_textures.push_back(texture);
  }

  return true;
}

void ProcessTextures::Update(const std::size_t width, const std::size_t height) {
  assert(width != 0 && height != 0);

  for (auto &tex : m_textures) {
    tex->Upload(width, height);
  }

  std::lock_guard<std::mutex> locker(m_mutex);

  resetTextureQueue();
  for (auto &tex : m_textures) {
    m_writer.push(tex);
  }
}

void ProcessTextures::DeInit() {
  std::lock_guard<std::mutex> locker(m_mutex);

  resetTextureQueue();

  for (auto &tex : m_textures) {
    tex->DeInit();
  }

  m_textures.clear();

  Log::v(target, "ProcessTextures::DeInit");
}

std::shared_ptr<Texture> ProcessTextures::PopWriteTexture() {
  std::lock_guard<std::mutex> locker(m_mutex);
  if (!m_writer.empty()) {
    auto ptr = m_writer.front();
    m_writer.pop();
    return ptr;
  }
  return nullptr;
}

std::shared_ptr<Texture> ProcessTextures::PopReadTexture() {
  std::lock_guard<std::mutex> locker(m_mutex);
  if (!m_reader.empty()) {
    auto ptr = m_reader.front();
    m_reader.pop();
    return ptr;
  }
  return nullptr;
}

void ProcessTextures::PushWriteTexture(const std::shared_ptr<Texture> &tex) {
  std::lock_guard<std::mutex> locker(m_mutex);
  m_writer.push(tex);
}

void ProcessTextures::PushReadTexture(const std::shared_ptr<Texture> &tex) {
  std::lock_guard<std::mutex> locker(m_mutex);
  m_reader.push(tex);
}

void ProcessTextures::resetTextureQueue() {
  while (!m_writer.empty())
    m_writer.pop();
  while (!m_reader.empty())
    m_reader.pop();
}