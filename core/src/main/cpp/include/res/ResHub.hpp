//
// Created by caolong on 2020/8/7.
//

#pragma once

#include <softarch/IComFunc.hpp>
#include <utils/Log.hpp>
#include <res/font/Font.hpp>
#include <res/Shader.hpp>

namespace clt {

  /**
   * 通用资源容器
   * @tparam T
   */
  template<typename T>
  class ResHub : public IComFunc<> {
  public:
    ResHub() = default;

    ~ResHub() = default;

    bool Init() override { return true; }

    void DeInit() override {
      std::lock_guard<std::mutex> locker(m_mutex);
      clear();
    }

    void Add(const std::string &name, std::shared_ptr<T> res) {
      std::lock_guard<std::mutex> locker(m_mutex);
      if (m_res.size() > 32) {
        Log::w(Log::RES_TAG, "to many res, clear");
        clear();
      }
      m_res.emplace(name, res);
    }

    std::shared_ptr<T> Get(const std::string &name) {
      std::lock_guard<std::mutex> locker(m_mutex);
      auto ret = m_res.find(name);
      if (ret != m_res.end()) {
        return ret->second;
      } else {
        return nullptr;
      }
    }

  private:
    void clear() {
      for (auto it:m_res) {
        it.second->DeInit();
      }
      m_res.clear();
    }

  private:
    std::unordered_map<std::string, std::shared_ptr<T>> m_res;
    std::mutex m_mutex;
  };

}
