//
// Created by caolong on 2020/8/12.
//

#include <res/ResManager.hpp>
#include <render/polygon/Polygon.hpp>
#include <res/Printer.hpp>

using namespace clt;

Polygon::Polygon()
        : PolygonDrawer(target),
          m_color(RedColor),
          m_lineWidth(10),
          m_vertexCount(0),
          m_fill(false) {

}

bool Polygon::Init() {
    PolygonDrawer::Init();

    GLint lingWidth[2];
    glGetIntegerv(GL_ALIASED_LINE_WIDTH_RANGE, lingWidth);

    if (!(m_lineWidth >= lingWidth[0] && m_lineWidth <= lingWidth[1])) {
        m_lineWidth = (lingWidth[0] * 10);
    }
    glLineWidth(m_lineWidth);

    m_model.Init();

    return true;
}

void Polygon::DeInit() {
    m_model.DeInit();
    PolygonDrawer::DeInit();
}

void Polygon::Render() {
    Filter();
}

void Polygon::UpdatePolygon(const PolygonObject &object, const Viewport &viewport) {
    m_projection = glm::ortho((float) viewport.x, (float) viewport.x + (float) viewport.width,
                              (float) viewport.y, (float) viewport.y + (float) viewport.height);

    PolygonObject polygon = object;
    m_vertexCount = polygon.region.size();

    float ratioW = (float) viewport.width / polygon.canvasSize.w;
    float ratioH = (float) viewport.height / polygon.canvasSize.h;

    float textX = polygon.region[0].x * ratioW;
    float textY = (polygon.canvasSize.h - polygon.region[0].y) * ratioH;

    for (auto &point : polygon.region) {
        point.x *= ratioW;
        point.x += (float) viewport.x;
        point.y *= ratioH;
        point.y += (float) viewport.y;
    }

    float rotate = 0.0f;
    if (polygon.region.size() >= 2) {
        rotate = Math::CoordinatesDegree(polygon.region[0], polygon.region[polygon.region.size() - 1]);
    }

    VarSet(Polygon::var_fill, polygon.fill);
    VarSet(Polygon::var_color, polygon.color);

    m_model.Bind(m_shader->PositionAttributeLocation());
    m_model.UpdateVertex(polygon.region);

    if (!polygon.tag.empty()) {
        m_tag.text = std::move(polygon.tag);
        m_tag.displayViewport = viewport;
        m_tag.realViewport = viewport;
        m_tag.color = BlueColor;
        m_tag.bgColor = polygon.color;
        m_tag.fillBg = true;
        m_tag.position = {textX, textY - static_cast<float>(m_lineWidth)};
        m_tag.baseAdjust = false;
        m_tag.shadow = false;
        m_tag.rotate = rotate;
    }
}

void Polygon::loadShader() {
    m_shader = ResManager::Self()->LoadShader("polygon");
    assert(m_shader != nullptr);

    m_uniformLocProjection = glGetUniformLocation(m_shader->ProgramId(), "uProjection");
    m_uniformLocModel = glGetUniformLocation(m_shader->ProgramId(), "uModel");
    m_uniformLocColor = glGetUniformLocation(m_shader->ProgramId(), "uColor");
}

void Polygon::updateValue() {
    m_shader->SetFloat4(m_uniformLocColor, m_color);
    m_shader->SetMatrix4(m_uniformLocProjection, m_projection);
    m_shader->SetMatrix4(m_uniformLocModel, glm::mat4(1.0f));
}

void Polygon::registerVar() {
    m_varGroup->VarRegister(var_color, {
            [this](const Var &var) {
                m_color = var.ToFloat4();
            },
            Float4TypeTag{}
    });
    m_varGroup->VarRegister(var_fill, {
            [this](const Var &var) {
                m_fill = var.ToBoolean().b;
            },
            BoolTypeTag{}
    });
}

void Polygon::draw() {
    m_model.Use([fill = m_fill, count = m_vertexCount]() {
        if (fill) {
            glDrawArrays(GL_TRIANGLE_FAN, 0, count);
        } else {
            glDrawArrays(GL_LINE_LOOP, 0, count);
        }
    });

    if (!m_tag.Empty()) {
        Printer::Self()->Print(m_tag);
    }
}