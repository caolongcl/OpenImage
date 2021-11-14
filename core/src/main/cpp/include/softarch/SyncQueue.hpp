//
// Created by caolong on 2020/6/7.
//

#pragma once

#include <softarch/std.hpp>

namespace clt {

  /**
   * 同步队列，构成线程池任务添加和删除队列
   * @tparam T
   */
  template<typename T>
  class SyncQueue {
  public:
    SyncQueue(int size) : m_maxSize(size), m_needStop(false), m_stopAdd(false) {}

    bool Add(const T &item) {
      add(item);
      return !m_needStop;
    }

    bool Add(T &&item) {
      add(std::forward<T>(item));
      return !m_needStop;
    }

    void Get(T &item) {
      std::unique_lock<std::mutex> locker(m_mutex);
      m_notEmpty.wait(locker, [this]() { return m_needStop || notEmpty(); });

      if (m_needStop) return;

      item = m_queue.front();
      m_queue.pop_front();
      m_notFull.notify_one();
    }

    void Get(std::list<T> &list) {
      std::unique_lock<std::mutex> locker(m_mutex);
      m_notEmpty.wait(locker, [this]() { return m_needStop || notEmpty(); });

      if (m_needStop) return;

      list = std::move(m_queue);
      m_notFull.notify_one();
    }

    void Clear() {
      std::unique_lock<std::mutex> locker(m_mutex);
      if (m_needStop) return;

      if (notEmpty()) {
        m_queue.clear();
        m_notFull.notify_one();
      }
    }

    void StopSync(T &&item) {
      stopSync(std::forward<T>(item));
    }

    void Stop() {
      {
        std::lock_guard<std::mutex> locker(m_mutex);
        m_needStop = true;
      }
      m_notFull.notify_all();
      m_notEmpty.notify_all();
    }

    bool Empty() {
      std::lock_guard<std::mutex> locker(m_mutex);
      return !notEmpty();
    }

    bool Full() {
      std::lock_guard<std::mutex> locker(m_mutex);
      return !notFull();
    }

    size_t Size() {
      std::lock_guard<std::mutex> locker(m_mutex);
      return m_queue.size();
    }

  private:
    bool notFull() const {
      return m_queue.size() < m_maxSize;
    }

    bool notEmpty() const {
      return !m_queue.empty();
    }

    template<typename F>
    void stopSync(F &&item) {
      std::unique_lock<std::mutex> locker(m_mutex);
      m_notFull.wait(locker, [this] { return m_needStop || m_stopAdd || notFull(); });

      if (m_needStop || m_stopAdd) return;
      m_stopAdd = true;

      m_queue.push_back([this, item1 = std::forward<F>(item)]() {
        {
          std::unique_lock<std::mutex> locker(m_mutex);
          m_needStop = true;
        }
        item1();
        m_notFull.notify_all();
        m_notEmpty.notify_all();
      });
      m_notEmpty.notify_one();
    }

    template<typename F>
    void add(F &&item) {
      std::unique_lock<std::mutex> locker(m_mutex);
      m_notFull.wait(locker, [this] { return m_needStop || m_stopAdd || notFull(); });

      if (m_needStop || m_stopAdd) return;

      m_queue.push_back(std::forward<F>(item));
      m_notEmpty.notify_one();
    }

  private:
    std::list<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_notEmpty;
    std::condition_variable m_notFull;

    int m_maxSize;
    bool m_needStop;
    bool m_stopAdd;
  };

}