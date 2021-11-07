//
// Created by caolong on 2020/8/14.
//

#include <res/ResManager.hpp>
#include <res/font/FontRender.hpp>
#include <render/texture/GrayscaleTexture.hpp>

using namespace clt;

bool FontRender::Init(std::shared_ptr<CharsGroupGlyph> glyph) {
  Log::v(Log::RES_TAG, "FontRender::Init");

  m_glyph = std::move(glyph);
  m_charsTexture = std::make_shared<GrayscaleTexture>(GL_TEXTURE1);
  m_charsTexture->Init();

  m_shader = ResManager::Self()->LoadShader("font");
  assert(m_shader != nullptr);

  m_uniformTextColorLoc = glGetUniformLocation(m_shader->ProgramId(), "uTextColor");
  m_uniformPrjLoc = glGetUniformLocation(m_shader->ProgramId(), "uProjection");
  m_uniformModelLoc = glGetUniformLocation(m_shader->ProgramId(), "uModel");

  // 用来绘制背景
  m_bgShader = ResManager::Self()->LoadShader("polygon");
  assert(m_bgShader != nullptr);

  m_uniformBgLocProjection = glGetUniformLocation(m_bgShader->ProgramId(), "uProjection");
  m_uniformBgLocModel = glGetUniformLocation(m_bgShader->ProgramId(), "uModel");
  m_uniformBgLocColor = glGetUniformLocation(m_bgShader->ProgramId(), "uColor");

  m_square = std::make_shared<FontSquareModel>();
  m_square->Init();

  return true;
}

void FontRender::DeInit() {
  m_charsTexture->DeInit();
  m_square->DeInit();

  m_charsTexture = nullptr;
  m_square = nullptr;
  m_shader = nullptr;
  m_bgShader = nullptr;

  Log::v(Log::RES_TAG, "FontRender::DeInit");
}

std::shared_ptr<Texture> &FontRender::GetTexture() {
  return m_charsTexture;
}

void FontRender::RenderBackground(const std::string &text,
                                  const Float2 &pos,
                                  const Float4 &bgColor,
                                  float scale,
                                  const glm::mat4 &model,
                                  const glm::mat4 &projection) {
  assert(m_bgShader != nullptr);

  m_bgShader->Use();

  float bgWidth = 0.f;
  float bgHeight = 0.f;

  // 字体原点往下多少距离。取最大的来计算背景高度
  float deltaYDown = 0.f;
  float deltaYUp = 0.f;

  for (auto c : text) {
    const CharGlyph &charGlyph = m_glyph->GetCharGlyph(c);
    if (!charGlyph.valid)
      continue;

    // 根据左下角原点计算
    deltaYDown = std::max(static_cast<float>(charGlyph.size.y - charGlyph.bearing.y) * scale, deltaYDown);
    bgHeight = std::max(static_cast<float>(charGlyph.size.h) * scale, bgHeight);

    // 更新位置到下一个字形的原点，注意单位是1/64像素
    bgWidth += static_cast<float>(charGlyph.advance.x >> 6) * scale;
  }

  bgHeight = std::max(m_glyph->GetCharRectSize().h * scale, bgHeight);

  // 边框加宽
  float deltaX = m_glyph->GetCharRectSize().w * 0.08f;
  float deltaY = m_glyph->GetCharRectSize().h * 0.08f;
  bgWidth += 2 * deltaX;
  bgHeight += 2 * deltaY;

  float startX = pos.x - deltaX;
  float startY = (pos.y - deltaYDown) - deltaY;

  GLfloat vertices[] = {
      startX, startY + bgHeight, 0.0f, 0.0f, 0.0f, // 左上角
      startX, startY, 0.0f, 0.0f, 0.0f,     // 左下角
      startX + bgWidth, startY, 0.0f, 0.0f, 0.0f, // 右下角
      startX + bgWidth, startY + bgHeight, 0.0f, 0.0f, 0.0f, // 右上角
  };

  // 绘制背景
  m_square->Bind(m_bgShader->PositionAttributeLocation(), m_bgShader->TexCoordinateAttributeLocation());
  m_square->UpdateVertex(vertices, sizeof(vertices));

  m_shader->SetFloat4(m_uniformBgLocColor, bgColor);
  m_shader->SetMatrix4(m_uniformBgLocProjection, projection);
  m_shader->SetMatrix4(m_uniformBgLocModel, model);

  m_square->Draw();
}

void FontRender::RenderText(const std::string &text,
                            const Float2 &pos,
                            const Float4 &color,
                            float scale,
                            const glm::mat4 &model,
                            const glm::mat4 &projection) {
  assert(m_shader != nullptr);

  m_shader->Use();

  m_charsTexture->Bind(m_shader->SamplerUniformLocation());

  float startX = pos.x;
  float startY = pos.y;

  for (auto c : text) {
    const CharGlyph &charGlyph = m_glyph->GetCharGlyph(c);
    if (!charGlyph.valid)
      continue;

    // 计算左下角原点和宽高
    GLfloat xpos = startX + static_cast<float>(charGlyph.bearing.x) * scale;
    GLfloat ypos = startY - static_cast<float>(charGlyph.size.y - charGlyph.bearing.y) * scale;
    GLfloat w = static_cast<float>(charGlyph.size.w) * scale;
    GLfloat h = static_cast<float>(charGlyph.size.h) * scale;

    // 计算纹理坐标左下角
    GLfloat xtex = static_cast<float>(charGlyph.index) *
                   m_glyph->GetCharRectSize().w / m_glyph->GetCharGroupSize().w;
    GLfloat ytex = (m_glyph->GetCharGroupSize().h - static_cast<float>(charGlyph.size.h))
                   / m_glyph->GetCharGroupSize().h;
    GLfloat wtex = static_cast<float>(charGlyph.size.w) / m_glyph->GetCharGroupSize().w;
    GLfloat htex = static_cast<float>(charGlyph.size.h) / m_glyph->GetCharGroupSize().h;

    GLfloat vertices[] = {
        xpos, ypos + h, 0.0f, xtex, ytex + htex, // 左上角
        xpos, ypos, 0.0f, xtex, ytex,     // 左下角
        xpos + w, ypos, 0.0f, xtex + wtex, ytex, // 右下角
        xpos + w, ypos + h, 0.0f, xtex + wtex, ytex + htex, // 右上角
    };

    // 更新位置到下一个字形的原点，注意单位是1/64像素
    startX += static_cast<float>(charGlyph.advance.x >> 6) * scale;

    // 绘制字符
    m_square->Bind(m_shader->PositionAttributeLocation(), m_shader->TexCoordinateAttributeLocation());
    m_square->UpdateVertex(vertices, sizeof(vertices));

    m_shader->SetFloat4(m_uniformTextColorLoc, color);
    m_shader->SetMatrix4(m_uniformPrjLoc, projection);
    m_shader->SetMatrix4(m_uniformModelLoc, model);

    m_square->Draw();
  }

  m_charsTexture->UnBind();
}