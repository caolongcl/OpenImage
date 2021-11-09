//
// Created by caolong on 2020/7/26.
//

#include <render/texture/GrayscaleTexture.hpp>

using namespace clt;

bool GrayscaleTexture::Init() {
  Texture::Init();

  Log::v(target, "GrayscaleTexture::Init id %d", m_id);

  bool hasError = false;

  glActiveTexture(m_textureIndex);
  glBindTexture(GL_TEXTURE_2D, m_id);
  hasError |= gles::hasGlError("glBindTexture");

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glBindTexture(GL_TEXTURE_2D, 0);

  if (hasError) {
    Log::e("GrayscaleTexture", "Init false");
  }
  return !hasError;
}

void GrayscaleTexture::Bind(GLuint uniformSamplerLoc) const {
  glActiveTexture(m_textureIndex);
  glBindTexture(GL_TEXTURE_2D, m_id);
  glUniform1i(uniformSamplerLoc, Texture::parseTextureIndex());
}

void GrayscaleTexture::UnBind() const {
  glActiveTexture(m_textureIndex);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void GrayscaleTexture::DeInit() {
  Texture::DeInit();
}

void GrayscaleTexture::upload(BufferPtr buffer) {
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //禁用字节对齐限制

  // 分配纹理空间，格式是Red
  glActiveTexture(m_textureIndex);
  glBindTexture(GL_TEXTURE_2D, m_id);
  glTexImage2D(GL_TEXTURE_2D,
               0,
               GL_R8, // 内部存储格式
               (GLsizei) m_width,
               (GLsizei) m_height,
               0,
               GL_RED, // 输入纹理数据格式
               GL_UNSIGNED_BYTE,
               (void *) (buffer ? buffer.get() : nullptr));
  glBindTexture(GL_TEXTURE_2D, 0);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 4); //恢复字节对齐限制
};