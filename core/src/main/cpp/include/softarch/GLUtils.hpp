//
// Created by caolong on 2019/7/1.
//

#pragma once

#if __ANDROID_API__ >= 24

#include <GLES3/gl32.h>

#elif __ANDROID_API__ >= 21
#include <GLES3/gl31.h>
#else
#include <GLES3/gl3.h>
#endif
//#include <GLES3/gl3ext.h>
#include <softarch/GL3Ext.hpp>


//#include <GLES2/gl2.h>
//#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <utils/Log.hpp>
#include <softarch/ComType.hpp>
#include <softarch/Task.hpp>

namespace clt {

  class gles {
  public:
    static inline void CheckGlError(const char *op) {
      for (GLint error = glGetError(); error != GL_NO_ERROR; error = glGetError()) {
        Log::e(Log::EGL_TAG, "after %s() glError (0x%x)", op, error);
      }
    }

    static inline bool hasGlError(const char *op) {
      for (GLint error = glGetError(); error != GL_NO_ERROR; error = glGetError()) {
        Log::e(Log::EGL_TAG, "after %s() glError (0x%x)", op, error);
        return true;
      }
      return false;
    }

    static inline void CheckEglError(const char *op) {
      Log::e(Log::EGL_TAG, "after %s() eglError (0x%x)", op, eglGetError());
    }

    static inline void PrintGLString(const char *name, GLenum s) {
      const char *v = (const char *) glGetString(s);
      Log::i(Log::EGL_TAG, "GL %s = %s", name, v);
    }

    /**
     * 编译shader程序
     * @param shaderType shader程序类别
     * @param pSource shader源码字符串
     * @return 返回shader程序ID
     */
    static GLuint LoadShader(GLenum shaderType, const char *pSource) {
      GLuint shader = glCreateShader(shaderType);
      if (shader) {
        // 1.读取shader源码
        glShaderSource(shader, 1, &pSource, nullptr);

        // 2.编译shader源码
        glCompileShader(shader);

        // 3.检查是否编译成功
        GLint compiled = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (compiled != GL_TRUE) {
          GLint infoLen = 0;
          glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
          if (!infoLen) {
            infoLen = INFO_LEN;
          }
          std::unique_ptr<char[]> buf(new char[infoLen]);
          if (buf) {
            glGetShaderInfoLog(shader, infoLen, nullptr, buf.get());
            Log::e(Log::RENDER_TAG, "Could not compile shader %d:\n%s", shaderType,
                   buf.get());
          }
          glDeleteShader(shader);
          shader = 0;
        }
      }
      return shader;
    }

    /**
     * 释放shader资源
     * @param id
     */
    static inline void UnLoadShader(GLuint &id) {
      if (id != 0) {
        glDeleteShader(id);
        id = 0;
      }
    }

    /**
     * 生成着色器程序
     * @param pVertexSource 顶点着色器源码
     * @param pFragmentSource 片段着色器源码
     * @return 着色器程序ID
     */
    static GLuint LoadProgram(const char *pVertexSource, const char *pFragmentSource) {
      // 1.生成着色器
      GLuint vertexShader = LoadShader(GL_VERTEX_SHADER, pVertexSource);
      if (!vertexShader) {
        return 0;
      }
      GLuint pixelShader = LoadShader(GL_FRAGMENT_SHADER, pFragmentSource);
      if (!pixelShader) {
        return 0;
      }

      // 2.生成着色器程序
      GLuint program = glCreateProgram();
      if (program) {
        // 2.1关联着色器
        glAttachShader(program, vertexShader);
        glAttachShader(program, pixelShader);
        CheckGlError("glAttachShader");

        // 2.2链接着色器
        glLinkProgram(program);

        // 2.3检查
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
          GLint bufLength = 0;
          glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
          if (bufLength) {
            std::unique_ptr<char[]> buf(new char[bufLength]);
            if (buf) {
              glGetProgramInfoLog(program, bufLength, nullptr, buf.get());
              Log::e(Log::RENDER_TAG, "Could not link program:\n%s", buf.get());
            }
          }
          glDeleteProgram(program);
          program = 0;
        }
      }

      UnLoadShader(vertexShader);
      UnLoadShader(pixelShader);

      return program;
    }

    static GLuint LoadProgram(const unsigned char *programBin) {
      if (!programBin)
        return 0;

      // 1.生成着色器程序
      GLuint program = glCreateProgram();
      if (program) {
//              glProgramBinary(program, )
      }
      return program;
    }

    static void UnLoadProgram(GLuint &id) {
      glDeleteProgram(id);
      id = 0;
    }

    static void UseProgram(GLuint id) {
      assert(id != 0);
      glUseProgram(id);
    }

