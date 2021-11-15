//
// Created by caolong on 2021/11/16.
//

#include <softarch/Buffer.hpp>

using namespace clt;

long Buffer::s_count = 0;

std::mutex Buffer::s_mutex;