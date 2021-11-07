//
// Created by caolong on 2020/8/10.
//

#pragma once

#include <softarch/IComFunc.hpp>
#include <softarch/GLUtils.hpp>

namespace clt {

  /**
   * 字体绘制
   */
  class FontSquareModel : public IComFunc<> {
  public:
    FontSquareModel();

    ~FontSquareModel() = default;

    bool Init() override;

    void DeInit() override;

    void Bind(GLint posLoc, GLint texLoc) const;

    void UpdateVertex(const GLfloat *vertex, int vSize) const;

    void Draw() const;

  private:
    GLuint m_vertexVbo;
    GLuint m_ebo;
  };

}
