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

  struct ViewParams {
    int width{0};
    int height{0};
    Viewport viewport{0, 0, 0, 0};

    bool operator==(const ViewParams &t) {
      return t.width != width || t.height != height;
    }

    bool operator!=(const ViewParams &t) {
      return !(*this == t);
    }

    bool Valid() const {
      return width > 0 && height > 0;
    }
  };

  using PreviewParams = struct ViewParams;

  /**
   * 显示宽高，在 display 坐标系
   * 坐标系原点就是屏幕左下角，X 正向朝右，Y 正向朝上
   */
  using DisplayParams = struct ViewParams;

  class Shader;

  class Texture;

  class SquareModel;

  class BaseModel;

  class TextMsgData;

  class FpsStatics;

  /**
   * 将从 FilterPipe 中输出的纹理拷贝到录制、拍照纹理上，或者绘制在屏幕上
   */
  class Copier final : public IComFunc<>,
                       public Observer<OPreviewSize>,
                       public MsgHandler {
  ClassDeclare(Copier)
  MsgDefine(process)
  MsgDefine(process_info)
  public:
    Copier();

    ~Copier() = default;

    bool Init() override;

    void DeInit() override;

    /**
     * 显示大小改变后更新 m_displayParams
     */
    void Update();

    /**
     * 预览纹理大小改变后更新 m_previewParams m_displayParams
     * @param t
     */
    void OnUpdate(OPreviewSize &&t) override;

    /**
     * 设置显示大小
     * @param width
     * @param height
     * @return
     */
    Copier &SetSurfaceSize(int width, int height);

    /**
     * 判断屏幕坐标系下的点是否在 display 坐标系内
     * Android 屏幕坐标系原点在左上角，X 正向朝右，Y 正向朝下，故判断时需要转化为  display 坐标系下的点
     * @param x 屏幕坐标系下
     * @param y 屏幕坐标系下
     * @return
     */
    bool PositionInDisplayRegion(int x, int y) const;

    /**
     * 将屏幕坐标系下的点转换到 display 坐标系
     * @param x
     * @param y
     */
    void AdjustPositionToDisplayRegion(int &x, int &y) const;

    /**
     * 把输入纹理拷贝到拍摄和录制的 Surface 上
     * @param viewport
     */
    void CopyPreFrame(const Viewport &viewport);

    /**
     * 在输入纹理上进行额外绘制，
     * 比如绘制文本，模型，多边形等，并最终拷贝到输出纹理
     */
    void Copy();

    /**
     * 把输出纹理拷贝到屏幕 Surface 上显示
     * @param viewport
     */
    void CopyPostFrame(const Viewport &viewport);

    void SetInput(std::shared_ptr<Texture> input);

    void SetOutput(std::shared_ptr<Texture> output);

    std::shared_ptr<Texture> &GetOutput();

    const Viewport &GetDisplayViewport() const;

    const Viewport &GetPreviewViewport() const;

    void OnMsgHandle(const Msg &msg) override;

  private:
    void update();

    /**
     * 根据预览大小和显示大小以计算出完整显示预览图像时的 viewport
     * @param previewParams
     * @param displayParams
     */
    static void update(PreviewParams &previewParams, DisplayParams &displayParams);

    /**
     * 判断点 (x, y) 是否在 display 区域内
     * @param x
     * @param y
     * @return
     */
    bool inDisplayRegion(int x, int y) const;

    /**
     * 绘制调试信息，文本，多边形、3D 模型等
     */
    void postRender();

    void renderText();

    void renderPolygon();

    void renderBaseModel();

    /**
     * 绘制调试信息
     */
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

    // 预览纹理参数
    PreviewParams m_previewParams;

    // 显示参数
    DisplayParams m_displayParams;
  };

}