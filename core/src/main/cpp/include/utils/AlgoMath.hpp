//
// Created by caolong on 2021/11/7.
//

#pragma once

#include <softarch/std.hpp>

namespace clt {
  struct AlgoMath {
    static inline float FastExp(float x) {
      union {
        uint32_t i;
        float f;
      } v{};
      v.i = (1 << 23) * (1.4426950409 * x + 126.93490512f);
      return v.f;
    }

    static inline float Sigmoid(float x) {
      return 1.0f / (1.0f + FastExp(-x));
    }

    template<typename _Tp>
    static int ActivationFunctionSoftmax(const _Tp *src, _Tp *dst, int length) {
      const _Tp alpha = *std::max_element(src, src + length);
      _Tp denominator{0};

      for (int i = 0; i < length; ++i) {
        dst[i] = FastExp(src[i] - alpha);
        denominator += dst[i];
      }

      for (int i = 0; i < length; ++i) {
        dst[i] /= denominator;
      }

      return 0;
    }
  };
}
