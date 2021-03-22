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
            CaptureSizeChanged,
            PostInfo,
        };
    public:
        PreviewControllerCallback() {}

        virtual ~PreviewControllerCallback() {}

        virtual void OnCallback(CallbackType type) = 0;
    };

}