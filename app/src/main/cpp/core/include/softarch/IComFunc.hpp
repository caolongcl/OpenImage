//
// Created by caolong on 2020/6/22.
//

#pragma once

namespace clt {

    template<typename ...Args>
    class IComFunc {
    public:
        IComFunc() = default;

        virtual ~IComFunc() = default;

        virtual bool Init(Args ...args) = 0;

        virtual void DeInit() = 0;
    };

    template<>
    class IComFunc<> {
    public:
        IComFunc() = default;

        virtual ~IComFunc() = default;

        virtual bool Init() = 0;

        virtual void DeInit() = 0;
    };

/**
 * 提供更新
 * @tparam Args
 */
    template<typename ...Args>
    struct IComUpdate {
        IComUpdate() = default;

        virtual ~IComUpdate() = default;

        virtual void Update(Args ...) = 0;
    };

    template<>
    struct IComUpdate<> {
        IComUpdate() = default;

        virtual ~IComUpdate() = default;

        virtual void Update() = 0;
    };

}