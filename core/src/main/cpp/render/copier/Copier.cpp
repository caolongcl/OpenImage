//
// Created by caolong on 2020/6/20.
//

#include <render/Flow.hpp>
#include <res/ResManager.hpp>
#include <res/Printer.hpp>
#include <render/copier/Copier.hpp>
#include <render/texture/Texture.hpp>
#include <utils/Utils.hpp>
#include <res/Shader.hpp>
#include <res/SquareModel.hpp>
#include <render/model/BaseModel.hpp>
#include <res/ResManager.hpp>
#include <utils/FpsStatics.hpp>

using namespace clt;

Copier::Copier() :
    MsgHandler(target),
    m_square(new SquareModel()),
    m_polygon(new Polygon()),
    m_baseModels(new BaseModel()),
    m_fpsStatics(new FpsStatics()),
    m_displayParams() {}

bool Copier::Init() {
  Log::v(target, "Copier::Init");

  m_shader = ResManager::Self()->LoadShader("default");
  assert(m_shader != nullptr);

  m_square->Init();
  m_square->Bind(m_shader->PositionAttributeLocation(), m_shader->TexCoordinateAttributeLocation());

  m_fpsStatics->Init();
  m_polygon->Init();
  m_baseModels->Init();

  setTextForDebug();

  Flow::Self()->AddMsgHandler(shared_from_this());

  return true;
}

void Copier::DeInit() {
  Flow::Self()->RemoveMsgHandler(shared_from_this());

  m_debugTexts.clear();
  m_polygonTasks.clear();

  m_baseModels->DeInit();
  m_polygon->DeInit();
  m_fpsStatics->DeInit();

  m_square->DeInit();
  m_input = nullptr;
  m_output = nullptr;
  m_shader = nullptr;

  m_displayParams = {};

  Log::v(target, "Copier::DeInit");
}

void Copier::Update() {
  update();
}

void Copier::OnUpdate(OPreviewSize &&t) {
  Log::d(target, "Copier::OnUpdate preview size (%d %d)", t.width, t.height);

  m_previewParams.width = t.width;
  m_previewParams.height = t.height;
  m_previewParams.viewport = {0, 0, t.width, t.height};

  update();
}

void Copier::CopyPreFrame(const Viewport &viewport) {
  gles::Clear(viewport);

  m_shader->Use();
  m_input->Bind(m_shader->SamplerUniformLocation());
  m_square->Draw();
  m_input->UnBind();
}

void Copier::Copy() {
  gles::Clear(m_previewParams.viewport);

  m_shader->Use();
  m_input->Bind(m_shader->SamplerUniformLocation());
  m_square->Draw();
  m_input->UnBind();

  postRender();
}

void Copier::CopyPostFrame(const Viewport &viewport) {
  gles::Clear(viewport);

  m_shader->Use();
  m_output->Bind(m_shader->SamplerUniformLocation());
  m_square->Draw();
  m_output->UnBind();
}

void Copier::OnMsgHandle(const Msg &msg) {
  switch (msg.type) {
    case Msg::Type::MSG_POLYGON:
      if (msg.data != nullptr) {
        m_polygonTasks[msg.what] = std::dynamic_pointer_cast<PolygonMsgData>(msg.data);
      } else {
        m_polygonTasks.erase(msg.what);
      }
      break;
    case Msg::Type::MSG_TEXT:
      if (msg.data != nullptr) {
        m_textTasks[msg.what] = std::dynamic_pointer_cast<TextMsgData>(msg.data);
      } else {
        m_textTasks.erase(msg.what);
      }
      break;
    default:
      break;
  }
}

void Copier::SetInput(std::shared_ptr<Texture> input) {
  m_input = std::move(input);
}

void Copier::SetOutput(std::shared_ptr<Texture> output) {
  m_output = std::move(output);
}

std::shared_ptr<Texture> &Copier::GetOutput() {
  return m_output;
}

Copier &Copier::SetSurfaceSize(int width, int height) {
  m_displayParams.width = width;
  m_displayParams.height = height;
  return *this;
}

bool Copier::PositionInDisplayRegion(int x, int y) const {
  return inDisplayRegion(x, m_displayParams.viewport.height - y);
}

void Copier::AdjustPositionToDisplayRegion(int &x, int &y) const {
  x = x - m_displayParams.viewport.x;
  y = y - m_displayParams.viewport.y;
  y = m_displayParams.viewport.height - y;
}

const Viewport &Copier::GetDisplayViewport() const {
  return m_displayParams.viewport;
}

const Viewport &Copier::GetPreviewViewport() const {
  return m_previewParams.viewport;
}

void Copier::update() {
  if (!m_displayParams.Valid() || !m_previewParams.Valid()) {
    Log::w(target, "Copier::update params invalid, preview(%d %d) display(%d %d)",
           m_previewParams.width, m_previewParams.height,
           m_displayParams.width, m_displayParams.height);
    return;
  }

  update(m_previewParams, m_displayParams);

  Log::d(target,
         "Copier::update preview viewport (%d %d %d %d) display viewport (%d %d %d %d)",
         m_previewParams.viewport.x, m_previewParams.viewport.y,
         m_previewParams.viewport.width, m_previewParams.viewport.height,
         m_displayParams.viewport.x, m_displayParams.viewport.y,
         m_displayParams.viewport.width, m_displayParams.viewport.height);
}

