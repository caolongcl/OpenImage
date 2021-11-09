//
// Created by caolong on 2020/8/1.
//

#pragma once

#include <utils/Log.hpp>
#include <softarch/Buffer.hpp>
#include <softarch/IComFunc.hpp>

namespace clt {
  /**
   * 工具类
   */
  struct Utils {
  ClassDeclare(Utils)

    /**
     * 归整到最近的偶数
     * @param n
     * @return
     */
    static int RoundToEven(float n) {
      int f = std::floor(n);
      int c = std::ceil(n);

      if (f == c) {
        n -= 0.4;
        f = std::floor(n);
        c = std::ceil(n);
      }

      return f % 2 == 0 ? f : c;
    }

    /**
     * 限制输入的范围
     * @param in
     * @param min
     * @param max
     * @return
     */
    static float Clamp(float in, float min, float max) {
      if (in > max)
        return max;

      if (in < min)
        return min;

      return in;
    }

    /**
     * 获取当前的时间 毫秒
     * @return
     */
    static long CurTimeMilli() {
      struct timeval tv{};
      gettimeofday(&tv, nullptr);
      return tv.tv_sec * 1000 + tv.tv_usec / 1000;
    }

    /**
     * 将输入按16进制打印
     * @param input
     * @param datasize
     * @return
     */
    static std::string ToHexString(unsigned char *input, int datasize) {
      if (input == nullptr)
        return "";

      std::stringstream ss;
      ss << std::setbase(16) << std::setfill('0');
      for (int i = 0; i < datasize; i++)
        ss << std::setw(2) << (unsigned int) input[i];
      return ss.str();
    }

    static std::string &Trim(std::string &s) {
      if (s.empty()) {
        return s;
      }

      s.erase(0, s.find_first_not_of(' '));
      s.erase(s.find_last_not_of(' ') + 1);
      return s;
    }

    static void PrintPixel(Buffer::DataPtrType data, std::size_t size) {
      std::string tmp = Utils::ToHexString(data, size / 20);
      Log::d(target, "printPixel %s", tmp.c_str());

      std::string tmp1 = Utils::ToHexString(data + size / 20, size / 20);
      Log::d(target, "printPixel1 %s", tmp1.c_str());

      std::string tmp2 = Utils::ToHexString(data + size / 10, size / 20);
      Log::d(target, "printPixel2 %s", tmp2.c_str());
    }

    static std::string GetBoolStr(bool b) {
      return b ? "true" : "false";
    }
  };

}