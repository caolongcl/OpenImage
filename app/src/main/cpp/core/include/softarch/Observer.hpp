//
// Created by caolong on 2020/8/1.
//

#pragma once

#include <softarch/std.hpp>

namespace clt {

    /**
     * 观察者组件
     * @tparam T
     */
    template<typename T>
    struct Observer {
        Observer() = default;

        virtual ~Observer() = default;

        virtual void OnUpdate(T &&t) = 0;
    };


    struct ObserverToJava {
    };

    /**
     * 被观察者组件
     * @tparam T
     */
    template<typename T>
    struct Observable {
        Observable() = default;

        virtual ~Observable() {
            m_observers.clear();
        }

        void AddObserver(std::weak_ptr<Observer<T>> o) {
            if (!found(o)) {
                m_observers.push_back(std::move(o));
            }
        }

        void DelObserver(std::weak_ptr<Observer<T>> o) {
            if (found(o)) {
                m_observers.remove(o);
            }
        }

        void ClearObservers() {
            m_observers.clear();
        }

        void Notify(T &&t) {
            for (std::weak_ptr<Observer<T>> &observer:m_observers) {
                if (!observer.expired()) {
                    observer.lock()->OnUpdate(std::forward<T>(t));
                }
            }
        }

    private:
        bool found(std::weak_ptr<Observer<T>> o) {
            bool ret = false;

            const auto &os = o.lock();
            if (os == nullptr)
                return false;

            for (std::weak_ptr<Observer<T>> &observer:m_observers) {
                if (os == observer.lock()) {
                    ret = true;
                    break;
                }
            }

            return ret;
        }

    private:
        std::list<std::weak_ptr<Observer<T>>> m_observers;
    };

}