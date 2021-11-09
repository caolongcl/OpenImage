//
// Created by caolong on 2020/8/13.
//

#include <softarch/GLThread.hpp>
#include <render/egl/EGLCore.hpp>
#include <softarch/SingleThreadPool.hpp>

using namespace clt;

GLThread::GLThread(const std::string &name)
  : m_thread(new SingleThreadPool(name)),
    m_egl(nullptr),
    m_name(name),
    m_needEgl(true) {

}

bool GLThread::Init(std::shared_ptr<EGLCore> egl, bool needEgl) {
  m_needEgl = needEgl;

  m_thread->Start();

  if (m_needEgl) {
    m_thread->AddTask([this, egl]() {
      if (egl != nullptr) {
        m_egl = egl->SharedEGL();
      } else {
        m_egl = std::make_shared<EGLCore>();
      }

      Log::v(target, "%s's EGL create", m_name.c_str());
      m_egl->Init();
      m_egl->ActiveContext("");

      gles::InitGLESFunc();
    });
  }
  return true;
}

void GLThread::DeInit() {
  // 清空消息处理器
  m_thread->AddTask([this]() {
    m_msgHandlers.clear();
  });

  if (m_needEgl) {
    m_thread->AddTask([this]() {
      Log::v(target, "%s's EGL destroy", m_name.c_str());
      m_egl->DeInit();
    });
  }

  m_thread->Stop();
}

void GLThread::Post(const Task &t) {
  m_thread->AddTask(t);
}

std::thread::id GLThread::Id() const {
  return m_thread->Id();
}

void GLThread::CreateWindowSurface(const std::string &name, ANativeWindow *window) {
  assertNoEgl();
  assertThread();
  m_egl->CreateWindowSurface(name, window);
}

void GLThread::CreateOffScreenSurface(const std::string &name) {
  assertNoEgl();
  assertThread();
  m_egl->CreateOffScreenSurface(name);
}

void GLThread::DestroyWindowSurface(const std::string &name) {
  assertNoEgl();
  assertThread();
  m_egl->DestroyWindowSurface(name);
}

void GLThread::ActiveContext(const std::string &name) {
  assertNoEgl();
  assertThread();
  m_egl->ActiveContext(name);
}

void GLThread::DeActiveContext() {
  assertNoEgl();
  assertThread();
  m_egl->DeActiveContext();
}

void GLThread::Update(const std::string &name) {
  assertNoEgl();
  assertThread();
  m_egl->Update(name);
}

void GLThread::SetPresentationTime(const std::string &name, khronos_stime_nanoseconds_t nsecs) {
  assertNoEgl();
  assertThread();
  m_egl->SetPresentationTime(name, nsecs);
}

std::shared_ptr<EGLCore> GLThread::SelfEGL() {
  return m_egl;
}

/// 添加和删除只能在调用线程处理（即将来只能在调用线程处理消息）
void GLThread::AddMsgHandler(const std::shared_ptr<IMsgHandler> &handler) {
  if (handler == nullptr || handler->Target().empty()) {
    return;
  }

  Log::d(target, "add msg handler : %s", handler->Target().c_str());

  if (m_msgHandlers.find(handler->Target()) == m_msgHandlers.end()) {
    m_msgHandlers[handler->Target()] = handler;
  }
}

void GLThread::RemoveMsgHandler(const std::shared_ptr<IMsgHandler> &handler) {
  if (handler == nullptr || handler->Target().empty()) {
    return;
  }

  Log::d(target, "remove msg handler : %s", handler->Target().c_str());

  if (m_msgHandlers.find(handler->Target()) == m_msgHandlers.end()) {
    m_msgHandlers.erase(handler->Target());
  }
}

/// 可以在本线程和其他线程发送消息
void GLThread::SendMsg(const Msg &msg) {
  if (InvokeInSelf()) { // 处于本线程，同步消息
    onMsgHandle(msg);
  } else { // 处于其他线程，异步消息
    m_thread->AddTask([this, msg]() {
      onMsgHandle(msg);
    });
  }
}

void GLThread::PostMsg(const Msg &msg) {
  // 无论是不是本线程发的都当作异步消息
  m_thread->AddTask([this, msg]() {
    onMsgHandle(msg);
  });
}

bool GLThread::InvokeInSelf() const {
  return std::this_thread::get_id() == m_thread->Id();
}

void GLThread::onMsgHandle(const Msg &msg) {
  if (m_msgHandlers.find(msg.target) != m_msgHandlers.end()) {
    m_msgHandlers.at(msg.target)->OnMsgHandle(msg);
  }
}

void GLThread::assertThread() {
  assert(std::this_thread::get_id() == m_thread->Id());
}

void GLThread::assertNoEgl() {
  assert(m_needEgl);
}