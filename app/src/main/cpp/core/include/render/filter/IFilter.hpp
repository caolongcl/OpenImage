//
// Created by caolong on 2020/8/10.
//

#pragma once

#include <softarch/IComFunc.hpp>
#include <softarch/VarType.hpp>

namespace clt {

/**
 * 大于PriorityNormal优先级高，在滤镜管道最前面
 * 小于PriorityNormal优先级低，在滤镜管道最后面
 */
    enum FilterPriority {
        PriorityNormal = 0, // 默认优先级
    };

    class Texture;

/**
 * 滤镜基类
 */
    class IFilter
            : virtual public IComFunc<>,
              public IVarSet {
    public:
        IFilter() = default;

        virtual ~IFilter() = default;

        /**
         * 更新filter的参数
         * @return
         */
        virtual bool Update() = 0;

        /**
         * 是否可以执行此滤镜
         * @return
         */
        virtual bool Filterable() = 0;

        /**
         * 执行一帧
         */
        virtual void Filter() = 0;

        /**
         * 设置输入纹理
         * @param input
         */
        virtual void SetInput(std::shared_ptr<Texture> input) = 0;

        /**
         * 设置输出纹理
         * @param output
         */
        virtual void SetOutput(std::shared_ptr<Texture> output) = 0;

        /**
         * 获取输出纹理
         * @return
         */
        virtual std::shared_ptr<Texture> &GetOutput() = 0;

        /**
         * 检查滤镜的类别
         * @param type
         * @return
         */
        virtual bool CheckType(const std::string &type) = 0;

        /**
         * 获取此滤镜的优先级
         * @return
         */
        virtual const int Priority() const = 0;
    };

}
