//
// Created by caolong on 2021/11/14.
//

#include <process/base/TextureBuffer.hpp>
#include <render/texture/NormalTexture.hpp>
#include <softarch/Constants.hpp>
#include <softarch/Buffer.hpp>

using namespace clt;

bool TextureBuffer::Init() {
  m_texture = std::make_shared<NormalTexture>(Constants::PROCESS_TEXTURE_UNIT);
  m_texture->Init();
  m_buffer = std::make_shared<Buffer>();
  return true;
}

void TextureBuffer::DeInit() {
  m_texture->DeInit();
  m_texture = nullptr;
  m_buffer = nullptr;
}

void TextureBuffer::Update(const std::size_t width, const std::size_t height) {
  assert(width != 0 && height != 0);
  m_texture->Upload(width, height);
  m_buffer = std::make_shared<Buffer>(width, height, 4);
}