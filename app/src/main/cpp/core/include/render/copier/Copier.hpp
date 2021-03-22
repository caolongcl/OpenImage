//
// Created by caolong on 2020/7/20.
//

#pragma once

#include <softarch/IComFunc.hpp>
#include <softarch/GLUtils.hpp>
#include <softarch/Constants.hpp>
#include <softarch/ComType.hpp>
#include <softarch/Observer.hpp>
#include <softarch/Task.hpp>
#include <res/Printer.hpp>
#include <utils/Utils.hpp>
#include <render/polygon/Polygon.hpp>
#include <softarch/IMsg.hpp>
#include <softarch/Msg.hpp>

namespace clt {

    /**
     * 从相机预览纹理中拷贝
     */
    struct SurfaceParams {
        int previewW{0};
        int previewH{0};
        int surfaceW{0};
        int surfaceH{0};

        Viewport surfaceViewport{0, 0, 0, 0};
        Viewport previewViewport{0, 0, 0, 0};

        bool operator==(const SurfaceParams &t) {
            return t.previewW != previewW || t.previewH != previewH
                   || t.surfaceW != surfaceW || t.surfaceH != surfaceH;
        }

        bool operator!=(const SurfaceParams &t) {
            return !(*this == t);
        }

        bool Valid() const {
            return previewW > 0 && previewH > 0 && surfaceW > 0 && surfaceH > 0;
        }
    };

    /**
     * 统计帧率
     */
    struct FpsStatics : public IComFunc<> {
        FpsStatics() : m_lastFrameTime(-1),
                       m_minFps(9999),
                       m_maxFps(0),
                       m_avgFps(0),
                       m_framesCount(-1),
                       m_framesStartPeriod(-1),
                       m_fps("") {}

        virtual ~FpsStatics() = default;

        bool Init() override { return true; }

        void DeInit() override {
            m_lastFrameTime = -1;
            m_minFps = 9999;
            m_maxFps = 0;
            m_avgFps = 0;
            m_framesCount = -1;
            m_framesStartPeriod = -1;
            m_fps = "";
        }

        void UpdateStatistics();

        long m_lastFrameTime;
        int m_minFps;
        int m_maxFps;
        int m_avgFps;
        long m_framesCount;
        long m_framesStartPeriod;

        std::string m_fps;
    };

    class Shader;

    class Texture;

    class SquareModel;

    class BaseModel;

    class TextMsgData;


    class Copier final : public IComFunc<>,
                         public Observer<OPreviewSize>,
                         public MsgHandler {
    ClassDeclare(Copier)
    MsgDefine(detect_face)
    MsgDefine(detect_face_info)
    MsgDefine(detect_marker)
    MsgDefine(detect_marker_info)
    public:
        Copier();

        ~Copier() = default;

        bool Init() override;

        void DeInit() override;

        void Update();

        void OnUpdate(OPreviewSize &&t) override;

        Copier &SetSurfaceSize(int width, int height);

        bool PositionInSurface(int x, int y) const;

        void AdjustPositionsToSurface(int &x, int &y) const;

        void Copy();

        void CopyPreFrame(const Viewport &viewport);

        void CopyPostFrame(const Viewport &viewport);

        void SetInput(std::shared_ptr<Texture> input);

        void SetOutput(std::shared_ptr<Texture> output);

        std::shared_ptr<Texture> &GetOutput();

        const Viewport &GetSurfaceViewport() const;

        const Viewport &GetPreviewViewport() const;

        const SurfaceParams &GetSurfaceParams() const;

        void OnMsgHandle(const Msg &msg) override;

    private:
        void update();

        static void update(SurfaceParams &params);

        bool inSurface(int x, int y) const;

        void postRender();

        void renderText();

        void renderPolygon();

        void renderBaseModel();

        void setTextForDebug();

    private:
        std::shared_ptr<Shader> m_shader;
        std::shared_ptr<Texture> m_input;
        std::shared_ptr<Texture> m_output;
        std::shared_ptr<SquareModel> m_square;

        // 绘制多边形
        std::shared_ptr<Polygon> m_polygon;

        // 绘制基本几何体
        std::shared_ptr<BaseModel> m_baseModels;

        // 帧率统计
        std::shared_ptr<FpsStatics> m_fpsStatics;

        // 处理绘制多边形消息，如人脸检测
        std::unordered_map<std::string, std::shared_ptr<PolygonMsgData>> m_polygonTasks;

        // 显示文本
        std::unordered_map<std::string, std::shared_ptr<TextMsgData>> m_textTasks;

        // 调试文本信息
        std::vector<Printer::PrintTask> m_debugTexts;

        // 预览图像和显示 surface 大小参数
        SurfaceParams m_surfaceParams;
    };

}