    static void CreateXbo(GLenum target, int usage, int dataSize, void *data, GLuint &id) {
      glGenBuffers(1, &id);
      assert(id != 0);
      glBindBuffer(target, id);
      glBufferData(target, dataSize, data, usage);
    }

    static void UploadXboData(GLuint id, GLenum target, int usage, int dataSize, void *data) {
      assert(id != 0);
      glBindBuffer(target, id);
      glBufferData(target, dataSize, data, usage);
    }

    static void UpdateXboData(GLuint id, GLenum target, int offset, int dataSize, void *data) {
      assert(id != 0);
      glBindBuffer(target, id);
      glBufferSubData(target, offset, dataSize, data);
    }

    static void CreateXbo(GLuint &id) {
      glGenBuffers(1, &id);
      assert(id != 0);
    }

    static void DeleteXbo(GLuint &id) {
      glDeleteBuffers(1, &id);
      id = 0;
    }

    static void CreateVao(GLuint &id) {
      glGenVertexArrays(1, &id);
      assert(id != 0);
    }

    static void UseVao(GLuint &id, GLTask task) {
      glBindVertexArray(id);
      task();
      glBindVertexArray(0);
    }

    static void DeleteVao(GLuint &id) {
      glDeleteVertexArrays(1, &id);
      id = 0;
    }

    static void UploadVertexAttrib(GLuint id, GLuint loc, int size, int stride, void *pointer) {
      glBindBuffer(GL_ARRAY_BUFFER, id);
      glEnableVertexAttribArray(loc);
      glVertexAttribPointer(loc,
                            size,
                            GL_FLOAT,
                            GL_FALSE,
                            stride,
                            pointer);
    }

    static void Clear(int x, int y, int width, int height) {
      // ffbb33
      // glClearColor(1.0f, 0.7f, 0.2f, 1.0f);
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glViewport(x, y, width, height);
    }

    static void Clear(const Viewport &viewport) {
      Clear(viewport.x, viewport.y, viewport.width, viewport.height);
    }

    static void CreateFbo(GLuint &fbo) {
      glGenFramebuffers(1, &fbo);
      assert(fbo != 0);
    }

    static void DeleteFbo(GLuint &fbo) {
      glDeleteFramebuffers(1, &fbo);
      fbo = 0;
    }

    static void CreateTexture(GLuint &id) {
      glGenTextures(1, &id);
      assert(id != 0);
    }

    static void DeleteTexture(GLuint &id) {
      glDeleteTextures(1, &id);
      id = 0;
    }

    static void SetViewport(int x, int y, int width, int height) {
      glViewport(x, y, width, height);
    }

    static void SetViewport(const Viewport &v) {
      glViewport(v.x, v.y, v.width, v.height);
    }

    static void UseFbo(GLuint fboId, GLuint textureId, GLTask task) {
      const GLenum attachments[1]{GL_COLOR_ATTACHMENT0};

      glBindFramebuffer(GL_FRAMEBUFFER, fboId);
      glBindTexture(GL_TEXTURE_2D, textureId);
      glFramebufferTexture2D(GL_FRAMEBUFFER,
                             attachments[0],
                             GL_TEXTURE_2D,
                             textureId,
                             0);
      glDrawBuffers(1, attachments);

      task();
      glBindTexture(GL_TEXTURE_2D, 0);
      glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[0], GL_TEXTURE_2D, 0, 0);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    static void UseFbo2MRT(GLuint fboId, const GLuint textureIds[], GLTask task) {
      const GLenum attachments[2]{GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};

      glBindFramebuffer(GL_FRAMEBUFFER, fboId);

      for (int i = 0; i < 2; ++i) {
        glBindTexture(GL_TEXTURE_2D, textureIds[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               attachments[i],
                               GL_TEXTURE_2D,
                               textureIds[i],
                               0);
      }
      glDrawBuffers(2, attachments);

      task();

      glBindTexture(GL_TEXTURE_2D, 0);
      for (int i = 0; i < 2; ++i) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachments[i], GL_TEXTURE_2D, 0, 0);
      }
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    static void DrawElements(GLuint ebo, int indexesCount) {
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
      glDrawElements(GL_TRIANGLES, indexesCount, GL_UNSIGNED_SHORT, nullptr);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    static void UseArrayBuffer(GLuint vbo, GLTask &&task) {
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      std::forward<GLTask>(task)();
      glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    }

    /**
     * 设置EGL的功能
     */
    static void InitGLESFunc() {
      glEnable(GL_DEPTH_TEST);

      // 绘制字体
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glFrontFace(GL_CCW); //逆时针方向为前面
      glCullFace(GL_BACK); // 剔除背面
      glEnable(GL_CULL_FACE);
    }

    static void Flush() { glFlush(); }

    static void Finish() { glFinish(); }

  private:
    static constexpr int INFO_LEN = 0x1000;
  };

}