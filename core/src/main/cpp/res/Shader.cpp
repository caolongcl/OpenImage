//
// Created by caolong on 2020/6/10.
//

#include <res/YamlParse.hpp>
#include <res/Shader.hpp>
#include <softarch/GLUtils.hpp>

using namespace clt;

Shader::Shader(const unsigned char *program)
  : m_programId(0) {
  assert(program != nullptr);
  Log::w(target, "don't support program binary");
}

Shader::Shader(const std::string &vertexSrc, const std::string &fragmentSrc)
  : m_programId(0) {
  genShaderProgram(vertexSrc, fragmentSrc, "");
}

Shader::Shader(const std::string &vertexSrc,
               const std::string &fragmentSrc,
               const std::string &computeSrc)
  : m_programId(0) {
  genShaderProgram(vertexSrc, fragmentSrc, computeSrc);
}

Shader::~Shader() {
  delShaderProgram();
}

void Shader::Use() const {
  gles::UseProgram(m_programId);
}

void Shader::SetFloat1(GLint location, const Float1 &v) {
  glUniform1f(location, v.x);
}

void Shader::SetFloat2(GLint location, const Float2 &v) {
  glUniform2f(location, v.x, v.y);
}

void Shader::SetFloat3(GLint location, const Float3 &v) {
  glUniform3f(location, v.x, v.y, v.z);
}

void Shader::SetFloat4(GLint location, const Float4 &v) {
  glUniform4f(location, v.x, v.y, v.z, v.w);
}

void Shader::SetInteger1(GLint location, const Integer1 &v) {
  glUniform1i(location, v.x);
}

void Shader::SetInteger2(GLint location, const Integer2 &v) {
  glUniform2i(location, v.x, v.y);
}

void Shader::SetInteger3(GLint location, const Integer3 &v) {
  glUniform3i(location, v.x, v.y, v.z);
}

void Shader::SetInteger4(GLint location, const Integer4 &v) {
  glUniform4i(location, v.x, v.y, v.z, v.w);
}

void Shader::SetMatrix2(GLint location, const glm::mat2 &v) {
  glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr<GLfloat>(v));
}

void Shader::SetMatrix3(GLint location, const glm::mat3 &v) {
  glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr<GLfloat>(v));
}

void Shader::SetMatrix4(GLint location, const glm::mat4 &v) {
  glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr<GLfloat>(v));
}

bool Shader::genShaderProgram(const std::string &vertexSrc,
                              const std::string &fragmentSrc,
                              const std::string &computeSrc) {
  m_programId = gles::LoadProgram(vertexSrc.c_str(), fragmentSrc.c_str());
  assert(m_programId != 0);

  m_attrLocPosition = glGetAttribLocation(m_programId, "aPosition");
  m_attrLocTexCoords = glGetAttribLocation(m_programId, "aTexCoords");
  m_attrLocColors = glGetAttribLocation(m_programId, "aColor");
  m_uniformLocSampler = glGetUniformLocation(m_programId, "uTexSampler");

  Log::v(target,
         "shader loc: pos %d tex %d color %d sampler %d",
         m_attrLocPosition,
         m_attrLocTexCoords,
         m_attrLocColors,
         m_uniformLocSampler);

  return true;
}

void Shader::delShaderProgram() {
  gles::UnLoadProgram(m_programId);
}


