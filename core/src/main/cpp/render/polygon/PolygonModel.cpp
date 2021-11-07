//
// Created by caolong on 2020/8/12.
//

#include <render/polygon/PolygonModel.hpp>

using namespace clt;

PolygonModel::PolygonModel()
    : m_vertexVbo(0) {
}

bool PolygonModel::Init() {
  gles::CreateXbo(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW,
                  s_vertexMax * s_vertexCoordsPer * sizeof(GLfloat),
                  (void *) nullptr, m_vertexVbo);

  std::shared_ptr<GLfloat> vertex(new GLfloat[s_vertexMax * s_vertexCoordsPer],
                                  [](const GLfloat *const b) { delete[] b; });
  m_vertexs = std::move(vertex);

  return true;
}

void PolygonModel::DeInit() {
  gles::DeleteXbo(m_vertexVbo);
  m_vertexs = nullptr;
}

void PolygonModel::Bind(GLint posLoc) const {
  if (posLoc != -1) {
    gles::UploadVertexAttrib(m_vertexVbo, posLoc,
                             s_vertexCoordsPer,
                             s_vertexCoordsPer * sizeof(GLfloat),
                             nullptr);
  }
}

void PolygonModel::UpdateVertex(const std::vector<Float2> &vertex) {
  if (!vertex.empty() && vertex.size() <= s_vertexMax) {
    for (int i = 0; i < vertex.size(); ++i) {
      m_vertexs.get()[i * 2] = vertex[i].x;
      m_vertexs.get()[i * 2 + 1] = vertex[i].y;
    }

    gles::UpdateXboData(m_vertexVbo, GL_ARRAY_BUFFER, 0,
                        vertex.size() * s_vertexCoordsPer * sizeof(GLfloat),
                        (void *) m_vertexs.get());
  }
}

void PolygonModel::Use(GLTask &&task) const {
  gles::UseArrayBuffer(m_vertexVbo, std::forward<GLTask>(task));
}