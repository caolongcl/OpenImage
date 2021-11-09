//
// Created by caolong on 2020/6/7.
//

#include <softarch/GLUtils.hpp>
#include <softarch/ComType.hpp>
#include <render/texture/OESTexture.hpp>

using namespace clt;

OESTexture::OESTexture() = default;

bool OESTexture::Init() {
  Texture::Init();

  Log::v(target, "OESTexture::Init id %d", m_id);

  bool hasError = false;

  glActiveTexture(m_textureIndex);
  glBindTexture(GL_TEXTURE_EXTERNAL_OES, m_id);
  hasError |= gles::hasGlError("glBindTexture");

  glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glBindTexture(GL_TEXTURE_EXTERNAL_OES, 0);

  if (hasError) {
    Log::e("OESTexture", "Init false");
  }
  return !hasError;
}

void OESTexture::DeInit() {
  Texture::DeInit();
}

void OESTexture::Bind(GLuint uniformSamplerLoc) const {
  glActiveTexture(m_textureIndex);
  glBindTexture(GL_TEXTURE_EXTERNAL_OES, m_id);
  glUniform1i(uniformSamplerLoc, Texture::parseTextureIndex());
}

void OESTexture::UnBind() const {
  glActiveTexture(m_textureIndex);
  glBindTexture(GL_TEXTURE_EXTERNAL_OES, 0);
}

