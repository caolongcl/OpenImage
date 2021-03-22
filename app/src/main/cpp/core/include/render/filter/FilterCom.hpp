//
// Created by caolong on 2020/8/10.
//

#pragma once

#include <res/Shader.hpp>
#include <res/SquareModel.hpp>
#include <render/filter/IFilter.hpp>
#include <utils/Utils.hpp>

namespace clt {

    /**
     * 滤镜通用实现
     */
    class FilterCom : public IFilter {
    public:
        FilterCom(const std::string &type, const int priority)
                : m_type(type), m_priority(priority), m_needUpdate(true) {}

        virtual ~FilterCom() = default;

        virtual void SetInput(std::shared_ptr<Texture> input) override {
            m_input = input;
        }

        virtual void SetOutput(std::shared_ptr<Texture> output) override {
            m_output = output;
        }

        virtual std::shared_ptr<Texture> &GetOutput() override {
            return m_output;
        }

        bool CheckType(const std::string &type) override {
            return type == m_type;
        }

        const int Priority() const override { return m_priority; }

        bool Filterable() override { return m_filterable; }

    private:
        std::string m_type;
        int m_priority;
    protected:
        std::shared_ptr<Texture> m_input;
        std::shared_ptr<Texture> m_output;
        bool m_needUpdate;
        bool m_filterable;
    };

    /**
     * 自带Shader的滤镜的基类,可以设置参数
     */
    class FilterWithShader : public FilterCom,
                             virtual public IComFunc<std::shared_ptr<SquareModel>> {
    public:
        FilterWithShader(const std::string &type, const int priority = PriorityNormal)
                : FilterCom(type, priority) {};

        virtual ~FilterWithShader() = default;

        bool Init() override { return false; }

        virtual bool Init(std::shared_ptr<SquareModel>) override;

        virtual bool Update() override { return true; }

        virtual void DeInit() override;

        void Filter() override;

        bool VarIn(const std::string &name) override;

        void VarSet(const std::string &varName, const Var &var) override;

    protected:
        /**
         * 加载对应的shader
         */
        virtual void loadShader() = 0;

        /**
         * 每帧更新需要传递给shader的值
         */
        virtual void updateValue() = 0;

        /**
         * 注册此滤镜能支持传入的参数
         */
        virtual void registerVar() = 0;

        /**
         * 此滤镜的shader
         */
        std::shared_ptr<Shader> m_shader;

        /**
         * 存储此滤镜支持的参数
         */
        std::shared_ptr<VarGroup> m_varGroup;

    private:
        std::shared_ptr<SquareModel> m_model;
    };
}
