//
// Created by caolong on 2020/8/10.
//

#pragma once

#include <render/texture/Texture.hpp>

namespace clt {

    class NormalTexture final : virtual public Texture {
    public:
        NormalTexture();

        NormalTexture(GLenum index) : Texture(index) {}

        ~NormalTexture() = default;

        bool Init() override;

        void DeInit() override;

        void Bind(GLuint uniformSamplerLoc) const override;

        void UnBind() const override;

        void upload(BufferPtr buffer) override;
    };
}