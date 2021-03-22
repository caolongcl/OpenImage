//
// Created by caolong on 2020/8/10.
//

#pragma once

#include <softarch/GLUtils.hpp>

namespace clt {

    class Constants {
    public:
        static const int ROTATION_0 = 0;
        static const int ROTATION_90 = 90;
        static const int ROTATION_180 = 180;
        static const int ROTATION_270 = 270;

        static const int RATIO_4_3 = 0;
        static const int RATIO_16_9 = 1;
        static const int RATIO_OTHER = 2;

        static const int MAIN_TYPE = 0;
        static const int CAPTURE_TYPE = 1;
        static const int RECORD_TYPE = 2;

        // Process 模块使用 GL_TEXTURE3
        constexpr static const GLenum PROCESS_TEXTURE_UNIT = GL_TEXTURE3;
        constexpr static const GLint PROCESS_TEXTURE_OFFSET = 3;

        static bool ValidRatio(int ratio) {
            return ratio >= Constants::RATIO_4_3 && ratio <= Constants::RATIO_OTHER;
        }

        static bool ValidRotation(int rotation) {
            return rotation == Constants::ROTATION_0 || rotation == Constants::ROTATION_90
                   || rotation == Constants::ROTATION_180 || rotation == Constants::ROTATION_270;
        }

        static bool ValidSurfaceType(int type) {
            return type == MAIN_TYPE || type == CAPTURE_TYPE || type == RECORD_TYPE;
        }

        static std::string SurfaceTypeName(int type) {
            if (type == MAIN_TYPE) return "main";
            else if (type == CAPTURE_TYPE) return "capture";
            else if (type == RECORD_TYPE) return "record";
            else return "";
        }
    };

};
