//
// Created by caolong on 2020/8/10.
//

#pragma once

#include <render/filter/FilterCom.hpp>

namespace clt {
/**
 * 漩涡效果
 */
    class WhirlpoolFilter : public FilterWithShader {
    ClassDeclare(WhirlpoolFilter)
    VarDeclare(angle)
    VarDeclare(radius)
    VarDeclare(target_position)
    VarDeclare(position_ratio)
    public:
        WhirlpoolFilter();

        ~WhirlpoolFilter() = default;

    private:
        void loadShader() override;

        void updateValue() override;

        void registerVar() override;

        WhirlpoolFilter &setAngle(const Float &angle);

        WhirlpoolFilter &setRadius(const Float &radius);

        WhirlpoolFilter &setCenter(const Float &center);

        WhirlpoolFilter &setRatio(const Float &ratio);

    private:
        Float1 m_angle;
        Float1 m_radius;
        Float2 m_center;
        Float2 m_ratio;

        GLint m_uniformLocAngle{};
        GLint m_uniformLocRadius{};
        GLint m_uniformLocInputSize{};
        GLint m_uniformLocCenter{};
    };
}
