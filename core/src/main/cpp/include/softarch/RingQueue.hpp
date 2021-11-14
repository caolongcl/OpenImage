//
// Created by caolong on 2021/11/13.
//

#pragma once

#include <softarch/std.hpp>

namespace clt {

  /**
   * 环形队列
   * @tparam T
   */
  template<typename T>
  class RingQueue {
  public:
    RingQueue(int size)
      : m_size(size),
        m_front(0),
        m_rear(0),
        m_freeSize(size),
        m_data(size) {
    }

    void Reset() {
      std::lock_guard<std::mutex> lockGuard(m_mutex);
      m_front = 0;
      m_rear = 0;
      m_freeSize = m_size;
    }

    bool Front(T &item) {
      std::lock_guard<std::mutex> lockGuard(m_mutex);
      if (!empty()) {
        item = front();
        return true;
      }
      return false;
    }

    bool Last(T &item) {
      std::lock_guard<std::mutex> lockGuard(m_mutex);
      if (last()) {
        item = front();
        return true;
      }
      return false;
    }

    bool Rear(T &item) {
      std::lock_guard<std::mutex> lockGuard(m_mutex);
      if (!full()) {
        item = rear();
        return true;
      }
      return false;
    }

    void Pop() {
      std::lock_guard<std::mutex> lockGuard(m_mutex);
      pop();
    }

    void Push() {
      std::lock_guard<std::mutex> lockGuard(m_mutex);
      push();
    }

  private:
    bool empty() const {
      return m_rear == m_front && m_freeSize == 0;
    }

    bool full() const {
      return m_rear == m_front && m_freeSize == m_size;
    }

    void push() {
      if (!full()) {
        forward(m_rear);
        ++m_freeSize;
      }
    }

    void pop() {
      if (!empty()) {
        forward(m_front);
        --m_freeSize;
      }
    }

    bool last() {
      if (!empty()) {
        while (m_freeSize != 1) {
          forward(m_front);
          --m_freeSize;
        }
        return true;
      }

      return false;
    }

    T &front() {
      return m_data[m_front];
    }

    T &rear() {
      return m_data[m_rear];
    }

    int forward(int &index) {
      index = (++index) % m_size;
    }
  protected:
    int m_size;
    int m_front;
    int m_rear;
    int m_freeSize;
    std::vector<T> m_data;
    std::mutex m_mutex;
  };
}