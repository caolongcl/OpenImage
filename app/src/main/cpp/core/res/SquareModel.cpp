//
// Created by caolong on 2020/8/6.
//

#include <res/SquareModel.hpp>

using namespace clt;

SquareModel::SquareModel()
    : m_vao(0),
      m_vertexVbo(0),
      m_texVbo(0),
      m_ebo(0) {
}

bool SquareModel::Init() {
  // 顶点
  GLfloat vertex[] = {
      -1.0f, 1.0f, 0.0f, // 0 左上角
      -1.0f, -1.0f, 0.0f, // 1 左下角
      1.0f, -1.0f, 0.0f, // 2 右下角
      1.0f, 1.0f, 0.0f, // 3 右上角
  };

  gles::CreateXbo(GL_ARRAY_BUFFER, GL_STATIC_DRAW,
                  sizeof(vertex), (void *) vertex, m_vertexVbo);

  // 绘制索引
  short index[] = {
      0, 1, 2, // 左下三角形
      0, 2, 3  // 右上三角形
  };

  gles::CreateXbo(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW,
                  sizeof(index), (void *) index, m_ebo);

  // 纹理
  GLfloat tex[] = {
      0.0f, 1.0f,  // 0 左上角
      0.0f, 0.0f,  // 1 左下角
      1.0f, 0.0f, // 2 右下角
      1.0f, 1.0f // 3 右上角
  };

  gles::CreateXbo(GL_ARRAY_BUFFER, GL_STATIC_DRAW,
                  sizeof(tex), (void *) tex, m_texVbo);

  // VAO
  gles::CreateVao(m_vao);
  gles::UseVao(m_vao, [this]() {
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexVbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_texVbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  });

  return true;
}

void SquareModel::DeInit() {
  gles::DeleteVao(m_vao);
  gles::DeleteXbo(m_vertexVbo);
  gles::DeleteXbo(m_texVbo);
  gles::DeleteXbo(m_ebo);
}

void SquareModel::Bind(GLint posLoc, GLint texLoc) {
  gles::UseVao(m_vao, [this, posLoc, texLoc]() {
    if (posLoc != -1) {
      const int vertexCoordsPer = 3;
      gles::UploadVertexAttrib(m_vertexVbo,
                               posLoc,
                               vertexCoordsPer,
                               vertexCoordsPer * sizeof(GLfloat),
                               nullptr);
    }

    if (texLoc != -1) {
      const int texCoordsPer = 2;
      gles::UploadVertexAttrib(m_texVbo,
                               texLoc,
                               texCoordsPer,
                               texCoordsPer * sizeof(GLfloat),
                               nullptr);
    }
  });
}

void SquareModel::UpdatePositionCoordinates(const GLfloat *vertex, int vSize) const {
  if (vertex != nullptr) {
    gles::UpdateXboData(m_vertexVbo,
                        GL_ARRAY_BUFFER,
                        0,
                        vSize,
                        (void *) vertex);
  }
}

void SquareModel::UpdateTextureCoordinates(const GLfloat *tex, int tSize) const {
  if (tex != nullptr) {
    gles::UpdateXboData(m_texVbo,
                        GL_ARRAY_BUFFER,
                        0,
                        tSize,
                        (void *) tex);
  }
}

void SquareModel::Draw() {
  gles::UseVao(m_vao, []() {
    const int indexesCount = 6;
    glDrawElements(GL_TRIANGLES, indexesCount, GL_UNSIGNED_SHORT, nullptr);
  });
}