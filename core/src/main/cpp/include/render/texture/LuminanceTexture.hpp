//
// Created by caolong on 2020/8/10.
//

#include <render/texture/Texture.hpp>

namespace clt {

  class LuminanceTexture final : public Texture {
  public:
    LuminanceTexture() = default;

    LuminanceTexture(GLenum index) : Texture(index) {}

    ~LuminanceTexture() = default;

    bool Init() override;

    void DeInit() override;

    void Bind(GLuint uniformSamplerLoc) const override;

    void UnBind() const override;

    void upload(BufferPtr buffer) override;
  };
}

