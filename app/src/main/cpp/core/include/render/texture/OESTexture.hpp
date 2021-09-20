//
// Created by caolong on 2020/8/10.
//

#pragma once

#include <render/texture/Texture.hpp>

namespace clt {

  /**
   * 相机输出的预览纹理格式
   */
  class OESTexture final : virtual public Texture {
  public:
    OESTexture();

    ~OESTexture() = default;

    bool Init() override;

    void DeInit() override;

    void Bind(GLuint uniformSamplerLoc) const override;

    void UnBind() const override;

    void upload(BufferPtr buffer) override {}
  };
}
