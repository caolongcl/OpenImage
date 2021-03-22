//
// Created by caolong on 2020/8/10.
//

#pragma once

#include <render/filter/FilterCom.hpp>

namespace clt {

/**
 * gamma 校正
 * 0.0 ~ 3.0, default 1.0
 */
    class GammaFilter : public FilterWithShader {
    ClassDeclare(GammaFilter)
    VarDeclare(gamma)
    public:
        GammaFilter();

        ~GammaFilter() = default;

    private:
        void loadShader() override;

        void updateValue() override;

        void registerVar() override;

        GammaFilter &setGamma(const Float &gamma);

    private:
        Float1 m_gamma;
        GLint m_uniformLocGamma{};
    };

}
