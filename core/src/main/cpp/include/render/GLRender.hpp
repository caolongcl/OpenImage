//
// Created by caolong on 2020/6/7.
//

#pragma once

#include <softarch/IComFunc.hpp>
#include <softarch/GLUtils.hpp>
#include <softarch/VarType.hpp>
#include <utils/Utils.hpp>
#include <softarch/Msg.hpp>

namespace clt {

  class FilterPipe;

  class EGLCore;

  class OESCopier;

  class Copier;

  class Drawer;

  class Capturer;

  class Recorder;

  class PreviewControllerCallback;

  class RenderObserver;

  class Flow;

  class ProcessPipe;

  /**
   * 控制整个渲染流程
   */
  class GLRender final
    : public IComFunc<std::shared_ptr<PreviewControllerCallback>>,
      public MsgHandler {
  ClassDeclare(GLRender)
  MsgDefine(post_info)

  private:
    using InitParamType = std::shared_ptr<PreviewControllerCallback>;

  public:
    GLRender();

    ~GLRender() = default;

    /**
     * 初始化，包含PreviewControllerCallback，将内部消息回调给PreviewController
     * @return
     */
    bool Init(InitParamType) override;

    void DeInit() override;

    void SetPreviewMode(int rotate, int ratio,
                        bool vFlip, bool hFlip,
                        int width, int height);

    /**
     * 渲染一帧
     */
    void Render();

    /**
     * 相机预览纹理ID
     * @return
     */
    GLuint GetPreviewTexId() const;

    void SetSurface(int type, ANativeWindow *windows);

    void SetSurfaceSize(int width, int height);

    void Capture();

    void Record(bool start, int fps, int bitrate);

    bool GetRecordState(int &fps, int &bitrate);

    void GetCaptureSize(int &width, int &height);

    void SetVar(const std::string &name, const Var &var);

    void EnableFilter(const std::string &name, bool enable);

    void EnableProcess(const std::string &name, bool enable);

    void ClearFilters();

    void ClearProcessTasks();

    void UpdateTargetPosition(int x, int y);

    void OnMsgHandle(const Msg &msg) override;

    const std::string &GetInfoToJava() const;

  private:
    /**
     * 将采集到的OES纹理转为RGBA的纹理便于后续处理
     */
    std::shared_ptr<OESCopier> m_oesCopier;

    /**
     * 将处理过的纹理绘制到的各个目标上
     */
    std::shared_ptr<Copier> m_copier;

    /**
     * 滤镜处理管道
     */
    std::shared_ptr<FilterPipe> m_filterPipe;

    /**
     * 将Copier输出绘制到屏幕上
     */
    std::shared_ptr<Drawer> m_drawer;

    /**
     * 拍照，将Copier输出绘制到拍照的surface上
     */
    std::shared_ptr<Capturer> m_capture;

    /**
     * 录制, 将Copier输出绘制到录制的surface上
     */
    std::shared_ptr<Recorder> m_recorder;

    /**
     * 代理PreviewControllerCallback入口
     */
    std::shared_ptr<RenderObserver> m_renderObserver;

    /**
     * 实时图像处理管道
     */
    std::shared_ptr<ProcessPipe> m_processPipe;

    /**
     * 缓存需要回调给 Java 层的信息，用于 UI 显示
     */
    std::string m_infoToJava;
  };

}