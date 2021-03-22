//
// Created by caolong on 2020/8/10.
//

#pragma once

#include <softarch/ComType.hpp>
#include <softarch/Observer.hpp>
#include <utils/Utils.hpp>
#include <softarch/GLUtils.hpp>

namespace clt {

    class Shader;

    class Texture;

    class SquareModel;

    class OESCopier final : public IComFunc<>,
                            public Observable<OPreviewSize> {
    VarDeclare(vflip)
    VarDeclare(hflip)
    VarDeclare(ratio)
    public:
        OESCopier();

        ~OESCopier() = default;

        bool Init() override;

        void DeInit() override;

        void CopyFrame();

        bool Update();

        void SetInput(std::shared_ptr<Texture> input);

        void SetOutput(std::shared_ptr<Texture> output);

        std::shared_ptr<Texture> &GetOutput();

        OESCopier &SetSize(int width, int height);

        OESCopier &SetRotation(int rotation);

        OESCopier &SetRatio(int ratio);

        OESCopier &SetVFlip(bool vFlip);

        OESCopier &SetHFlip(bool hFlip);

    private:
        void update();

        static void update(int ratio, int originPreviewW, int originPreviewH,
                           float &xOffset, float &yOffset,
                           int &targetPreviewW, int &targetPreviewH);

        static void rotateTex(int rotate, GLfloat *tex);

        static void flipTex(bool vFlip, bool hFlip, GLfloat *tex);

        static void swapTexRow(int row0, int row1, GLfloat *tex);

        static float flip(float origin, bool flip);

    private:
        std::shared_ptr<Shader> m_shader;
        std::shared_ptr<Texture> m_input;
        std::shared_ptr<Texture> m_output;
        std::shared_ptr<SquareModel> m_square;

        int m_rotation{};
        int m_ratio{};
        bool m_vFlip{};
        bool m_hFlip{};
    };

}
