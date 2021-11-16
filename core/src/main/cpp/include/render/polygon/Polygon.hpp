//
// Created by caolong on 2020/8/12.
//

#pragma once

#include <softarch/std.hpp>
#include <utils/Utils.hpp>
#include <render/polygon/PolygonModel.hpp>
#include <render/polygon/PolygonDrawer.hpp>
#include <res/model/Model.hpp>
#include <res/TextInfo.hpp>

namespace clt {

  /**
   * 绘制多边形，如人脸检测框，AR Marker 框等
   */
  class Polygon final : public PolygonDrawer {
  ClassDeclare(Polygon)
  VarDeclare(color)
  VarDeclare(fill)
  VarDeclare(to_points)

  public:
    Polygon();

    ~Polygon() = default;

    bool Init() override;

    void DeInit() override;

    void UpdatePolygon(const PolygonObject &object, const Viewport &viewport);

    void Render();

  private:

    void loadShader() override;

    void updateValue() override;

    void registerVar() override;

    void draw() override;

  private:
    PolygonModel m_model;
    TextInfo m_tag;
    glm::mat4 m_projection;
    Float4 m_color;
    int m_lineWidth;
    float m_pointSize;
    int m_vertexCount;
    bool m_fill;
    bool m_toPoints;

    GLint m_uniformLocProjection{-1};
    GLint m_uniformLocModel{-1};
    GLint m_uniformLocColor{-1};
    GLint m_uniformLocPointSize{-1};
  };
}
