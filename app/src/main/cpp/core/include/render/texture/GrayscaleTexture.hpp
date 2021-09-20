//
// Created by caolong on 2020/8/10.
//

#pragma once

#include <render/texture/Texture.hpp>

namespace clt {

  /**
   * 单通道纹理，主要用来渲染字体
   */
  class GrayscaleTexture final : public Texture {
  public:
    GrayscaleTexture() = default;

    GrayscaleTexture(GLenum index) : Texture(index) {}

    ~GrayscaleTexture() = default;

    bool Init() override;

    void DeInit() override;

    void Bind(GLuint uniformSamplerLoc) const override;

    void UnBind() const override;

    void upload(BufferPtr buffer) override;
  };
}
