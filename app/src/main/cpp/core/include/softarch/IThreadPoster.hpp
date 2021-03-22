//
// Created by caolong on 2020/8/13.
//

#pragma once

#include <softarch/Task.hpp>

namespace clt {

/**
 * 具有将任务加入到线程中的能力
 */
    struct IThreadPoster {
        IThreadPoster() = default;

        virtual ~IThreadPoster() = default;

        /**
         * 将任务t加入到线程name中
         * @param t
         * @param name
         */
        virtual void Post(const Task &t, const std::string &name) = 0;

        virtual void Post(const Task &t) = 0;
    };

}
