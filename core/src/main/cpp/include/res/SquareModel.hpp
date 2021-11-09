//
// Created by caolong on 2020/8/6.
//

#pragma once

#include <softarch/IComFunc.hpp>
#include <softarch/GLUtils.hpp>

namespace clt {

  /**
   * 可绘制长方形
   */
  class SquareModel : public IComFunc<> {
  ClassDeclare(SquareModel)
  public:
    SquareModel();

    ~SquareModel() = default;

    bool Init() override;

    void DeInit() override;

    void Bind(GLint posLoc, GLint texLoc);

    void UpdatePositionCoordinates(const GLfloat *vertex, int vSize) const;

    void UpdateTextureCoordinates(const GLfloat *tex, int tSize) const;

    void Draw();

  private:
    GLuint m_vao;
    GLuint m_vertexVbo;
    GLuint m_texVbo;
    GLuint m_ebo;
  };

}


