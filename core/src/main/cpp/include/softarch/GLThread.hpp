//
// Created by caolong on 2020/8/13.
//

#pragma once

#include <render/egl/IEGL.hpp>
#include <softarch/IComFunc.hpp>
#include <softarch/ThreadPool.hpp>
#include <softarch/IMsg.hpp>


namespace clt {

  class EGLCore;

  class SingleThreadPool;

  /**
   * 不同于普通的线程，GLThread 可以拥有 EGL 环境
   */
  class GLThread final : public IComFunc<std::shared_ptr<EGLCore>, bool>,
                         public IEGL,
                         public IMsg {
  ClassDeclare(GLThread)
  public:
    explicit GLThread(const std::string &name);

    ~GLThread() = default;

    bool Init(std::shared_ptr<EGLCore>, bool needEgl) override;

    void DeInit() override;

    void Post(const Task &t);

    std::thread::id Id() const;

    /// 以下方法必须在本线程调用，且有 EGL的情况下
    void CreateWindowSurface(const std::string &name, ANativeWindow *window) override;

    void CreateOffScreenSurface(const std::string &name) override;

    void DestroyWindowSurface(const std::string &name) override;

    void ActiveContext(const std::string &name) override;

    void DeActiveContext() override;

    void Update(const std::string &name) override;

    void SetPresentationTime(const std::string &name, khronos_stime_nanoseconds_t nsecs) override;

    /**
     * 返回本线程的 EGL，其他 GLThread 可以共享
     * @return
     */
    std::shared_ptr<EGLCore> SelfEGL();

    /// 支持消息
    void AddMsgHandler(const std::shared_ptr<IMsgHandler> &handler) override;

    void RemoveMsgHandler(const std::shared_ptr<IMsgHandler> &handler) override;

    void SendMsg(const Msg &msg) override;

    void PostMsg(const Msg &msg) override;

    // 检查调用线程是否就是自己
    bool InvokeInSelf() const;

  private:
    /**
     * 确保某些方法必须在m_thread线程中调用
     */
    void assertThread();

    void assertNoEgl();

    void onMsgHandle(const Msg &);

  private:
    std::shared_ptr<SingleThreadPool> m_thread;
    std::shared_ptr<EGLCore> m_egl;
    std::string m_name;
    bool m_needEgl;

    // IMsgHandler 持有者名字，Target()，保存了要此线程处理消息的对象
    std::unordered_map<std::string, std::shared_ptr<IMsgHandler>> m_msgHandlers;
  };

}
