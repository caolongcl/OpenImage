//
// Created by caolong on 2020/8/10.
//

#pragma once

#include <render/filter/FilterCom.hpp>

namespace clt {

/**
 * 火焰
 */
    class FireFilter : public FilterWithShader {
    ClassDeclare(FireFilter)

    public:
        FireFilter();

        ~FireFilter() = default;

    private:
        void loadShader() override;

        void updateValue() override;

        void registerVar() override;

    private:
        long m_startTime;

        GLint m_uniformTime{};
        GLint m_uniformLocInputSize{};
    };

}

