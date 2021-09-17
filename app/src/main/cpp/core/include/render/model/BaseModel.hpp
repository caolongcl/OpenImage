//
// Created by caolong on 2021/3/19.
//

#pragma once

#include <softarch/IComFunc.hpp>
#include <softarch/GLUtils.hpp>
#include <utils/Utils.hpp>
#include <softarch/Math.hpp>
#include <softarch/Variable.hpp>
#include <softarch/Msg.hpp>

namespace clt {
    class Shader;

    /**
     * 预置的几何体模型
     */
    class BaseModel final : public IComFunc<>,
                            public MsgHandler,
                            public IVariableSet<glm::mat4>,
                            public IVariableSet<Color> {
    ClassDeclare(BaseModel)
    VarDeclare(color)
    VarDeclare(projection_matrix)
    VarDeclare(view_matrix)
    VarDeclare(model_matrix)

    MsgDefine(marker_ar)

    public:
        struct Cube {
            GLuint cubeEbo{0};
            int count{0};
        };
      struct Pyramid {
        GLuint pyramidEbo{0};
        int count{0};
      };
    public:
        BaseModel();

        ~BaseModel() = default;

        bool Init() override;

        void DeInit() override;

        void SetVariable(const std::string &varName, const glm::mat4 &var) override;

        void SetVariable(const std::string &varName, const Color &var) override;

        void OnMsgHandle(const Msg &msg) override;

        void Render(const Viewport &viewport);

    private:
        void loadModels();

        void loadShader();

        void updateValue();

        void registerVar();

        void draw();

        void updateValue(const glm::mat4 &_projection,
                         const glm::mat4 &_view,
                         const glm::mat4 &_model,
                         const Color &_color);

        static std::vector<float> createCubeVertex();

        static std::vector<unsigned short> createCubeIndex();

        static std::vector<float> createPyramidVertex();

        static std::vector<unsigned short> createPyramidIndex();

        /**
         * 构造摄像机内参矩阵，得到投影矩阵
         * @param width 绘制区域宽
         * @param height 绘制区域高
         * @param near 近平面
         * @param far 远平面
         * @param projection 模拟实际的投影矩阵
         */
        static void getRealWorldProjection(int width, int height, float near, float far, glm::mat4 &projection);

        // 测试用

        void initBase();

        void updateBase(const Viewport &viewport);

        void updateViewport(const Viewport &viewport);

    private:
        std::shared_ptr<Shader> m_shader;
        glm::mat4 m_projectionMatrix;
        glm::mat4 m_viewMatrix;
        glm::mat4 m_modelMatrix;
        Color m_color;

        VariableGroup<glm::mat4> m_mat4Vars;
        VariableGroup<Color> m_colorVars;

        std::unordered_map<std::string, std::shared_ptr<BaseModelMsgData>> m_models;

        GLuint m_vao{0};
        GLuint m_vbo{0};
        Cube m_cube;
        Pyramid m_pyramid;

        GLint m_uniformLocProjection{-1};
        GLint m_uniformLocView{-1};
        GLint m_uniformLocModel{-1};
        GLint m_uniformLocColor{-1};

    private:
        static const int s_vertexCoordsPer = 6;
        static const int s_positionPer = 3;
        static const int s_colorPer = 3;
    };
}
