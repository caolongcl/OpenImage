//
// Created by caolong on 2021/3/11.
//

#pragma once

#include <softarch/IMsg.hpp>
#include <res/TextInfo.hpp>
#include <res/model/Model.hpp>

namespace clt {
    struct PolygonMsgData : public MsgData {
        PolygonMsgData() = default;

        PolygonMsgData(std::vector<PolygonObject> &&obj)
                : objects(std::move(obj)) {}

        std::vector<PolygonObject> objects;
    };

    struct TextMsgData : public MsgData {
        TextMsgData() = default;

        TextMsgData(TextInfo &&obj)
                : textInfo(std::move(obj)) {}

        TextInfo textInfo;
    };

    struct BaseModelMsgData : public MsgData {
        BaseModelMsgData() = default;

        BaseModelMsgData(std::vector<BaseModelObject> &&obj)
                : objects(std::move(obj)) {}

        std::vector<BaseModelObject> objects;
    };
}
