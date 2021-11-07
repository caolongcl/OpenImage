//
// Created by caolong on 2020/7/20.
//

#include <res/ResManager.hpp>
#include <render/copier/OESCopier.hpp>
#include <softarch/Constants.hpp>
#include <utils/Utils.hpp>
#include <res/Shader.hpp>
#include <res/SquareModel.hpp>
#include <render/texture/Texture.hpp>

using namespace clt;

OESCopier::OESCopier() :
    m_rotation(Constants::ROTATION_90),
    m_shader(nullptr),
    m_square(new SquareModel()) {
}

bool OESCopier::Init() {
  Log::v(Log::RENDER_TAG, "OESCopier::Init");

  m_shader = ResManager::Self()->LoadShader("oes");
  assert(m_shader != nullptr);

  m_square->Init();

  // 准备绘制的数据
  m_square->Bind(m_shader->PositionAttributeLocation(), m_shader->TexCoordinateAttributeLocation());

  return true;
}

void OESCopier::DeInit() {
  ClearObservers();

  m_square->DeInit();
  m_shader = nullptr;
  m_input = nullptr;
  m_output = nullptr;
  Log::v(Log::RENDER_TAG, "OESCopier::DeInit");
}

void OESCopier::SetInput(std::shared_ptr<Texture> input) {
  m_input = std::move(input);
}

void OESCopier::SetOutput(std::shared_ptr<Texture> output) {
  m_output = std::move(output);
}

std::shared_ptr<Texture> &OESCopier::GetOutput() {
  return m_output;
}

bool OESCopier::Update() {
  assert(m_input != nullptr);
  assert(m_output != nullptr);

  update();

  // 通知预览输出纹理的大小改变
  Notify(OPreviewSize(m_output->Width(), m_output->Height()));

  return true;
}

OESCopier &OESCopier::SetSize(int width, int height) {
  assert(m_input != nullptr);

  m_input->Upload(width, height);
  return *this;
}

OESCopier &OESCopier::SetRotation(int rotation) {
  if (!Constants::ValidRotation(rotation)) {
    rotation = Constants::ROTATION_90;
  }

  m_rotation = rotation;

  return *this;
}

OESCopier &OESCopier::SetRatio(int ratio) {
  if (!Constants::ValidRatio(ratio)) {
    ratio = Constants::RATIO_OTHER;
  }

  m_ratio = ratio;
  return *this;
}

OESCopier &OESCopier::SetVFlip(bool vFlip) {
  m_vFlip = vFlip;
  return *this;
}

OESCopier &OESCopier::SetHFlip(bool hFlip) {
  m_hFlip = hFlip;
  return *this;
}

void OESCopier::CopyFrame() {
  assert(m_input != nullptr);

  m_shader->Use();
  m_input->Bind(m_shader->SamplerUniformLocation());
  m_square->Draw();
  m_input->UnBind();
}

/**************************************************************************************************/
void OESCopier::update() {
  assert(m_input != nullptr);
  assert(m_output != nullptr);

  float xOffset = 0.0f;
  float yOffset = 0.0f;
  int targetW, targetH;

  /// 1. 将预览帧按设置的宽高比裁剪
  update(m_ratio, m_input->Width(), m_input->Height(), xOffset, yOffset, targetW, targetH);

  GLfloat tex[] = {
      xOffset, 1.0f - yOffset,  // 0 左上角
      xOffset, yOffset,  // 1 左下角
      1.0f - xOffset, yOffset, // 2 右下角
      1.0f - xOffset, 1.0f - yOffset // 3 右上角
  };

  /// 2. 旋转纹理
  rotateTex(m_rotation, tex);

  /// 3. 翻转纹理
  if (m_rotation == Constants::ROTATION_90 || m_rotation == Constants::ROTATION_270) {
    flipTex(m_hFlip, m_vFlip, tex);
  } else {
    flipTex(m_vFlip, m_hFlip, tex);
  }

  //上传更新的纹理坐标
  m_square->UpdateTextureCoordinates(tex, sizeof(tex));

  // 更新输出纹理大小
  if (m_rotation == Constants::ROTATION_90 || m_rotation == Constants::ROTATION_270) {
    std::swap(targetW, targetH);
  }

  if (targetW != m_output->Width() || targetH != m_output->Height()) {
    m_output->Upload(targetW, targetH);
  }

  Log::d(Log::RENDER_TAG,
         "OESCopier::Update clip offset(%f %f) clipped preview size(%d %d)",
         xOffset, yOffset, targetW, targetH);
}

void OESCopier::update(int ratio, int originPreviewW, int originPreviewH,
                       float &xOffset, float &yOffset, int &targetPreviewW, int &targetPreviewH) {
  xOffset = 0.0f;
  yOffset = 0.0f;
  targetPreviewW = originPreviewW;
  targetPreviewH = originPreviewH;

  float targetRatio;

  if (ratio == Constants::RATIO_4_3) {
    targetRatio = 4.0f / 3.0f;
  } else if (ratio == Constants::RATIO_16_9) {
    targetRatio = 16.0f / 9.0f;
  } else {
    Log::w(Log::RENDER_TAG, "no support this ratio %d", ratio);
    return;
  }

  float width;
  float height;
  float originRatio = (float) originPreviewW / (float) originPreviewH;

  if (originRatio > targetRatio) { // 预览帧相对宽一点，保持预览帧高不变，裁宽度
    height = static_cast<float>(originPreviewH);
    width = height * targetRatio;
    targetPreviewW = Utils::RoundToEven(width);
    targetPreviewH = Utils::RoundToEven(height);

    // 计算裁剪位置 X 相对预览帧比例
    xOffset = (static_cast<float>(originPreviewW) - width) / (2.0f * (float) originPreviewW);
  } else if (originRatio < targetRatio) {
    width = static_cast<float>(originPreviewW);
    height = width / targetRatio;

    targetPreviewW = Utils::RoundToEven(width);
    targetPreviewH = Utils::RoundToEven(height);

    // 计算裁剪位置 Y 相对预览帧比例
    yOffset = (static_cast<float>(originPreviewH) - height) / (2.0f * (float) originPreviewH);
  } else {
    targetPreviewW = Utils::RoundToEven(static_cast<float>(originPreviewW));
    targetPreviewH = Utils::RoundToEven(static_cast<float>(originPreviewH));
  }
}

void OESCopier::swapTexRow(int row0, int row1, GLfloat *tex) {
  std::swap(tex[row0 * 2], tex[row1 * 2]);
  std::swap(tex[row0 * 2 + 1], tex[row1 * 2 + 1]);
}

void OESCopier::rotateTex(int rotate, GLfloat *tex) {
  switch (rotate) {
    case Constants::ROTATION_90:
      //
      swapTexRow(0, 1, tex);
      swapTexRow(1, 2, tex);
      swapTexRow(2, 3, tex);
      break;
    case Constants::ROTATION_180:
      //
      swapTexRow(0, 2, tex);
      swapTexRow(1, 3, tex);
      break;
    case Constants::ROTATION_270:
      //
      swapTexRow(2, 3, tex);
      swapTexRow(1, 2, tex);
      swapTexRow(0, 1, tex);
      break;
    case Constants::ROTATION_0:
    default:
      break;
  }
}

float OESCopier::flip(float origin, bool flip) {
  return flip ? 1.0f - origin : origin;
}

void OESCopier::flipTex(bool vFlip, bool hFlip, GLfloat *tex) {
  for (int i = 0; i < 4; ++i) {
    tex[i * 2] = flip(tex[i * 2], hFlip);
    tex[i * 2 + 1] = flip(tex[i * 2 + 1], vFlip);
  }
}
