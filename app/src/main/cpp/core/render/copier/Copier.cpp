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

using namespace clt;

Copier::Copier() :
        MsgHandler(target),
        m_square(new SquareModel()),
        m_polygon(new Polygon()),
        m_baseModels(new BaseModel()),
        m_fpsStatics(new FpsStatics()),
        m_surfaceParams() {}

bool Copier::Init() {
    Log::v(Log::RENDER_TAG, "Copier::Init");

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

    m_surfaceParams = {};

    Log::v(Log::RENDER_TAG, "Copier::DeInit");
}

void Copier::Update() {
    update();
}

/**
 * 预览大小改变
 * @param t
 */
void Copier::OnUpdate(OPreviewSize &&t) {
    Log::d(Log::RENDER_TAG, "Copier::OnUpdate preview size (%d %d)", t.width, t.height);

    m_surfaceParams.previewW = t.width;
    m_surfaceParams.previewH = t.height;
    m_surfaceParams.previewViewport = {0, 0, t.width, t.height};

    update();
}


/**
 * 拷贝输入纹理到输出纹理，进行额外绘制
 */
void Copier::Copy() {
    gles::Clear(m_surfaceParams.previewViewport);

    m_shader->Use();
    m_input->Bind(m_shader->SamplerUniformLocation());
    m_square->Draw();
    m_input->UnBind();

    postRender();
}

/**
 * 用于拍摄和录制
 * @param viewport
 */
void Copier::CopyPreFrame(const Viewport &viewport) {
    gles::Clear(viewport);

    m_shader->Use();
    m_input->Bind(m_shader->SamplerUniformLocation());
    m_square->Draw();
    m_input->UnBind();
}

/**
 * 用于绘制到屏幕上
 * @param viewport
 */
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
    m_surfaceParams.surfaceW = width;
    m_surfaceParams.surfaceH = height;
    return *this;
}

bool Copier::PositionInSurface(int x, int y) const {
    return inSurface(x, m_surfaceParams.surfaceH - y);
}

void Copier::AdjustPositionsToSurface(int &x, int &y) const {
    x = x - m_surfaceParams.surfaceViewport.x;
    y = y - m_surfaceParams.surfaceViewport.y;
    y = m_surfaceParams.surfaceViewport.height - y;
}

const Viewport &Copier::GetSurfaceViewport() const {
    return m_surfaceParams.surfaceViewport;
}

const Viewport &Copier::GetPreviewViewport() const {
    return m_surfaceParams.previewViewport;
}

const SurfaceParams &Copier::GetSurfaceParams() const {
    return m_surfaceParams;
}

void Copier::update() {
    if (!m_surfaceParams.Valid()) {
        Log::w(Log::RENDER_TAG, "Copier::update surface params invalid %d %d %d %d",
               m_surfaceParams.surfaceW, m_surfaceParams.surfaceH,
               m_surfaceParams.previewW, m_surfaceParams.previewH);
        return;
    }

    update(m_surfaceParams);

    Log::d(Log::RENDER_TAG,
           "Copier::update preview viewport (%d %d %d %d) surface viewport (%d %d %d %d)",
           m_surfaceParams.previewViewport.x, m_surfaceParams.previewViewport.y,
           m_surfaceParams.previewViewport.width, m_surfaceParams.previewViewport.height,
           m_surfaceParams.surfaceViewport.x, m_surfaceParams.surfaceViewport.y,
           m_surfaceParams.surfaceViewport.width, m_surfaceParams.surfaceViewport.height);
}

/**
 * 根据预览大小和显示大小以及缩放模式计算viewport
 * @param params
 * @param xOffset
 * @param yOffset
 * @param viewport
 */
void Copier::update(SurfaceParams &params) {
    int x = 0, y = 0;
    auto width = static_cast<float>(params.surfaceW);
    auto height = static_cast<float>(params.surfaceH);

    float surfaceAspectRatio = static_cast<float>(params.surfaceW) / static_cast<float>(params.surfaceH);
    float previewAspectRatio = static_cast<float>(params.previewW) / static_cast<float>(params.previewH);

    if (surfaceAspectRatio > previewAspectRatio) {
        // surface区域比预览图像按比例宽，以surface高为准，算出显示区域宽
        height = static_cast<float>(params.surfaceH);
        width = static_cast<float>(params.previewW)
                * (static_cast<float>( height) / static_cast<float>(params.previewH));

        x = static_cast<int>((static_cast<float>(params.surfaceW) - width) / 2);
        y = 0;
    } else if (surfaceAspectRatio < previewAspectRatio) {
        // surface区域比预览图像按比例窄，以surface宽为准，算出显示区域高
        width = static_cast<float>(params.surfaceW);
        height = static_cast<float>(params.previewH)
                 * (static_cast<float>( width) / static_cast<float>(params.previewW));

        x = 0;
        y = static_cast<int>((static_cast<float>(params.surfaceH) - height) / 2);
    }

    params.surfaceViewport = {x, y, Utils::RoundToEven(width), Utils::RoundToEven(height)};
}

