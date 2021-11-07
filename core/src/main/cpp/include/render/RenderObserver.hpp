//
// Created by caolong on 2020/7/20.
//

#pragma once

#include <softarch/PreviewControllerCallback.hpp>
#include <softarch/ComType.hpp>
#include <softarch/IComFunc.hpp>
#include <softarch/Observer.hpp>

namespace clt {

  struct RenderObserver : public IComFunc<std::shared_ptr<PreviewControllerCallback>>,
                          public Observer<OPreviewSize>,
                          public Observer<ObserverToJava> {
    using ParamType = PreviewControllerCallback;
    using MemberType = std::shared_ptr<ParamType>;

    bool Init(MemberType callback) override {
      m_callback = std::move(callback);
      return m_callback != nullptr;
    }

    void DeInit() override {
      m_callback = nullptr;
    }

    void OnUpdate(OPreviewSize &&t) override {
      if (m_callback != nullptr) {
        m_callback->OnCallback(ParamType::CallbackType::CaptureSizeChanged);
      }
    }

    void OnUpdate(ObserverToJava &&) override {
      if (m_callback != nullptr) {
        m_callback->OnCallback(ParamType::CallbackType::PostInfo);
      }
    }

  private:
    MemberType m_callback;
  };

}

