//
// Created by caolong on 2020/8/10.
//

#include <res/font/FontSquareModel.hpp>

using namespace clt;

FontSquareModel::FontSquareModel()
    : m_vertexVbo(0),
      m_ebo(0) {
}

bool FontSquareModel::Init() {
//  //顶点
//  GLfloat vertex[] = {
//    -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,// 0 左上角
//    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // 1 左下角
//    1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // 2 右下角
//    1.0f, 1.0f, 0.0f, 1.0f, 1.0f// 3 右上角
//  };
  static const int vertexCoordsPer = 3; // vertex xyz
  static const int texCoordsPer = 2; // texcoords st
  static const int stride = (vertexCoordsPer + texCoordsPer) * sizeof(GLfloat);
  static const int vertexBytes = vertexCoordsPer * sizeof(GLfloat);
  gles::CreateXbo(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW,
                  4 * stride, (void *) nullptr, m_vertexVbo);

  // 绘制索引
  short index[] = {
      0, 1, 2, // 左下三角形
      0, 2, 3  // 右上三角形
  };

  gles::CreateXbo(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW,
                  sizeof(index), (void *) index, m_ebo);

  return true;
}

void FontSquareModel::DeInit() {
  gles::DeleteXbo(m_vertexVbo);
  gles::DeleteXbo(m_ebo);
}

void FontSquareModel::Bind(GLint posLoc, GLint texLoc) const {
  static const int vertexCoordsPer = 3; // vertex xyz
  static const int texCoordsPer = 2; // texcoords st
  static const int stride = (vertexCoordsPer + texCoordsPer) * sizeof(GLfloat);
  static const int vertexBytes = vertexCoordsPer * sizeof(GLfloat);

  if (posLoc != -1) {
    gles::UploadVertexAttrib(m_vertexVbo,
                             posLoc,
                             vertexCoordsPer,
                             stride,
                             nullptr);
  }

  if (texLoc != -1) {
    gles::UploadVertexAttrib(m_vertexVbo,
                             texLoc,
                             texCoordsPer,
                             stride,
                             (void *) (vertexBytes));
  }
}

void FontSquareModel::UpdateVertex(const GLfloat *vertex, int vSize) const {
  if (vertex != nullptr) {
    gles::UpdateXboData(m_vertexVbo,
                        GL_ARRAY_BUFFER,
                        0,
                        vSize,
                        (void *) vertex);
  }
}

void FontSquareModel::Draw() const {
  const int indexesCount = 6;
  glBindBuffer(GL_ARRAY_BUFFER, m_vertexVbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  glDrawElements(GL_TRIANGLES, indexesCount, GL_UNSIGNED_SHORT, nullptr);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);
  glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
}
