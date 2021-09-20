//
// Created by caolong on 2020/8/10.
//

#include <render/texture/LuminanceTexture.hpp>

using namespace clt;

bool LuminanceTexture::Init() {
  Texture::Init();

  Log::v(Log::RENDER_TAG, "LuminanceTexture::Init id %d", m_id);

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
    Log::e("LuminanceTexture", "Init false");
  }
  return !hasError;
}

void LuminanceTexture::Bind(GLuint uniformSamplerLoc) const {
  glActiveTexture(m_textureIndex);
  glBindTexture(GL_TEXTURE_2D, m_id);
  glUniform1i(uniformSamplerLoc, Texture::parseTextureIndex());
}

void LuminanceTexture::UnBind() const {
  glActiveTexture(m_textureIndex);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void LuminanceTexture::DeInit() {
  Texture::DeInit();
}

void LuminanceTexture::upload(BufferPtr buffer) {
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //禁用字节对齐限制

  // 分配纹理空间，格式是Red
  glActiveTexture(m_textureIndex);
  glBindTexture(GL_TEXTURE_2D, m_id);
  glTexImage2D(GL_TEXTURE_2D,
               0,
               GL_LUMINANCE, // 内部存储格式
               (GLsizei) m_width,
               (GLsizei) m_height,
               0,
               GL_LUMINANCE, // 输入纹理数据格式
               GL_UNSIGNED_BYTE,
               (void *) (buffer ? buffer.get() : nullptr));
  glBindTexture(GL_TEXTURE_2D, 0);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 4); //恢复字节对齐限制
};