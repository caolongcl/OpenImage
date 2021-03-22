//
// Created by caolong on 2020/8/10.
//

#include <process/base/RGBAToGray.hpp>
#include <res/ResManager.hpp>
#include <res/Shader.hpp>
#include <res/SquareModel.hpp>
#include <render/texture/Texture.hpp>

using namespace clt;

RGBAToGray::RGBAToGray()
        : FilterCom("rgba2gray", PriorityNormal) {

}

bool RGBAToGray::Init() {
    loadShader();

    m_square = std::make_shared<SquareModel>();
    m_square->Init();
    return true;
}

void RGBAToGray::DeInit() {
    m_square->DeInit();
    m_square = nullptr;
    m_shader = nullptr;
}

void RGBAToGray::Filter() {
    assert(m_input != nullptr);

    m_square->Bind(m_shader->PositionAttributeLocation(),
                   m_shader->TexCoordinateAttributeLocation());
    m_shader->Use();
    m_input->Bind(m_shader->SamplerUniformLocation());
    updateValue();
    m_square->Draw();
    m_input->UnBind();
}

void RGBAToGray::loadShader() {
    m_shader = ResManager::Self()->LoadShader("rgba2gray");
    assert(m_shader != nullptr);
}

void RGBAToGray::updateValue() {
}

void RGBAToGray::registerVar() {
}
