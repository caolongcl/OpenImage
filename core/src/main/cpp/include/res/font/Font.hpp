//
// Created by caolong on 2020/8/6.
//

#pragma once

#include <res/font/FontSquareModel.hpp>
#include <res/IRenderText.hpp>
#include <softarch/VarType.hpp>
#include <softarch/IComFunc.hpp>
#include <softarch/GLUtils.hpp>
#include <render/texture/Texture.hpp>
#include <res/Shader.hpp>

#include "freetype2/ft2build.h"
#include FT_FREETYPE_H

namespace clt {

  class CharsGroupGlyph;

  class FontRender;

  class Font : public IComFunc<>,
               public IRenderText {
  ClassDeclare(Font)
    using Face = FT_Face;

  public:
    Font(std::string path);

    virtual ~Font() = default;

    bool Init() override;

    void DeInit() override;

    /**
     * 加载字体
     */
    virtual void Load();

    virtual bool Loaded();

    /**
     *
     * @param info
     */
    void RenderText(const TextInfo &info) override;

  private:
    /**
     * 加载freetype字体
     * @return
     */
    bool loadFont();

    /**
     * 根据字体字形，统计各个可用字符的字形信息
     * @param face
     * @return
     */
    bool loadChars(Face &face);

    /**
     * 绘制文本
     * @param text
     * @param viewport
     * @param pos
     * @param color
     * @param scale
     * @param shadow
     */
    virtual void renderText(const TextInfo &info);

    void printGlyphBitmap(const Texture::BufferPtr &buffer, int bytes);

  private:
    std::atomic<bool> m_loaded;
    /**
     * 加载字库生成字形需要的大小参数
     */
    Integer2 m_charGlyphSizeHint;

    std::shared_ptr<CharsGroupGlyph> m_glyph;
    std::shared_ptr<FontRender> m_fontRender;

  protected:
    /**
     * 字库文件
     */
    std::string m_path;

  private:
    /**
     * 默认每行显示多少个字符
     */
    const int cTextCountPerWidth;
  };

  /**
   * 默认字体
   */
  class ArialFont : public Font {
  public:
    ArialFont(std::string path) : Font(path + "/arial.ttf") {}

    static const char *sName;
  };

}