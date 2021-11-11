//
// Created by caolong on 2020/7/29.
//

#include <softarch/ComType.hpp>
#include <render/Flow.hpp>
#include <render/egl/EGLCore.hpp>

using namespace clt;

std::mutex Flow::s_mutex;

Flow::Flow()
  : m_render(nullptr),
    m_shared(nullptr) {
}

bool Flow::Init() {
  Log::v(target, "Flow::Init");

  // 基本线程
  m_render = std::make_shared<GLThread>("render");
  m_shared = std::make_shared<GLThread>("shared");

  bool needEgl = true;
  m_render->Init(nullptr, needEgl);
  m_render->Post([this, needEgl]() {
    m_shared->Init(m_render->SelfEGL(), needEgl);
  });

  return true;
}

void Flow::DeInit() {
  m_shared->DeInit();
  m_shared = nullptr;

  m_render->DeInit();
  m_render = nullptr;

  Log::v(target, "Flow::DeInit");
}

void Flow::Post(const Task &t, const std::string &name) {
  if (m_flows.find(name) != m_flows.end()) {
    m_flows.at(name)->Post(t);
  }
}

void Flow::Post(const Task &t) {
  PostToRender(t);
}

void Flow::PostToRender(const Task &t) {
  if (m_render != nullptr) {
    m_render->Post(t);
  }
}

void Flow::PostToShared(const Task &t) {
  if (m_shared != nullptr) {
    m_shared->Post(t);
  }
}

std::shared_ptr<GLThread> Flow::CreateThread(const std::string &name, bool needEgl, bool syncStop) {
  if (m_flows.find(name) == m_flows.end()) {
    if (m_render == nullptr) {
      return nullptr;
    }

    auto thread = std::make_shared<GLThread>(name, syncStop);
    thread->Init(m_render->SelfEGL(), needEgl);

    {
      std::lock_guard<std::mutex> locker(s_mutex);
      m_flows.emplace(name, thread);
      m_flowsById[thread->Id()] = thread;
    }
    return thread;
  } else {
    return m_flows.at(name);
  }
}

std::shared_ptr<GLThread> Flow::CreateThread(const std::string &name, bool needEgl) {
  return CreateThread(name, needEgl, true);
}

std::shared_ptr<GLThread> Flow::CreateThread(const std::string &name) {
  return CreateThread(name, false);
}

void Flow::DestroyThread(const std::string &name) {
  if (m_flows.find(name) != m_flows.end()) {
    auto thread = m_flows.at(name);
    auto id = thread->Id();
    thread->DeInit();

    {
      std::lock_guard<std::mutex> locker(s_mutex);
      // 删除对应线程的 handlerTargets
      auto tmp = m_msgTargets;
      m_msgTargets.clear();

      for (auto &elem : tmp) {
        if (elem.second != id) {
          m_msgTargets.insert(elem);
        }
      }

      m_flowsById.erase(id);
      m_flows.erase(name);
    }
  }
}

std::shared_ptr<GLThread> Flow::GetThread(const std::string &name) {
  if (m_flows.find(name) != m_flows.end()) {
    return m_flows.at(name);
  }
  return nullptr;
}

std::shared_ptr<GLThread> Flow::GetInvokeThread() {
  auto curId = std::this_thread::get_id();

  if (m_render != nullptr && curId == m_render->Id()) {
    return m_render;
  } else if (m_shared != nullptr && curId == m_shared->Id()) {
    return m_shared;
  } else if (m_flowsById.find(curId) != m_flowsById.end()) {
    return m_flowsById.at(curId);
  } else {
    // 超出了Flow的作用域，不可能
    assert(false);
  }
  return nullptr;
}

/// 将消息处理器注册给此函数的调用线程
void Flow::AddMsgHandler(const std::shared_ptr<IMsgHandler> &handler) {
  if (handler == nullptr)
    return;

  auto curId = std::this_thread::get_id();

  if (m_render != nullptr && curId == m_render->Id()) {
    m_render->AddMsgHandler(handler);
  } else if (m_shared != nullptr && curId == m_shared->Id()) {
    m_shared->AddMsgHandler(handler);
  } else {
    if (m_flowsById.find(curId) != m_flowsById.end()) {
      m_flowsById.at(curId)->AddMsgHandler(handler);
    } else {
      // 只有 Flow 产生的线程具有消息机制
      curId = std::thread::id();
    }
  }

  if (curId != std::thread::id()) {
    std::lock_guard<std::mutex> locker(s_mutex);
    m_msgTargets[handler->Target()] = curId;
  }
}

void Flow::RemoveMsgHandler(const std::shared_ptr<IMsgHandler> &handler) {
  if (handler == nullptr)
    return;

  auto curId = std::this_thread::get_id();

  if (m_render != nullptr && curId == m_render->Id()) {
    m_render->RemoveMsgHandler(handler);
  } else if (m_shared != nullptr && curId == m_shared->Id()) {
    m_shared->RemoveMsgHandler(handler);
  } else if (m_flowsById.find(curId) != m_flowsById.end()) {
    m_flowsById.at(curId)->RemoveMsgHandler(handler);
  }

  std::lock_guard<std::mutex> locker(s_mutex);
  m_msgTargets.erase(handler->Target());
}

void Flow::SendMsg(const Msg &msg) {
  std::thread::id id;

  {
    std::lock_guard<std::mutex> locker(s_mutex);
    if (m_msgTargets.find(msg.target) != m_msgTargets.end()) {
      id = m_msgTargets.at(msg.target);
    }
  }

  if (m_render != nullptr && id == m_render->Id()) {
    m_render->SendMsg(msg);
  } else if (m_shared != nullptr && id == m_shared->Id()) {
    m_shared->SendMsg(msg);
  } else if (m_flowsById.find(id) != m_flowsById.end()) {
    m_flowsById.at(id)->SendMsg(msg);
  }
}

void Flow::PostMsg(const Msg &msg) {
  std::thread::id id;

  {
    std::lock_guard<std::mutex> locker(s_mutex);
    if (m_msgTargets.find(msg.target) != m_msgTargets.end()) {
      id = m_msgTargets.at(msg.target);
    }
  }

  if (m_render != nullptr && id == m_render->Id()) {
    m_render->PostMsg(msg);
  } else if (m_shared != nullptr && id == m_shared->Id()) {
    m_shared->PostMsg(msg);
  } else if (m_flowsById.find(id) != m_flowsById.end()) {
    m_flowsById.at(id)->PostMsg(msg);
  }
}

bool Flow::InvokeInRender() {
  return m_render != nullptr && std::this_thread::get_id() == m_render->Id();
}