void Copier::update(PreviewParams &previewParams, DisplayParams &displayParams) {
  int x = 0, y = 0;
  auto width = static_cast<float>(displayParams.width);
  auto height = static_cast<float>(displayParams.height);

  float displayAspectRatio = static_cast<float>(displayParams.width) / static_cast<float>(displayParams.height);
  float previewAspectRatio = static_cast<float>(previewParams.width) / static_cast<float>(previewParams.height);

  if (displayAspectRatio > previewAspectRatio) {
    // display 区域比预览图像按比例要宽，以 display 的高为准，算出显示区域宽
    height = static_cast<float>(displayParams.width);
    width = static_cast<float>(previewParams.width)
            * (static_cast<float>(height) / static_cast<float>(previewParams.height));

    x = static_cast<int>((static_cast<float>(displayParams.width) - width) / 2);
    y = 0;
  } else if (displayAspectRatio < previewAspectRatio) {
    // display 区域比预览图像按比例要窄，以 display宽为准，算出显示区域高
    width = static_cast<float>(displayParams.width);
    height = static_cast<float>(previewParams.height)
             * (static_cast<float>( width) / static_cast<float>(previewParams.width));

    x = 0;
    y = static_cast<int>((static_cast<float>(displayParams.height) - height) / 2);
  }

  displayParams.viewport = {x, y, Utils::RoundToEven(width), Utils::RoundToEven(height)};
}

bool Copier::inDisplayRegion(int x, int y) const {
  return x > m_displayParams.viewport.x
         && x < (m_displayParams.viewport.x + m_displayParams.viewport.width)
         && y > m_displayParams.viewport.y
         && y < (m_displayParams.viewport.y + m_displayParams.viewport.height);
}

void Copier::postRender() {
  m_fpsStatics->UpdateStatistics();

  renderPolygon();
  renderBaseModel();
  renderText();
}

void Copier::renderPolygon() {
  for (auto &t : m_polygonTasks) {
    auto polygon = t.second;
    for (auto &object : polygon->objects) {
      m_polygon->UpdatePolygon(object, m_previewParams.viewport);
      m_polygon->Render();
    }
  }
}

void Copier::renderBaseModel() {
  m_baseModels->Render(m_previewParams.viewport);
}

void Copier::renderText() {
  for (auto &text : m_debugTexts) {
    text(m_displayParams.viewport, m_previewParams.viewport);
  }

  for (auto &text : m_textTasks) {
    TextInfo info(text.second->textInfo);
    info.displayViewport = m_displayParams.viewport;
    info.previewViewport = m_previewParams.viewport;
    Printer::Self()->Print(info);
  }
}

void Copier::setTextForDebug() {
  // 显示预览和surface参数
  m_debugTexts.emplace_back([this](const Viewport &displayViewport,
                                   const Viewport &previewViewport) {
    TextInfo info(m_fpsStatics->m_fps, displayViewport, previewViewport);
    info.position = {100.0f, 0.0f};
    return info;
  });

  m_debugTexts.emplace_back([this](const Viewport &displayViewport,
                                   const Viewport &previewViewport) {
    std::string text = "surface size:";
    text += std::to_string(m_displayParams.width);
    text += " ";
    text += std::to_string(m_displayParams.height);

    TextInfo info(text, displayViewport, previewViewport);
    info.position = {100.0f, 50.0f};
    return info;
  });

  m_debugTexts.emplace_back([this](const Viewport &displayViewport,
                                   const Viewport &previewViewport) {
    std::string text = "preview size:";
    text += std::to_string(m_previewParams.width);
    text += " ";
    text += std::to_string(m_previewParams.height);

    TextInfo info(text, displayViewport, previewViewport);
    info.position = {100.0f, 100.0f};
    return info;
  });

  m_debugTexts.emplace_back([this](const Viewport &displayViewport,
                                   const Viewport &previewViewport) {
    std::string text = "surface viewport:";
    text += std::to_string(m_displayParams.viewport.x);
    text += " ";
    text += std::to_string(m_displayParams.viewport.y);
    text += " ";
    text += std::to_string(m_displayParams.viewport.width);
    text += " ";
    text += std::to_string(m_displayParams.viewport.height);

    TextInfo info(text, displayViewport, previewViewport);
    info.position = {100.0f, 150.0f};
    return info;
  });

  // 版本信息
  m_debugTexts.emplace_back([this](const Viewport &displayViewport,
                                   const Viewport &previewViewport) {
    TextInfo info("version 1.0", displayViewport, previewViewport);
    info.position = {200.0f, 400.0f};
    return info;
  });
  m_debugTexts.emplace_back([this](const Viewport &displayViewport,
                                   const Viewport &previewViewport) {
    TextInfo info("https://github.com/caolongcl/OpenImage", displayViewport, previewViewport);
    info.position = {200.0f, 450.0f};
    return info;
  });
}