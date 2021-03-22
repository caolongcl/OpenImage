//
// Created by caolong on 2021/3/9.
//

#pragma once

#include <softarch/std.hpp>

namespace clt {

#define MsgDefine(what) public:constexpr static const char *msg_##what = #what;

    struct MsgData {
        MsgData() = default;

        virtual ~MsgData() = default;
    };

    struct Msg {
        enum class Type {
            MSG_POLYGON,
            MSG_BASE_MODEL,
            MSG_TEXT,
            MSG_POST_INFO,
            MSG_OTHER,
        };

        Msg(const std::string _target, const std::string _what, std::shared_ptr<MsgData> _data = nullptr) :
                type(Type::MSG_OTHER), target(_target), what(_what), data(_data) {}

        Msg(Type _type, const std::string _target, const std::string _what, std::shared_ptr<MsgData> _data = nullptr) :
                type(_type), target(_target), what(_what), data(_data) {}

        virtual ~Msg() = default;

        Type type;
        std::string target; // 目标的名字，对应 Target，对应能够处理消息的类
        std::string what;   // 消息类别
        std::shared_ptr<MsgData> data; // 消息数据
    };

    struct PolygonMsg : public Msg {
        PolygonMsg(const std::string _target, const std::string _what, std::shared_ptr<MsgData> _data = nullptr) :
                Msg(Type::MSG_POLYGON, _target, _what, _data) {}
    };

    struct TextMsg : public Msg {
        TextMsg(const std::string _target, const std::string _what, std::shared_ptr<MsgData> _data = nullptr) :
                Msg(Type::MSG_TEXT, _target, _what, _data) {}
    };

    struct PosInfoMsg : public Msg {
        PosInfoMsg(const std::string _target, const std::string _what) :
                Msg(Type::MSG_POST_INFO, _target, _what) {}
    };

    struct BaseModelMsg : public Msg {
        BaseModelMsg(const std::string _target, const std::string _what, std::shared_ptr<MsgData> _data = nullptr) :
                Msg(Type::MSG_BASE_MODEL, _target, _what, _data) {}
    };

/**
 * 处理消息的接口
 */
    struct IMsgHandler : public std::enable_shared_from_this<IMsgHandler> {
        IMsgHandler() = default;

        virtual ~IMsgHandler() = default;

        virtual const std::string &Target() const = 0;

        virtual void OnMsgHandle(const Msg &msg) = 0;
    };

    struct MsgHandler : public IMsgHandler {
        MsgHandler(const std::string &name) : m_name(name) {}

        virtual ~MsgHandler() = default;

        virtual const std::string &Target() const {
            return m_name;
        }

    private:
        std::string m_name;
    };

    struct IMsg {
        IMsg() = default;

        virtual ~IMsg() = default;

        virtual void AddMsgHandler(std::shared_ptr<IMsgHandler>) = 0;

        virtual void RemoveMsgHandler(std::shared_ptr<IMsgHandler>) = 0;

        virtual void SendMsg(const Msg &msg) = 0;

        virtual void PostMsg(const Msg &msg) = 0;
    };

};
