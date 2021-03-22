//
// Created by caolong on 2020/8/10.
//

#pragma once

#include <render/filter/FilterCom.hpp>

namespace clt {
/**
 * Mosaic效果
 */
    class MosaicFilter : public FilterWithShader {
    ClassDeclare(MosaicFilter)
    VarDeclare(mosaic_size)
    public:
        MosaicFilter();

        ~MosaicFilter() = default;

    private:
        void loadShader() override;

        void updateValue() override;

        void registerVar() override;

        MosaicFilter &setMosaicSize(const Float &mosaicSize);

    private:
        Float2 m_mosaicSize;

        GLint m_uniformLocMosaicSize{};
        GLint m_uniformLocInputSize{};
    };
}