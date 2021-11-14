//
// Created by caolong on 2021/11/13.
//

#pragma once

#include <softarch/std.hpp>
#include <utils/Log.hpp>

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
        m_frontSize(0),
        m_data(size) {
    }

    void Reset() {
      std::lock_guard<std::mutex> lockGuard(m_mutex);
      m_front = 0;
      m_rear = 0;
      m_frontSize = 0;
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
      return (m_rear == m_front) && (m_frontSize == 0);
    }

    bool full() const {
      return (m_rear == m_front) && (m_frontSize == m_size);
    }

    void push() {
      if (!full()) {
        forward(m_rear);
        ++m_frontSize;
        Log::d("RingQueue", "push front %d rear %d size %d", m_front, m_rear, m_frontSize);
      }
    }

    void pop() {
      if (!empty()) {
        forward(m_front);
        --m_frontSize;
        Log::d("RingQueue", "pop  front %d rear %d size %d", m_front, m_rear, m_frontSize);
      }
    }

    bool last() {
      if (!empty()) {
        while (m_frontSize != 1) {
          forward(m_front);
          --m_frontSize;
          Log::d("RingQueue", "pop0 front %d rear %d size %d", m_front, m_rear, m_frontSize);
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

    void forward(int &index) {
      index = (index + 1) % m_size;
    }
  protected:
    int m_size;
    int m_front;
    int m_rear;
    int m_frontSize;
    std::vector<T> m_data;
    std::mutex m_mutex;
  };
}