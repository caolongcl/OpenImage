//
// Created by caolong on 2020/6/9.
//

#pragma once

#include <softarch/GLUtils.hpp>
#include <softarch/IComFunc.hpp>

namespace clt {

  /**
   * 纹理基类
   */
  class Texture : public IComFunc<> {
  public:
    using Buffer = unsigned char;
    using BufferPtr = std::shared_ptr<Buffer>;

  public:
    Texture() : Texture(GL_TEXTURE0) {};

    Texture(GLenum index) : m_id(0), m_textureIndex(index), m_width(0), m_height(0) {}

    virtual ~Texture() {};

    GLuint Id() const { return m_id; }

    GLsizei Width() const { return m_width; }

    GLsizei Height() const { return m_height; }

    /**
     * 创建纹理即设置纹理属性
     * @return
     */
    virtual bool Init() {
      gles::CreateTexture(m_id);
      return true;
    };

    /**
     * 释放纹理资源
     */
    virtual void DeInit() {
      gles::DeleteTexture(m_id);
    };

    virtual Texture &Upload(size_t width, size_t height, BufferPtr buffer = nullptr) {
      assert(m_id != 0);

      if (width == m_width && height == m_height && buffer == nullptr)
        return *this;

      m_width = width;
      m_height = height;

      upload(buffer);
      return *this;
    }

    virtual void Bind(GLuint uniformSamplerLoc) const = 0;

    virtual void UnBind() const = 0;

    virtual GLenum TextureIndex() const {
      return m_textureIndex;
    }

  protected:
    virtual void upload(BufferPtr buffer) = 0;

    GLint parseTextureIndex() const {
      assert(m_textureIndex >= GL_TEXTURE0 && m_textureIndex <= GL_TEXTURE31);
      return m_textureIndex - GL_TEXTURE0;
    }

  protected:
    GLuint m_id;
    GLenum m_textureIndex;
    GLsizei m_width{0};
    GLsizei m_height{0};
  };

}