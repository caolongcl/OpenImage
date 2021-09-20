//
// Created by caolong on 2020/6/10.
//

#pragma once

#include <softarch/IComFunc.hpp>
#include <softarch/GLUtils.hpp>
#include <softarch/Math.hpp>
#include <softarch/VarType.hpp>

namespace clt {

  class Shader : public IComFunc<> {
  public:
    Shader() = default;

    explicit Shader(const unsigned char *program);

    explicit Shader(const std::string &vertexSrc, const std::string &fragmentSrc);

    explicit Shader(const std::string &vertexSrc,
                    const std::string &fragmentSrc,
                    const std::string &computeSrc);

    virtual ~Shader();

    bool Init() override { return true; }

    void DeInit() override {}

    GLuint ProgramId() const { return m_programId; }

    GLint PositionAttributeLocation() const { return m_attrLocPosition; }

    GLint TexCoordinateAttributeLocation() const { return m_attrLocTexCoords; }

    GLint SamplerUniformLocation() const { return m_uniformLocSampler; }

    GLint ColorAttributeLocation() const { return m_attrLocColors; }

    void Use() const;

    static void SetFloat1(GLint location, const Float1 &v);

    static void SetFloat2(GLint location, const Float2 &v);

    static void SetFloat3(GLint location, const Float3 &v);

    static void SetFloat4(GLint location, const Float4 &v);

    static void SetInteger1(GLint location, const Integer1 &v);

    static void SetInteger2(GLint location, const Integer2 &v);

    static void SetInteger3(GLint location, const Integer3 &v);

    static void SetInteger4(GLint location, const Integer4 &v);

    static void SetMatrix2(GLint location, const glm::mat2 &v);

    static void SetMatrix3(GLint location, const glm::mat3 &v);

    static void SetMatrix4(GLint location, const glm::mat4 &v);

  private:
    bool genShaderProgram(const std::string &vertexSrc,
                          const std::string &fragmentSrc,
                          const std::string &computeSrc);

    void delShaderProgram();

  protected:
    GLuint m_programId{0};

    GLint m_attrLocPosition{-1};
    GLint m_attrLocTexCoords{-1};
    GLint m_attrLocColors{-1};
    GLint m_uniformLocSampler{-1};
  };

}