//
// Created by caolong on 2021/3/19.
//

#include <render/model/BaseModel.hpp>
#include <res/ResManager.hpp>
#include <render/Flow.hpp>

using namespace clt;

BaseModel::BaseModel()
    : MsgHandler(target),
      m_shader(nullptr) {

}

bool BaseModel::Init() {
  Log::v(Log::RENDER_TAG, "BaseModel::Init");

  loadShader();
  loadModels();
  registerVar();

//    initBase();

  Flow::Self()->AddMsgHandler(shared_from_this());

  return true;
}

void BaseModel::DeInit() {
  Flow::Self()->RemoveMsgHandler(shared_from_this());

  m_shader = nullptr;
  m_models.clear();

  gles::DeleteVao(m_vao);
  gles::DeleteXbo(m_vbo);
  gles::DeleteXbo(m_cube.cubeEbo);
  gles::DeleteXbo(m_pyramid.pyramidEbo);

  m_colorVars.Clear();
  m_mat4Vars.Clear();

  Log::v(Log::RENDER_TAG, "BaseModel::DeInit");
}

void BaseModel::SetVariable(const std::string &varName, const glm::mat4 &var) {
  m_mat4Vars.SetVariable(varName, var);
}

void BaseModel::SetVariable(const std::string &varName, const Color &var) {
  m_colorVars.SetVariable(varName, var);
}

void BaseModel::OnMsgHandle(const Msg &msg) {
  switch (msg.type) {
    case Msg::Type::MSG_BASE_MODEL:
      if (msg.data != nullptr) {
        m_models[msg.what] = std::dynamic_pointer_cast<BaseModelMsgData>(msg.data);
      } else {
        m_models.erase(msg.what);
      }
      break;
    default:
      break;
  }
}

void BaseModel::Render(const Viewport &viewport) {
//    updateViewport(viewport);
//    updateBase(viewport);
  draw();
}

void BaseModel::loadModels() {
//    auto cubeVertex = createCubeVertex();
//    auto cubeIndex = createCubeIndex();
//
//    gles::CreateXbo(GL_ARRAY_BUFFER, GL_STATIC_DRAW,
//                    cubeVertex.size() * sizeof(float), (void *) cubeVertex.data(), m_vbo);
//
//    gles::CreateXbo(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW,
//                    cubeIndex.size() * sizeof(unsigned short), (void *) cubeIndex.data(), m_cube.cubeEbo);

//    m_cube.count = cubeIndex.size();

  auto pyramidVertex = createPyramidVertex();
  auto pyramidIndex = createPyramidIndex();

  gles::CreateXbo(GL_ARRAY_BUFFER, GL_STATIC_DRAW,
                  pyramidVertex.size() * sizeof(float), (void *) pyramidVertex.data(), m_vbo);

  gles::CreateXbo(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW,
                  pyramidIndex.size() * sizeof(unsigned short), (void *) pyramidIndex.data(), m_pyramid.pyramidEbo);

  m_pyramid.count = pyramidIndex.size();

  gles::CreateVao(m_vao);
  gles::UseVao(m_vao, [this, pos = m_shader->PositionAttributeLocation(),
      color = m_shader->ColorAttributeLocation()]() {
    if (pos != -1) {
      gles::UploadVertexAttrib(m_vbo, pos,
                               s_positionPer,
                               s_vertexCoordsPer * sizeof(GLfloat),
                               nullptr);
    }
    if (color != -1) {
      gles::UploadVertexAttrib(m_vbo, color,
                               s_colorPer,
                               s_vertexCoordsPer * sizeof(GLfloat),
                               (void *) (s_positionPer * sizeof(float)));
    }
  });
}

void BaseModel::loadShader() {
  m_shader = ResManager::Self()->LoadShader("base_model");
  assert(m_shader != nullptr);

  m_uniformLocProjection = glGetUniformLocation(m_shader->ProgramId(), "uProjection");
  m_uniformLocView = glGetUniformLocation(m_shader->ProgramId(), "uView");
  m_uniformLocModel = glGetUniformLocation(m_shader->ProgramId(), "uModel");
  m_uniformLocColor = glGetUniformLocation(m_shader->ProgramId(), "uColor");
}

void BaseModel::updateValue() {
//    m_shader->SetFloat4(m_uniformLocColor, m_color);
  m_shader->SetMatrix4(m_uniformLocProjection, m_projectionMatrix);
  m_shader->SetMatrix4(m_uniformLocView, m_viewMatrix);
  m_shader->SetMatrix4(m_uniformLocModel, m_modelMatrix);
}

void BaseModel::registerVar() {
  m_colorVars.Register(var_color, {[this](const Color &var) {
    m_color = var;
  }});
  m_mat4Vars.Register(var_projection_matrix, {[this](const glm::mat4 &var) {
    m_projectionMatrix = var;
  }});
  m_mat4Vars.Register(var_view_matrix, {[this](const glm::mat4 &var) {
    m_viewMatrix = var;
  }});
  m_mat4Vars.Register(var_model_matrix, {[this](const glm::mat4 &var) {
    m_modelMatrix = var;
  }});
}

