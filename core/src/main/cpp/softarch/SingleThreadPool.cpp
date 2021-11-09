//
// Created by caolong on 2020/8/13.
//

#include <softarch/SingleThreadPool.hpp>

using namespace clt;

SingleThreadPool::SingleThreadPool(const std::string &name)
  : ThreadPool(name, 1) {

}

SingleThreadPool::SingleThreadPool(const std::string &name, bool syncStop)
  : ThreadPool(name, syncStop, 1) {

}

std::thread::id SingleThreadPool::Id() {
  return ThreadPool::Id();
}