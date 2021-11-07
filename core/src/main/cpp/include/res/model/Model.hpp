//
// Created by caolong on 2021/3/15.
//

#pragma once

#include <utils/Log.hpp>

namespace clt {
  /*
   * 原点在左上角，需转化为 OpenGL 坐标系
   */
  struct PolygonObject {
    PolygonObject() = default;

    // _region 是图像坐标系下的
    PolygonObject(const Float2 &_canvasSize,
                  const std::vector<Float2> &_region,
                  const Float4 &_color,
                  const std::string &_tag,
                  bool _fill = false)
        : canvasSize(_canvasSize), region(_region), color(_color), tag(_tag), fill(_fill) {
      for (auto &point : region) {
        point.y = canvasSize.h - point.y;
      }
    }

    PolygonObject(const Float2 &_canvasSize,
                  const Float4 &_region, // 图像坐标
                  const Float4 &_color,
                  const std::string &_tag,
                  bool _fill = false)
        : canvasSize(_canvasSize), color(_color), tag(_tag), fill(_fill) {
      float x = _region.x;
      float y = canvasSize.h - _region.y;
      float w = _region.z; // width
      float h = _region.w; // height
      region.emplace_back(x, y);
      region.emplace_back(x, y - h);
      region.emplace_back(x + w, y - h);
      region.emplace_back(x + w, y);
    }

    Float2 canvasSize;
    std::vector<Float2> region;
    Float4 color;
    std::string tag;
    bool fill;
  };

  struct BaseModelObject {
    enum class Type {
      CUBE,
      PYRAMID,
    };

    BaseModelObject(Type _type,
                    const glm::mat4 &_projection, const glm::mat4 &_view, const glm::mat4 &_model,
                    const Color &_color)
        : type(_type),
          projection(_projection),
          view(_view),
          model(_model),
          color(_color) {}

    Type type;
    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 model;
    Color color;
  };
}
