//
// Created by caolong on 2020/6/7.
//

#include <res/ResManager.hpp>
#include <res/Printer.hpp>
#include <render/GLRender.hpp>
#include <render/Flow.hpp>
#include <softarch/Math.hpp>
#include <softarch/ComType.hpp>
#include <render/egl/EGLCore.hpp>
#include <render/copier/Copier.hpp>
#include <render/copier/OESCopier.hpp>
#include <render/copier/Recorder.hpp>
#include <render/copier/Drawer.hpp>
#include <render/copier/Capturer.hpp>
#include <render/filter/FilterPipe.hpp>
#include <softarch/PreviewControllerCallback.hpp>
#include <render/RenderObserver.hpp>
#include <process/base/ProcessPipe.hpp>
#include <process/facedetector/FaceDetector.hpp>
#include <render/copier/Surface.hpp>

using namespace clt;

GLRender::GLRender()
    : MsgHandler(target),
      m_oesCopier(new OESCopier()),
      m_copier(new Copier()),
      m_filterPipe(new FilterPipe()),
      m_drawer(new Drawer()),
      m_capture(new Capturer()),
      m_recorder(new Recorder()),
      m_renderObserver(new RenderObserver()),
      m_processPipe(new ProcessPipe()) {
}

bool GLRender::Init(InitParamType callback) {
  Log::v(target, "GLRender::Init");

  m_capture->Init(m_copier);
  m_recorder->Init(m_copier);
  m_drawer->Init(m_copier);

  m_renderObserver->Init(callback);
  m_filterPipe->Init(m_oesCopier, m_copier, m_processPipe);
  m_processPipe->Init();

  m_oesCopier->AddObserver(std::weak_ptr<Observer<OPreviewSize>>(m_renderObserver));
  m_oesCopier->AddObserver(std::weak_ptr<Observer<OPreviewSize>>(m_processPipe));

  Flow::Self()->AddMsgHandler(shared_from_this());

  return true;
}

void GLRender::DeInit() {
  Flow::Self()->RemoveMsgHandler(shared_from_this());

  m_processPipe->DeInit();
  m_filterPipe->DeInit();
  m_renderObserver->DeInit();

  m_drawer->DeInit();
  m_recorder->DeInit();
  m_capture->DeInit();

  Log::v(target, "GLRender::DeInit");
}

void GLRender::SetPreviewMode(int rotate, int ratio,
                              bool vFlip, bool hFlip,
                              int width, int height) {
  // 预览大小等
  m_oesCopier->SetSize(width, height)
      .SetRotation(rotate).SetRatio(ratio)
      .SetVFlip(vFlip).SetHFlip(hFlip)
      .Update();
}

void GLRender::Render() {
  m_filterPipe->Filter();
  m_recorder->Render();
  m_drawer->Render();
}

GLuint GLRender::GetPreviewTexId() const {
  return m_filterPipe->PreviewTexId();
}

void GLRender::SetSurface(int type, ANativeWindow *windows) {
  if (Constants::ValidSurfaceType(type) && windows != nullptr) {
    auto surface = std::make_shared<Surface>(
        Constants::SurfaceTypeName(type) + std::to_string(Utils::CurTimeMilli()));
    surface->Init(windows);

    switch (type) {
      case Constants::NATIVE_WINDOW_MAIN_TYPE:
        m_drawer->RegisterSurface(surface);
        break;
      case Constants::NATIVE_WINDOW_CAPTURE_TYPE:
        m_capture->RegisterSurface(surface);
        break;
      case Constants::NATIVE_WINDOW_RECORD_TYPE:
        m_recorder->RegisterSurface(surface);
        break;
      default:
        break;
    }
  }
}

void GLRender::SetSurfaceSize(int width, int height) {
  m_copier->SetSurfaceSize(width, height)
      .Update();
}

void GLRender::Capture() {
  m_capture->Render();
}

void GLRender::GetCaptureSize(int &width, int &height) {
  width = m_copier->GetPreviewViewport().width;
  height = m_copier->GetPreviewViewport().height;
}

void GLRender::Record(bool start, int fps, int bitrate) {
  m_recorder->Record(start, fps, bitrate);
}

bool GLRender::GetRecordState(int &fps, int &bitrate) {
  m_recorder->GetRecordParams(fps, bitrate);
  return m_recorder->Recording();
}

void GLRender::SetVar(const std::string &name, const Var &var) {
  if (name == OESCopier::var_vflip) {
    m_oesCopier->SetVFlip(var.ToBoolean().b).Update();
  } else if (name == OESCopier::var_hflip) {
    m_oesCopier->SetHFlip(var.ToBoolean().b).Update();
  } else if (name == OESCopier::var_ratio) {
    m_oesCopier->SetRatio(var.ToInteger().x).Update();
  } else {
    m_filterPipe->DispatchVar(name, var);
  }
}

void GLRender::EnableFilter(const std::string &name, bool enable) {
  m_filterPipe->EnableFilter(name, enable);
}

void GLRender::EnableProcess(const std::string &name, bool enable) {
  m_processPipe->EnableProcess(name, enable);
}

void GLRender::ClearFilters() {
  m_filterPipe->ClearFilters();
}

void GLRender::ClearProcessTasks() {
  m_processPipe->ClearProcessTasks();
}

void GLRender::UpdateTargetPosition(int x, int y) {
  if (m_copier->PositionInDisplayRegion(x, y)) {
    m_copier->AdjustPositionToDisplayRegion(x, y);
    m_filterPipe->DispatchVar(FilterPipe::var_target_position, Float{(float) x, (float) y});

    float ratiox =
        (float) m_copier->GetPreviewViewport().width /
        (float) m_copier->GetDisplayViewport().width;
    float ratioy =
        (float) m_copier->GetPreviewViewport().height /
        (float) m_copier->GetDisplayViewport().height;
    m_filterPipe->DispatchVar(FilterPipe::var_position_ratio, Float{ratiox, ratioy});
  }
}

void GLRender::OnMsgHandle(const Msg &msg) {
  switch (msg.type) {
    case Msg::Type::MSG_POST_INFO:
      m_infoToJava = msg.what;
      m_renderObserver->OnUpdate(ObserverToJava());
      break;
    default:
      break;
  }
}

const std::string &GLRender::GetInfoToJava() const {
  return m_infoToJava;
}