bool Copier::inSurface(int x, int y) const {
    return x > m_surfaceParams.surfaceViewport.x
           && x < (m_surfaceParams.surfaceViewport.x + m_surfaceParams.surfaceViewport.width)
           && y > m_surfaceParams.surfaceViewport.y
           && y < (m_surfaceParams.surfaceViewport.y + m_surfaceParams.surfaceViewport.height);
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
            m_polygon->UpdatePolygon(object, m_surfaceParams.previewViewport);
            m_polygon->Render();
        }
    }
}

void Copier::renderBaseModel() {
    m_baseModels->Render(m_surfaceParams.previewViewport);
}

void Copier::renderText() {
    for (auto &text : m_debugTexts) {
        text(m_surfaceParams.surfaceViewport, m_surfaceParams.previewViewport);
    }

    for (auto &text : m_textTasks) {
        TextInfo info(text.second->textInfo);
        info.displayViewport = m_surfaceParams.surfaceViewport;
        info.realViewport = m_surfaceParams.previewViewport;
        Printer::Self()->Print(info);
    }
}

void Copier::setTextForDebug() {
    // 显示预览和surface参数
    m_debugTexts.emplace_back([this](const Viewport &displayViewport,
                                     const Viewport &realViewport) {
        TextInfo info(m_fpsStatics->m_fps, displayViewport, realViewport);
        info.position = {100.0f, 0.0f};
        return info;
    });

    m_debugTexts.emplace_back([this](const Viewport &displayViewport,
                                     const Viewport &realViewport) {
        std::string text = "surface size:";
        text += std::to_string(m_surfaceParams.surfaceW);
        text += " ";
        text += std::to_string(m_surfaceParams.surfaceH);

        TextInfo info(text, displayViewport, realViewport);
        info.position = {100.0f, 50.0f};
        return info;
    });

    m_debugTexts.emplace_back([this](const Viewport &displayViewport,
                                     const Viewport &realViewport) {
        std::string text = "preview size:";
        text += std::to_string(m_surfaceParams.previewW);
        text += " ";
        text += std::to_string(m_surfaceParams.previewH);

        TextInfo info(text, displayViewport, realViewport);
        info.position = {100.0f, 100.0f};
        return info;
    });

    m_debugTexts.emplace_back([this](const Viewport &displayViewport,
                                     const Viewport &realViewport) {
        std::string text = "surface viewport:";
        text += std::to_string(m_surfaceParams.surfaceViewport.x);
        text += " ";
        text += std::to_string(m_surfaceParams.surfaceViewport.y);
        text += " ";
        text += std::to_string(m_surfaceParams.surfaceViewport.width);
        text += " ";
        text += std::to_string(m_surfaceParams.surfaceViewport.height);

        TextInfo info(text, displayViewport, realViewport);
        info.position = {100.0f, 150.0f};
        return info;
    });

    // 版本信息
    m_debugTexts.emplace_back([this](const Viewport &displayViewport,
                                     const Viewport &realViewport) {
        TextInfo info("version 1.0", displayViewport, realViewport);
        info.position = {200.0f, 400.0f};
        return info;
    });
    m_debugTexts.emplace_back([this](const Viewport &displayViewport,
                                     const Viewport &realViewport) {
        TextInfo info("author 1106912908@qq.com", displayViewport, realViewport);
        info.position = {200.0f, 450.0f};
        return info;
    });
}

///

void FpsStatics::UpdateStatistics() {
    // 统计帧率
    if (m_lastFrameTime == -1) {
        m_lastFrameTime = Utils::CurTimeMilli();
        m_framesStartPeriod = m_lastFrameTime;
        m_framesCount = 0;
        return;
    }
    long curFrameTime = Utils::CurTimeMilli();

    int fps = static_cast<int>(1000.0f / static_cast<float>(curFrameTime - m_lastFrameTime));
    if (fps > m_maxFps)
        m_maxFps = fps;
    if (fps < m_minFps)
        m_minFps = fps;

    m_lastFrameTime = curFrameTime;

    m_framesCount++;

    std::stringstream ss;

    if (curFrameTime - m_framesStartPeriod >= 1000 * 5) { //5s计算一次平均值
        m_avgFps = static_cast<int>(static_cast<float>(m_framesCount) * 1000.0f /
                                    static_cast<float>(curFrameTime - m_framesStartPeriod));
        m_framesStartPeriod = curFrameTime;
        m_framesCount = 0;
        ss << "fps:" << fps << " (5s min:" << m_minFps << " max:" << m_maxFps << " avg:" << m_avgFps
           << ")";

        m_fps = std::move(ss.str());

        m_minFps = 9999;
        m_maxFps = 0;
    } else {
        ss << "fps:" << fps << " (5s min:" << m_minFps << " max:" << m_maxFps << " avg:" << m_avgFps
           << ")";
        m_fps = std::move(ss.str());
    }
}