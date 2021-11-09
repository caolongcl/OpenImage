//
// Created by caolong on 2019/7/1.
//

#include <render/egl/EGLCore.hpp>
#include <softarch/ComType.hpp>

using namespace clt;

#define CheckRet(ret) \
    if (!(ret)) {\
        Log::e(EGLCore::target,"CheckRet:%s", #ret);\
        return;}

#define CheckRetExit(ret) \
    if (!(ret)) {\
        Log::e(EGLCore::target,"CheckRetExit:%s", #ret);\
        exit(EXIT_FAILURE);}

#define EGL_NO_CONFIG (void *)0

EGLCore::EGLCore()
    : m_display(EGL_NO_DISPLAY),
      m_config(EGL_NO_CONFIG),
      m_context(EGL_NO_CONTEXT) {
}

EGLCore::EGLCore(EGLDisplay display, EGLConfig config, EGLContext context)
    : m_display(display), m_config(config), m_context(context) {
}

EGLCore::~EGLCore() noexcept {
  m_display = EGL_NO_DISPLAY;
  m_config = EGL_NO_CONFIG;
  m_context = EGL_NO_CONTEXT;
}

bool EGLCore::Init() {
  Log::v(target, "EGLCore::Init");

  CheckRetExit(createDisplay(m_display));
  CheckRetExit(chooseConfig(m_display, m_config));
  CheckRetExit(createContext(m_display, m_config, m_context));

  return true;
}

void EGLCore::DeInit() {
  Log::v(target, "EGLCore::DeInit");

  m_surfaces.clear();
  destroyContext();
}

void EGLCore::CreateWindowSurface(const std::string &name, ANativeWindow *window) {
  Log::v(target, "EGLCore::CreateWindowSurface");
  if (m_surfaces.find(name) != m_surfaces.end()) return;

  EGLSurface surface;
  CheckRet(createWindowSurface(window, m_display, m_config, surface));
  m_surfaces[name] = surface;
}

void EGLCore::CreateOffScreenSurface(const std::string &name) {
  Log::v(target, "EGLCore::CreateOffScreenSurface");
  if (m_surfaces.find(name) != m_surfaces.end()) return;

  EGLConfig config;
  EGLSurface surface;
  CheckRet(chooseOffScreenConfig(m_display, config));
  CheckRet(createOffScreenSurface(m_display, config, surface));
  m_surfaces[name] = surface;
}

void EGLCore::DestroyWindowSurface(const std::string &name) {
  if (m_surfaces.find(name) != m_surfaces.end()) {
    CheckRet(destroySurface(m_display, m_surfaces.at(name)));
    m_surfaces.erase(name);
  }
}

void EGLCore::ActiveContext(const std::string &name) {
  EGLSurface surface = EGL_NO_SURFACE;
  if (m_surfaces.find(name) != m_surfaces.end()) {
    surface = m_surfaces.at(name);
  }
  activeContext(m_display, surface, surface, m_context);
}

void EGLCore::DeActiveContext() {
  activeContext(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
}

void EGLCore::Update(const std::string &name) {
  if (m_surfaces.find(name) != m_surfaces.end()) {
    CheckRet(swapBuffer(m_display, m_surfaces.at(name)));
  }
}

void EGLCore::SetPresentationTime(const std::string &name, khronos_stime_nanoseconds_t nsecs) {
  if (m_surfaces.find(name) != m_surfaces.end()) {
    CheckRet(setPresentationTime(m_surfaces.at(name), nsecs));
  }
}

std::shared_ptr<EGLCore> EGLCore::SharedEGL() {
  Log::v(target, "EGLCore::SharedEGL");

  assert(m_display != EGL_NO_DISPLAY);
  assert(m_config != EGL_NO_CONFIG);
  assert(m_context != EGL_NO_CONTEXT);

  return std::make_shared<EGLCore>(m_display, m_config, m_context);
}

/**************************************************************************************************/
/**
 *
 * @param display
 * @return
 */
bool EGLCore::createDisplay(EGLDisplay &display) {
  if ((display = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY) {
    gles::CheckEglError("eglGetDisplay");
    return false;
  }

  EGLint majorVersion;
  EGLint minorVersion;
  if (eglInitialize(display, &majorVersion, &minorVersion) != EGL_TRUE) {
    gles::CheckEglError("eglInitialize");
    display = EGL_NO_DISPLAY;
    return false;
  }
  Log::d(target, "egl version {%d, %d}", majorVersion, minorVersion);

  printEglInfo();

  return true;
}

/**
 *
 * @param display
 * @param config
 * @return
 */
bool EGLCore::chooseConfig(EGLDisplay display, EGLConfig &config) {
  const EGLint attribs[] = {
      EGL_BUFFER_SIZE, 32,
      EGL_ALPHA_SIZE, 8,
      EGL_BLUE_SIZE, 8,
      EGL_GREEN_SIZE, 8,
      EGL_RED_SIZE, 8,
      EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
      EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
      EGL_RECORDABLE_ANDROID, 1,
      EGL_NONE};
  EGLint numConfigs;
  if (eglChooseConfig(display, attribs, &config, 1, &numConfigs) == EGL_FALSE) {
    gles::CheckEglError("eglChooseConfig");
    destroyContext();
    return false;
  }

  return true;
}

/**
 *
 * @param display
 * @param config
 * @param context
 * @return
 */
bool EGLCore::createContext(EGLDisplay display, EGLConfig config, EGLContext &context) {
  EGLint eglContextAttributes[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
  if ((context = eglCreateContext(display, config, context, eglContextAttributes)) ==
      EGL_NO_CONTEXT) {
    gles::CheckEglError("eglCreateContext");
    destroyContext();
    return false;
  }

  const unsigned char *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
  if (glslVersion != nullptr) {
    Log::i(target, "glsl version %s", glslVersion);
  }

  return true;
}

/**
 *
 * @param window
 * @param display
 * @param config
 * @param surface
 * @return
 */
bool EGLCore::createWindowSurface(ANativeWindow *window, EGLDisplay display,
                                  EGLConfig config, EGLSurface &surface) {
  EGLint format;

  if (eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format) != EGL_TRUE) {
    gles::CheckEglError("eglGetConfigAttrib");
    destroyContext();
    return false;
  }
  ANativeWindow_setBuffersGeometry(window, 0, 0, format);
  if ((surface = eglCreateWindowSurface(display, config, window, nullptr)) ==
      EGL_NO_SURFACE) {
    gles::CheckEglError("eglCreateWindowSurface");
    return false;
  }

  return true;
}

bool EGLCore::chooseOffScreenConfig(EGLDisplay display, EGLConfig &config) {
  const EGLint attribs[] = {
      EGL_BUFFER_SIZE, 32,
      EGL_ALPHA_SIZE, 8,
      EGL_BLUE_SIZE, 8,
      EGL_GREEN_SIZE, 8,
      EGL_RED_SIZE, 8,
      EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
      EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
      EGL_RECORDABLE_ANDROID, 1,
      EGL_NONE};
  EGLint numConfigs;
  if (eglChooseConfig(display, attribs, &config, 1, &numConfigs) == EGL_FALSE) {
    gles::CheckEglError("chooseOffScreenConfig");
    return false;
  }

  return true;
}

bool EGLCore::createOffScreenSurface(EGLDisplay display,
                                     EGLConfig config,
                                     EGLSurface &surface) {
  EGLint attribs[] = {EGL_WIDTH, 0,
                      EGL_HEIGHT, 0,
                      EGL_LARGEST_PBUFFER, EGL_TRUE,
                      EGL_NONE};
  if ((surface = eglCreatePbufferSurface(display, config, attribs)) == EGL_NO_SURFACE) {
    gles::CheckEglError("createOffScreenSurface");
    return false;
  }

  return true;
}

/**
 *
 * @param display
 * @param draw
 * @param read
 * @param context
 * @return
 */
bool EGLCore::activeContext(EGLDisplay display, EGLSurface draw,
                            EGLSurface read, EGLContext context) {
  if (eglMakeCurrent(display, draw, read, context) != EGL_TRUE) {
    gles::CheckEglError("eglMakeCurrent");
    return false;
  }

  return true;
}

/**
 *
 * @param display
 * @param surface
 * @return
 */
bool EGLCore::swapBuffer(EGLDisplay display, EGLSurface surface) {
  if (eglSwapBuffers(display, surface) != EGL_TRUE) {
    gles::CheckEglError("eglSwapBuffers");
    return false;
  }

  return true;
}

/**
 *
 * @param context
 * @param surface
 * @param what
 * @param value
 * @return
 */
bool EGLCore::querySurfaceAttrb(EGLContext context, EGLSurface surface, EGLint what,
                                EGLint &value) {
  if (eglQuerySurface(context, surface, what, &value) != EGL_TRUE) {
    gles::CheckEglError("eglQuerySurface");
    Log::d(target, "what %d, value %d", what, value);
    return false;
  }
  return true;
}

/**
 *
 * @param display
 * @param surface
 * @return
 */
bool EGLCore::destroySurface(EGLDisplay display, EGLSurface surface) {
  if (surface == EGL_NO_SURFACE)
    return true;

  eglDestroySurface(display, surface);
  return true;
}

/**
 *
 */
void EGLCore::destroyContext() {
  if (m_display != EGL_NO_DISPLAY) {
    eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, m_context);
    if (m_context != EGL_NO_CONTEXT) {
      eglDestroyContext(m_display, m_context);
    }
    eglTerminate(m_display);
    Log::i(target, "EGLCore::destroyContext");
  }

  m_display = EGL_NO_DISPLAY;
  m_context = EGL_NO_CONTEXT;
}

/**
 *
 * @param surface
 * @param nsecs
 * @return
 */
bool EGLCore::setPresentationTime(EGLSurface &surface, khronos_stime_nanoseconds_t nsecs) {
  return eglPresentationTimeANDROID(m_display, surface, nsecs) == EGL_TRUE;
}

/**
 * 打印EGL环境信息
 */
void EGLCore::printEglInfo() {
  if (m_display == EGL_NO_DISPLAY) {
    return;
  }
  Log::i(target, "########################## egl display info ##########################");
  Log::i(target, "%s", eglQueryString(m_display, EGL_VENDOR));
  Log::i(target, "%s", eglQueryString(m_display, EGL_VERSION));
  Log::i(target, "%s", eglQueryString(m_display, EGL_EXTENSIONS));
  Log::i(target, "######################################################################");
}