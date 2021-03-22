//
// Created by caolong on 2020/8/10.
//

#pragma once

#include <res/TextInfo.hpp>
#include <softarch/ComType.hpp>

namespace clt {

    struct IRenderText {
        IRenderText() = default;

        virtual ~IRenderText() = default;

        virtual void RenderText(const TextInfo &info) = 0;
    };

    struct IPrint {
        IPrint() = default;

        virtual ~IPrint() = default;

        virtual void Print(const TextInfo &info) = 0;
    };

}
