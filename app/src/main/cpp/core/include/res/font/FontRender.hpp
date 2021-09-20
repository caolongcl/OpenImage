//
// Created by caolong on 2020/8/14.
//

#pragma once

#include <softarch/IComFunc.hpp>
#include <res/font/FontGlyph.hpp>
#include <res/Shader.hpp>
#include <res/font/FontSquareModel.hpp>

namespace clt {
  /**
   * 渲染字符
   */
  class FontRender final
      : public IComFunc<std::shared_ptr<CharsGroupGlyph>> {
  public:
    FontRender() = default;

    virtual ~FontRender() = default;

    bool Init(std::shared_ptr<CharsGroupGlyph>) override;

    void DeInit() override;

    /**
     * 获取字形纹理
     * @return
     */
    std::shared_ptr<Texture> &GetTexture();

    /**
     * 渲染文本
     * @param text
     * @param pos 屏幕坐标系
     * @param color
     * @param scale
     * @param projection
     */
    void RenderText(const std::string &text,
                    const Float2 &pos,
                    const Float4 &color,
                    float scale,
                    const glm::mat4 &model,
                    const glm::mat4 &projection);

    void RenderBackground(const std::string &text,
                          const Float2 &pos,
                          const Float4 &bgColor,
                          float scale,
                          const glm::mat4 &model,
                          const glm::mat4 &projection);

  private:
    /**
     * 存储字符字形bitmap
     */
    std::shared_ptr<Texture> m_charsTexture;

    /**
     * 存储字形信息
     */
    std::shared_ptr<CharsGroupGlyph> m_glyph;

    /**
     * 渲染字形shader
     */
    std::shared_ptr<Shader> m_shader;

    /**
     * 绘制字形
     */
    std::shared_ptr<FontSquareModel> m_square;

    GLint m_uniformTextColorLoc{-1};
    GLint m_uniformPrjLoc{-1};
    GLint m_uniformModelLoc{-1};

    std::shared_ptr<Shader> m_bgShader;
    GLint m_uniformBgLocProjection{-1};
    GLint m_uniformBgLocModel{-1};
    GLint m_uniformBgLocColor{-1};
  };

}
