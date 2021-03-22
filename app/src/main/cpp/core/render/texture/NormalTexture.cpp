//
// Created by caolong on 2020/6/9.
//

#include <render/texture/NormalTexture.hpp>

using namespace clt;

NormalTexture::NormalTexture() = default;

bool NormalTexture::Init() {
    Texture::Init();

    Log::v(Log::RENDER_TAG, "NormalTexture::Init id %d", m_id);

    bool hasError = false;

    glActiveTexture(m_textureIndex);
    glBindTexture(GL_TEXTURE_2D, m_id);
    hasError |= gles::hasGlError("glBindTexture");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 0);

    if (hasError) {
        Log::e("NormalTexture", "Init false");
    }
    return !hasError;
}

void NormalTexture::Bind(GLuint uniformSamplerLoc) const {
    glActiveTexture(m_textureIndex);
    glBindTexture(GL_TEXTURE_2D, m_id);
    glUniform1i(uniformSamplerLoc, Texture::parseTextureIndex());
}

void NormalTexture::UnBind() const {
    glActiveTexture(m_textureIndex);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void NormalTexture::DeInit() {
    Texture::DeInit();
}

void NormalTexture::upload(BufferPtr buffer) {
    // 分配纹理空间，格式是RGBA
    glActiveTexture(m_textureIndex);
    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 (GLsizei) m_width,
                 (GLsizei) m_height,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 (void *) (buffer ? buffer.get() : nullptr));
    glBindTexture(GL_TEXTURE_2D, 0);
};