void BaseModel::draw() {
  m_shader->Use();
  gles::UseVao(m_vao, [this]() {
    for (auto &data: m_models) {
      if (data.second != nullptr) {
        auto objects = data.second->objects;
        for (auto &object : objects) {
          updateValue(object.projection, object.view, object.model, object.color);

          switch (object.type) {
//                        case BaseModelObject::Type::CUBE:
//                            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cube.cubeEbo);
//                            glDrawElements(GL_TRIANGLES, m_cube.count, GL_UNSIGNED_SHORT, nullptr);
//                            break;
            case BaseModelObject::Type::PYRAMID:
              glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pyramid.pyramidEbo);
              glDrawElements(GL_TRIANGLES, m_pyramid.count, GL_UNSIGNED_SHORT, nullptr);
              break;
            default:
              break;
          }
        }
      }
    }
  });
}

void BaseModel::updateValue(const glm::mat4 &_projection,
                            const glm::mat4 &_view,
                            const glm::mat4 &_model,
                            const Color &_color) {
//    m_shader->SetFloat4(m_uniformLocColor, _color);
  m_shader->SetMatrix4(m_uniformLocProjection, _projection);
  m_shader->SetMatrix4(m_uniformLocView, _view);
  m_shader->SetMatrix4(m_uniformLocModel, _model);
}

std::vector<float> BaseModel::createCubeVertex() {
  return {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,// 上面 0
          0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // 1
          -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, // 2
          -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // 3
          0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,// 下面 4
          0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // 5
          -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, // 6
          -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,}; // 7
}

std::vector<unsigned short> BaseModel::createCubeIndex() {
  return {//上
      0, 1, 2, 1, 3, 2,
      //下
      4, 6, 5, 6, 7, 5,
      //左
      6, 2, 7, 2, 3, 7,
      //右
      5, 1, 4, 1, 0, 4,
      //前
      4, 0, 6, 0, 2, 6,
      //后
      7, 3, 5, 3, 1, 5};
}

std::vector<float> BaseModel::createPyramidVertex() {
//    return {0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, // 0
//            0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, // 1
//            -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, // 2
//            -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, // 3
//            0.0f, 0.25f, -0.5f, 0.0f, 0.0f, 1.0f, // 4 上顶点
//            0.0f, -0.25f, -0.5f, 0.0f, 0.0f, 1.0f, // 5 下顶点
//            };

  return {0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // 0
          0.0f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // 1
          1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // 2
          0.34f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, // 3
  };
}

std::vector<unsigned short> BaseModel::createPyramidIndex() {
//    return {// 前面
//        0, 2, 1, 2, 3, 1,
//        // 上
//        0, 4, 2,
//        //左
//        2, 4, 3, 4, 5, 3,
//        //右
//        0, 1, 4, 1, 5, 4,
//        // 下
//        3, 5, 1};
  return {// 前
      0, 1, 2,
      // 左
      0, 3, 1,
      // 右
      0, 2, 3,
      // 下
      2, 1, 3,};
}

void BaseModel::getRealWorldProjection(int width, int height, float near, float far, glm::mat4 &projection) {
  // 模拟摄像机内参矩阵
  double fx = 2494.0;
  double cx = static_cast<double >(width) / 2;
  double fy = 2494.0;
  double cy = static_cast<double >(height) / 2;

  projection = glm::mat4(0.0f);
  projection[0][0] = 2 * fx / width;
  projection[1][1] = 2 * fy / height;
  projection[2][0] = 1.0 - 2 * cx / width;
  projection[2][1] = 2 * cy / height - 1.0;
  projection[2][2] = -(far + near) / (far - near);
  projection[2][3] = -1.0;
  projection[3][2] = -2.0 * far * near / (far - near);
}

void BaseModel::initBase() {
  std::vector<BaseModelObject> baseModelObjects;
  baseModelObjects.emplace_back(BaseModelObject::Type::CUBE,
                                glm::mat4(1.0), glm::mat4(1.0), glm::mat4(), GreenColor);
  m_models["base"] = std::make_shared<BaseModelMsgData>(std::move(baseModelObjects));
}

void BaseModel::updateBase(const Viewport &viewport) {
  static size_t count = 0;
  size_t cur = count++;

  float unit = 1.8f;
  float phase = (2 * cur) % 360;
  glm::mat4 model(1.0f);
//    model = glm::translate(model, glm::vec3(unit, unit * std::sin(glm::radians(phase)), 0.0f));
  model = glm::translate(model, glm::vec3(unit, -1.5 * unit, 0.0f));
  model = glm::rotate(model, glm::radians((float) ((10 * cur) % 360)), glm::vec3(0.0f, 1.0f, 0.0f));

  float ratio = (float) viewport.height / (float) viewport.width;
  float ratioBase = 0.8f;
  model = glm::scale(model, glm::vec3(ratioBase, ratioBase * ratio, ratioBase * 0.2));

  m_models["base"]->objects[0].model = model;
}

void BaseModel::updateViewport(const Viewport &viewport) {
  // 测试立方体 摄像机在世界坐标系 +Z 轴  处
  glm::mat4 view;
  view = glm::lookAt(glm::vec3(0.0f, 0.0f, 10.0f), // eye
                     glm::vec3(0.0f, 0.0f, 0.0f), // center
                     glm::vec3(0.0f, 1.0f, 0.0f));// up
  m_models["base"]->objects[0].view = view;

  glm::mat4 glProjection = glm::perspective(glm::radians(45.f),
                                            (float) viewport.width / (float) viewport.height, 0.1f, 1000.0f);
//    glm::mat4 glProjection;
//    getRealWorldProjection(viewport.width, viewport.height, 0.1f, 1000.f, glProjection);

  m_models["base"]->objects[0].projection = glProjection;
}