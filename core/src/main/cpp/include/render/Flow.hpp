//
// Created by caolong on 2020/7/29.
//

#pragma once

#include <softarch/IComFunc.hpp>
#include <softarch/GLThread.hpp>
#include <softarch/IThreadPoster.hpp>
#include <softarch/IMsg.hpp>

namespace clt {

  /**
   * 管理单个线程
   * 线程可以是GL线程或普通线程
   * 基本线程 render 是渲染主线程，shared 是用来加载纹理，字体等资源的线程
   */
  class Flow final
    : public IComFunc<>,
      public IThreadPoster,
      public IMsg {
  ClassDeclare(Flow)
  public:
    bool Init() override;

    void DeInit() override;

    /// 任务添加到相应的线程中
    void Post(const Task &t, const std::string &name) override;

    void Post(const Task &t) override;

    void PostToRender(const Task &t);

    void PostToShared(const Task &t);

    /// 线程创建和销毁，获取线程，获取调用线程
    std::shared_ptr<GLThread> CreateThread(const std::string &name, bool needEgl);

    std::shared_ptr<GLThread> CreateThread(const std::string &name);

    void DestroyThread(const std::string &name);

    std::shared_ptr<GLThread> GetThread(const std::string &name);

    std::shared_ptr<GLThread> GetInvokeThread();

    /// 支持消息机制
    void AddMsgHandler(const std::shared_ptr<IMsgHandler> &handler) override;

    void RemoveMsgHandler(const std::shared_ptr<IMsgHandler> &handler) override;

    void SendMsg(const Msg &msg) override;

    void PostMsg(const Msg &msg) override;

    /// 线程检测
    bool InvokeInRender();

  private:
    // 存储所有的线程
    std::unordered_map<std::string, std::shared_ptr<GLThread>> m_flows;
    std::unordered_map<std::thread::id, std::shared_ptr<GLThread>> m_flowsById;

    // IMsgHandler 对应 Target() 的消息处理线程，根据 Target() 找到处理消息的线程，分发消息
    std::unordered_map<std::string, std::thread::id> m_msgTargets;

    // 基本线程
    std::shared_ptr<GLThread> m_render;
    std::shared_ptr<GLThread> m_shared;

  public:
    Flow(const Flow &) = delete;

    Flow &operator=(const Flow &) = delete;

    Flow(Flow &&) = delete;

    Flow &operator=(Flow &&) = delete;

    ~Flow() = default;

    static std::shared_ptr<Flow> Self() {
      std::lock_guard<std::mutex> locker(s_mutex);
      static std::shared_ptr<Flow> instance(new Flow());
      return instance;
    }

  private:
    Flow();

    static std::mutex s_mutex;
  };

}