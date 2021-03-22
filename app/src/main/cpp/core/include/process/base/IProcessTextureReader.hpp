//
// Created by caolong on 2021/3/9.
//

#pragma once

#include <softarch/GLUtils.hpp>

namespace clt {

    struct IProcessTextureReader {
        IProcessTextureReader() = default;

        virtual ~IProcessTextureReader() = default;

        virtual std::shared_ptr<Texture> PopWriteTexture() = 0;

        virtual void PushReadTexture(std::shared_ptr<Texture>) = 0;

        virtual void Process() = 0;
    };

};
