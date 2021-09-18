//
// Created by caolong on 2020/6/7.
//

#pragma once

namespace clt {

    /**
     * OpenGL 坐标系下的视图区域
     */
    struct Viewport {
        Viewport() : x(0), y(0), width(0), height(0) {}
        Viewport(int _x, int _y, int _width, int _height) : x(_x), y(_y), width(_width), height(_height) {}

        int x;
        int y;
        int width;
        int height;
    };

    /**
     * 预览帧大小
     */
    struct OPreviewSize {
        OPreviewSize(int _width, int _height)
                : width(_width), height(_height) {}

        int width;
        int height;
    };

    struct OTargetSize {
        OTargetSize(int _width, int _height)
                : width(_width), height(_height) {}

        int width;
        int height;
    };

}