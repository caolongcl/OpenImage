//
// Created by caolong on 2020/8/6.
//

#include <utils/Utils.hpp>
#include <softarch/ComType.hpp>
#include <res/font/Font.hpp>
#include <render/texture/GrayscaleTexture.hpp>
#include <res/font/FontGlyph.hpp>
#include <res/font/FontRender.hpp>

using namespace clt;

Font::Font(std::string path)
  : m_loaded(false),
    m_path(std::move(path)),
    m_charGlyphSizeHint(0, 48),
    m_fontRender(new FontRender()),
    cTextCountPerWidth(32) {

}

bool Font::Init() {
  m_glyph = std::make_shared<CharsGroupGlyph>();
  m_fontRender->Init(m_glyph);
  return true;
}

void Font::DeInit() {
  m_fontRender->DeInit();
}

void Font::Load() {
  Log::d(target, "Font::Load %s", m_path.c_str());
  m_loaded = loadFont();
}

bool Font::Loaded() {
  return m_loaded;
}

void Font::RenderText(const TextInfo &info) {
  renderText(info);
}

bool Font::loadFont() {
  std::fstream f(m_path.c_str());
  if (!f.good()) {
    Log::e(target, "Could not find font %s", m_path.c_str());
    return false;
  }

  FT_Library ft;
  if (FT_Init_FreeType(&ft)) {
    Log::e(target, "Could not init FreeType Library");
    FT_Done_FreeType(ft);
    return false;
  }

  FT_Face face;
  if (FT_New_Face(ft, m_path.c_str(), 0, &face)) {
    Log::e(target, "Failed to load font");
    FT_Done_Face(face);
    return false;
  }

  // 定义字体大小
  FT_Set_Pixel_Sizes(face, m_charGlyphSizeHint.w, m_charGlyphSizeHint.h);

  // 加载ASCII字符
  if (!loadChars(face)) {
    Log::e(target, "Failed to load font");
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    return false;
  }

  FT_Done_Face(face);
  FT_Done_FreeType(ft);
  return true;
}

bool Font::loadChars(Face &face) {
  static int index = 0;

  for (char c = 0; c < 128; c++) {
    // 过滤可打印字符
    if (!std::isprint(c))
      continue;

    CharGlyph glyph;
    glyph.c = c;
    glyph.index = index++;

    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
      Log::e(target, "Failed to load Glyph %c", (char) c);
      glyph.valid = false;
    } else {
      glyph.valid = true;
      glyph.size.w = face->glyph->bitmap.width;
      glyph.size.h = face->glyph->bitmap.rows;
      glyph.bearing.x = face->glyph->bitmap_left;
      glyph.bearing.y = face->glyph->bitmap_top;
      glyph.advance.x = face->glyph->advance.x;
      glyph.advance.y = face->glyph->advance.y;
      glyph.pitch = face->glyph->bitmap.pitch;

      // 存储生成的字符Bitmap
      int bytes = glyph.pitch * glyph.size.h;
      if (bytes > 0) {
        Texture::BufferPtr
          buffer(new Texture::Buffer[bytes]{0},
                 [](const Texture::Buffer *const b) { delete[] b; });
        for (int i = 0; i < bytes; ++i) {
          buffer.get()[i] = face->glyph->bitmap.buffer[i];;
        }
        glyph.buffer = buffer;

//        printGlyphBitmap(buffer, bytes);

        Log::n(target,
               "Font::loadChars %d %c size(%d %d) bearing(%d %d) advance(%d %d) pitch %d bytes %d %p",
               glyph.index, glyph.c,
               glyph.size.w, glyph.size.h,
               glyph.bearing.x, glyph.bearing.y,
               glyph.advance.x, glyph.advance.y,
               glyph.pitch, bytes, glyph.buffer.get());
      }
    }

    m_glyph->AddChar(glyph);
  }
  index = 0;

  // 将碎片化的字体bitmap合并为一个大Bitmap
  return m_glyph->Merge(m_fontRender->GetTexture());
}

/**
 * 绘制文本
 * @param text
 * @param pos
 * @param color
 * @param scale
 * @param shadow
 */
void Font::renderText(const TextInfo &info) {
  // position 都是屏幕窗口坐标系的，需要换算到实际绘制字体的坐标系位置
  Float2 position{
    info.position.x * ((float) info.previewViewport.width / (float) info.displayViewport.width),
    info.position.y * ((float) info.previewViewport.height / (float) info.displayViewport.height)};

  float scale = info.scale * ((float) info.previewViewport.width / (float) cTextCountPerWidth) /
                m_glyph->GetCharRectSize().w;

  // 高度调节一个字符容器的高度
  if (info.baseAdjust) {
    position.y += m_glyph->GetCharRectSize().h * scale;
  }

  // 换到 OpenGL 坐标系
  position.x = position.x + (float) info.previewViewport.x;
  position.y = -position.y + (float) info.previewViewport.y + (float) info.previewViewport.height;

//    Log::e(target, "info.rotate %f", info.rotate);

  // 变换顺序和定义的相反
  glm::mat4 model(1.0f);
  model = glm::translate(model, glm::vec3(position.x, position.y, 0.0f));
  model = glm::rotate(model, glm::radians(info.rotate), glm::vec3(0.0f, 0.0f, 1.0f));
  model = glm::translate(model, glm::vec3(-position.x, -position.y, 0.0f));

  glm::mat4 projection = glm::ortho((float) info.previewViewport.x,
                                    (float) info.previewViewport.x +
                                    (float) info.previewViewport.width,
                                    (float) info.previewViewport.y,
                                    (float) info.previewViewport.y +
                                    (float) info.previewViewport.height);
  // 绘制字体背景
  if (info.fillBg && info.bgColor.a != 0.f) {
    m_fontRender->RenderBackground(info.text, position, info.bgColor, scale, model, projection);
  }

  // 绘制字体阴影
  if (info.shadow) {
    m_fontRender->RenderText(info.text,
                             {position.x + info.shadowDelta.x, position.y + info.shadowDelta.y},
                             info.shadowColor, scale, model, projection);
  }
  // 绘制文本
  m_fontRender->RenderText(info.text, position, info.color, scale, model, projection);
}

void Font::printGlyphBitmap(const Texture::BufferPtr &buffer, int bytes) {
  if (buffer == nullptr)
    return;

  std::stringstream ss;
  std::string tmp = Utils::ToHexString(buffer.get(), bytes / 3);
  Log::d(target, "Font::loadChars %s", tmp.c_str());

  tmp = Utils::ToHexString(buffer.get() + bytes / 3, bytes / 3);
  Log::d(target, "Font::loadChars %s", tmp.c_str());

  tmp =
    Utils::ToHexString(buffer.get() + bytes / 3 + bytes / 3,
                       bytes - (bytes / 3 + bytes / 3));
  Log::d(target, "Font::loadChars %s", tmp.c_str());
}

const char *ArialFont::sName = "arial";