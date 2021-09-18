//
// Created by caolong on 2020/6/13.
//

#pragma once

// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <softarch/VarType.hpp>

namespace clt {

#undef PI
#define PI 3.141592653589793238462643383279502884L

    struct Math {
        /**
         * 计算 2D 坐标系下矢量与 X 轴的夹角
         * @param f
         * @param s
         * @return 返回度数
         */
        static float CoordinatesDegree(const Float2 &f, const Float2 &s) {
            double deltaX = s.x - f.x;
            double deltaY = s.y - f.y;
            return glm::degrees(std::atan2(deltaY, deltaX));
        }
    };
}