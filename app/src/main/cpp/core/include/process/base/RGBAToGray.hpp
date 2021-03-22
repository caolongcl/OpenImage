//
// Created by caolong on 2020/8/10.
//

#pragma once

#include <render/filter/FilterCom.hpp>

namespace clt {

    class Shader;

    class SquareModel;

    class RGBAToGray : public FilterCom {
    public:
        RGBAToGray();

        ~RGBAToGray() = default;

        virtual bool Init() override;

        virtual bool Update() override { return true; }

        virtual void DeInit() override;

        void Filter() override;

        bool VarIn(const std::string &name) override { return false; }

        void VarSet(const std::string &varName, const clt::Var &var) override {}

    private:
        void loadShader();

        void updateValue();

        void registerVar();

    private:
        std::shared_ptr<Shader> m_shader;
        std::shared_ptr<SquareModel> m_square;
    };

}
