//
// Created by caolong on 2021/3/12.
//

#pragma once

#include <softarch/ComType.hpp>
#include <softarch/VarType.hpp>

namespace clt {
    /**
 * 描述文本绘制信息
 */
    struct TextInfo {
        TextInfo() : TextInfo("") {}

        TextInfo(const std::string &_text,
                 const Viewport &_displayViewport,
                 const Viewport &_realViewport)
                : text(_text),
                  displayViewport(_displayViewport),
                  realViewport(_realViewport),
                  position{0.0f, 0.0f},
                  color(WhiteColor),
                  bgColor(TransColor),
                  scale(1.0f),
                  shadowColor(BlackColor),
                  shadowDelta{1.5f, -1.5f},
                  shadow(true),
                  baseAdjust(true),
                  fillBg(false),
                  rotate(0.0f) {
        }

        TextInfo(const std::string &_text)
                : TextInfo(_text, {}, {}) {}

        bool Empty() {
            return text.empty();
        }

        /**
         * 文本
         */
        std::string text;

        /**
         * 文本显示区域
         */
        Viewport displayViewport;

        /**
         * 如果要显示到实际的帧上
         */
        Viewport realViewport;
        /**
         * 起始位置，是displayViewport大小的窗口坐标系
         */
        Float2 position;

        /**
         * 文本颜色
         */
        Float4 color;

        Float4 bgColor;

        /**
         * 字体缩放
         */
        float scale;

        /**
         * 阴影颜色
         */
        Float4 shadowColor;

        /**
         * 显示偏移，用于阴影效果
         */
        Float2 shadowDelta;

        /**
         * 是否显示文本阴影
         */
        bool shadow;

        /**
         * 是否调整基线位置，字体基线原点是否向下调整一个字符容器大小
         */
        bool baseAdjust;

        bool fillBg;

        // 绕起点逆时针旋转角度
        float rotate;
    };
}
