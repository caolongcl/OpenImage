//
// Created by caolong on 2020/7/3.
//

#pragma once

namespace clt {

  /**
   * GLRender回调给PreviewController
   */
  class PreviewControllerCallback {
  public:
    enum class CallbackType {
      // 预览大小改变，通知 Java 层改变截帧 Surface 的大小
      CaptureSizeChanged,
      // native 层可以通知 Java 层显示信息
      PostInfo,
    };
  public:
    PreviewControllerCallback() {}

    virtual ~PreviewControllerCallback() {}

    virtual void OnCallback(CallbackType type) = 0;
  };

}