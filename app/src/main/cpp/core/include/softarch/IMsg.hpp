//
// Created by caolong on 2021/3/9.
//

#pragma once

#include <softarch/std.hpp>

namespace clt {

  /**
   * 线程支持消息组件，同步消息，异步消息
   */
#define MsgDefine(what) public:constexpr static const char *msg_##what = #what;

  struct MsgData {
    MsgData() = default;

    virtual ~MsgData() = default;
  };

  /**
   * 当前支持的消息类型
   */
  struct Msg {
    enum class Type {
      MSG_POLYGON, // 绘制多边形
      MSG_BASE_MODEL, // 绘制基本 3D 物体
      MSG_TEXT, //  绘制文字
      MSG_POST_INFO, // 通知消息给 Java 层，显示到 Java 的 UI 组件上
      MSG_OTHER, // 其他，待添加实现
    };

    Msg(const std::string &_target, const std::string &_what, const std::shared_ptr<MsgData> &_data = nullptr) :
        type(Type::MSG_OTHER), target(_target), what(_what), data(_data) {}

    Msg(Type _type, const std::string &_target, const std::string &_what,
        const std::shared_ptr<MsgData> &_data = nullptr) :
        type(_type), target(_target), what(_what), data(_data) {}

    virtual ~Msg() = default;

    Type type;
    std::string target; // 目标的名字，对应 Target，对应能够处理消息的类
    std::string what;   // 消息类别
    std::shared_ptr<MsgData> data; // 消息数据
  };

  /**
   * 处理消息的接口，继承此接口的类具有处理消息的能力
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

  /**
   * 继承此接口的类能够记录具有消息处理能力的组件
   * 可以发送同步、异步消息
   */
  struct IMsg {
    IMsg() = default;

    virtual ~IMsg() = default;

    virtual void AddMsgHandler(const std::shared_ptr<IMsgHandler> &) = 0;

    virtual void RemoveMsgHandler(const std::shared_ptr<IMsgHandler> &) = 0;

    virtual void SendMsg(const Msg &msg) = 0;

    virtual void PostMsg(const Msg &msg) = 0;
  };

};
