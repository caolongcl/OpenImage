//
// Created by caolong on 2020/8/12.
//

#pragma once

#include <softarch/IComFunc.hpp>
#include <softarch/GLUtils.hpp>
#include <softarch/VarType.hpp>

namespace clt {

  struct PolygonModelUpdater {
    PolygonModelUpdater() = default;

    virtual ~PolygonModelUpdater() = default;

    /**
     * 赋予修改顶点的能力
     * @param vertex
     * @param vSize
     */
    virtual void UpdateVertex(const std::vector<Float2> &vertex) = 0;
  };

  /**
   * 可绘制几何图形
   * 用于绘制多边形
   */
  class PolygonModel : public IComFunc<>,
                       public PolygonModelUpdater {
  public:
    PolygonModel();

    virtual ~PolygonModel() = default;

    bool Init() override;

    void DeInit() override;

    void Bind(GLint posLoc) const;

    void UpdateVertex(const std::vector<Float2> &vertex) override;

    void Use(GLTask &&task) const;

  private:
    GLuint m_vertexVbo;
    std::shared_ptr<GLfloat> m_vertexs;

  public:
    // 最多支持的定点数
    static const int s_vertexMax = 300;
    // x y
    static const int s_vertexCoordsPer = 2;
  };

}
