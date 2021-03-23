//
// Created by caolong on 2021/3/11.
//

#pragma once

#include <softarch/IMsg.hpp>
#include <res/TextInfo.hpp>
#include <res/model/Model.hpp>

namespace clt {
    struct PolygonMsg : public Msg {
        PolygonMsg(const std::string& _target, const std::string& _what, const std::shared_ptr<MsgData>& _data = nullptr) :
                Msg(Type::MSG_POLYGON, _target, _what, _data) {}
    };

    struct TextMsg : public Msg {
        TextMsg(const std::string& _target, const std::string& _what, const std::shared_ptr<MsgData>& _data = nullptr) :
                Msg(Type::MSG_TEXT, _target, _what, _data) {}
    };

    struct PosInfoMsg : public Msg {
        PosInfoMsg(const std::string& _target, const std::string& _what) :
                Msg(Type::MSG_POST_INFO, _target, _what) {}
    };

    struct BaseModelMsg : public Msg {
        BaseModelMsg(const std::string& _target, const std::string& _what, const std::shared_ptr<MsgData>& _data = nullptr) :
                Msg(Type::MSG_BASE_MODEL, _target, _what, _data) {}
    };


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
