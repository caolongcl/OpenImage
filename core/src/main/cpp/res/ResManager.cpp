//
// Created by caolong on 2020/8/3.
//

#include <utils/Utils.hpp>
#include <render/Flow.hpp>
#include <res/Printer.hpp>
#include <res/ResManager.hpp>
#include <res/Shader.hpp>
#include <res/font/Font.hpp>
#include <dirent.h>
#include <cstdio>

using namespace clt;

// 文件系统别名空间
namespace fs = std::__fs::filesystem;

std::mutex ResManager::s_mutex;

bool ResManager::Init() {
  Log::v(Log::RES_TAG, "ResManager::Init");

  m_shaders.Init();
  m_fonts.Init();

  m_calibrateData = {{m_realWorldParam.boardSize.w,       m_realWorldParam.boardSize.h},
                     {m_realWorldParam.boardSquareSize.w, m_realWorldParam.boardSquareSize.h},
                     {m_realWorldParam.markerSize.w,      m_realWorldParam.markerSize.h}};

  Printer::Self()->Init();

  return true;
}

void ResManager::DeInit() {
  Printer::Self()->DeInit();

  m_shaders.DeInit();
  m_fonts.DeInit();

  Log::v(Log::RES_TAG, "ResManager::DeInit");
}

void ResManager::ScanAndLoad(const std::string &path) {
  m_resPath = path;

  scanResFiles(m_resPath);
  loadFonts();
}

std::string ResManager::GetResAbsolutePath(const std::string &file) const {
  return m_resPath + file;
}

void ResManager::RegisterBaseDir(const std::string &path) {
  m_basePath = path;
}

void ResManager::RegisterFunctionDir(const std::string &path) {
  m_functionPath = path;
}

const std::string &ResManager::GetBaseAbsolutePath() const {
  return m_basePath;
}

const std::string &ResManager::GetFunctionAbsolutePath() const {
  return m_functionPath;
}

std::shared_ptr<Shader> ResManager::LoadShader(const std::string &name) {
  if (name == s_oes) {
    m_shaders.Add(s_oes, makeShader(s_oesVertex, s_oesFragment, ""));
  } else if (name == s_copier) {
    m_shaders.Add(s_copier, makeShader(s_copierVertex, s_copierFragment, ""));
  }

  auto shader = m_shaders.Get(name);
  if (shader != nullptr) {
    return shader;
  }

  assert(!m_resPath.empty());
  Log::v(Log::RES_TAG, "load shader %s", name.c_str());

  std::string file = name + "_shader.yml";
  if (std::find(m_resFiles.cbegin(), m_resFiles.cend(), file) == m_resFiles.cend()) {
    Log::w(Log::RES_TAG, "no shader %s ", name.c_str());
    return nullptr;
  } else {
    std::string root = m_resPath + "/shaders/";
    YamlParse yaml(root + file);
    m_shaders.Add(getShaderName(yaml), makeShader(yaml));
    return m_shaders.Get(name);
  }
}

std::shared_ptr<Font> ResManager::GetGUIFont() {
  return m_fonts.Get(ArialFont::sName);
}

void ResManager::SaveMatImage(const std::string &fileName, const cv::Mat &image) {
  static long lastTime = -1;

  if (lastTime == -1) {
    lastTime = Utils::CurTimeMilli();
  } else {
    long curTime = Utils::CurTimeMilli();
    if (curTime - lastTime > 1000) {
      SaveMatImageImmediate(fileName, image);
      lastTime = curTime;
    }
  }
}

void ResManager::SaveMatImageImmediate(const std::string &fileName, const cv::Mat &image) {
  std::string file = ResManager::Self()->GetFunctionAbsolutePath() + "/";
  file += fileName + ".jpg";
  cv::imwrite(file, image);
  Log::v(Log::RES_TAG, "save mat picture %s", file.c_str());
}

std::string ResManager::GetCameraParamsFile() const {
  return m_functionPath + "/" + "camera_params.yml";
}

void ResManager::UpdateResParamsBufRatio(float ratio) {
  std::lock_guard<std::mutex> locker(s_mutex);
  m_realWorldParam.bufRatio = ratio;
}

void ResManager::loadFonts() {
  assert(!m_resPath.empty());

  std::string root = m_resPath + "/fonts";

  registerFonts(ArialFont::sName, std::make_shared<ArialFont>(root));
}

void ResManager::clearOldCalibrateImages() {
  DIR *dirItem;
  struct dirent *dir;

  if ((dirItem = opendir(m_functionPath.c_str())) != nullptr) {
    while ((dir = readdir(dirItem)) != nullptr) {
      std::string dirName(dir->d_name);
      auto pos = dirName.find_last_of('.');
      if (dirName != "." && dirName != ".." && dirName.substr(pos) == ".jpg") {
        Log::d(Log::PROCESSOR_TAG, "remove old calibrate file %s", dirName.c_str());
        std::string fullPath = m_functionPath + "/" + dirName;
        std::remove(fullPath.c_str());
      }
    }

    closedir(dirItem);
  }
}

const CalibrateData &ResManager::LoadCalibrateParams() {
  std::lock_guard<std::mutex> locker(s_mutex);

  std::string fileName = m_functionPath + "/" + "calibrate_params.yml";
  std::ifstream f(fileName.c_str());
  if (!f.good()) {
    f.close();
    Log::w(Log::RES_TAG, "no calibrate_params.yml, load default");

    YamlCreator creator(fileName);
    creator.Emit() << CalibrateData::Encode(m_calibrateData);
    creator.Save();
  } else {
    f.close();

    // 解析校正工具参数
    bool got = false;
    YamlParse parse(fileName);
    if (parse.Valid()) {
      got = CalibrateData::Decode(parse.GetNode(), m_calibrateData);
    }

    if (!got) {
      Log::e(Log::RES_TAG, "parse calibrate_params.yml failed, load default");
    } else {
      Log::d(Log::RES_TAG, "parse calibrate_params.yml success");
    }
  }

  return m_calibrateData;
}

void ResManager::SaveCalibrateParams(Integer2 boardSize,
                                     Float2 boardSquareSize, Float2 markerSize) {
  if (boardSize.w != m_calibrateData.GetBoardSize().w
      || boardSize.h != m_calibrateData.GetBoardSize().h
      || boardSquareSize.w != m_calibrateData.GetBoardSquareSize().w
      || boardSquareSize.h != m_calibrateData.GetBoardSquareSize().h
      || markerSize.w != m_calibrateData.GetMarkerSize().w
      || markerSize.h != m_calibrateData.GetMarkerSize().h) {
    Log::d(Log::RES_TAG, "SaveCalibrateParams calibrate_params changed");

    std::lock_guard<std::mutex> locker(s_mutex);

    m_calibrateData = {{boardSize.w,       boardSize.h},
                       {boardSquareSize.w, boardSquareSize.h},
                       {markerSize.w,      markerSize.h}};

    YamlCreator creator(m_functionPath + "/" + "calibrate_params.yml");
    creator.Emit() << CalibrateData::Encode(m_calibrateData);
    creator.Save();

    // 清除旧参数拍摄的校正图片
    clearOldCalibrateImages();
  } else {
    Log::w(Log::RES_TAG, "SaveCalibrateParams calibrate_params unchanged");
  }
}

std::shared_ptr<Shader> ResManager::makeShader(const YamlParse &yaml) {
  std::shared_ptr<Shader> shader = std::make_shared<Shader>(
      getVertexShaderSrc(yaml),
      getFragmentShaderSrc(yaml),
      getComputeShaderSrc(yaml)
  );
  shader->Init();
  return shader;
}

std::shared_ptr<Shader> ResManager::makeShader(const std::string &vertexSrc,
                                               const std::string &fragmentSrc,
                                               const std::string &computeSrc) {
  std::shared_ptr<Shader> shader = std::make_shared<Shader>(
      vertexSrc, fragmentSrc, computeSrc
  );
  shader->Init();
  return shader;
}

std::string ResManager::getShaderName(const YamlParse &yaml) {
  return yaml.Parse<std::string>("name");
}

std::string ResManager::getVertexShaderSrc(const YamlParse &yaml) {
  return getShaderSrc(yaml, "vertex");
}

std::string ResManager::getComputeShaderSrc(const YamlParse &yaml) {
  return getShaderSrc(yaml, "compute");
}

std::string ResManager::getFragmentShaderSrc(const YamlParse &yaml) {
  return getShaderSrc(yaml, "fragment");
}

std::string ResManager::getShaderSrc(const YamlParse &yaml, const std::string &type) {
  if (!yaml.HasToken(type)) {
    return "";
  }

  auto src = yaml.Parse<std::string>(type);
  src = Utils::Trim(src);

  std::regex reg("_#1_");
  src = std::regex_replace(src.c_str(), reg, "#");

  std::regex reg1("_#2_");
  src = std::regex_replace(src.c_str(), reg1, ":");

  Log::n(Log::RES_TAG, "src %s\n%s", type.c_str(), src.c_str());

  return src;
}

bool ResManager::scanResFiles(const std::string &rootPath) {
#if 0
  fs::path root(rootPath);
  if (fs::exists(root)) {
    fs::directory_entry rootEntry(root);
    if (rootEntry.status().type() == fs::file_type::directory) {
      for (auto &it : fs::directory_iterator(rootEntry)) {
        if (it.is_regular_file()) {
          m_resFiles.emplace_back(it.path().filename().c_str());
          Log::v(Log::RES_TAG, "res file %s", it.path().filename().c_str());
        } else if (it.is_directory()) {
          scanResFiles(it.path());
        }
      }
    }
  }

  Log::e(Log::RES_TAG, "no exist resource path %d", root.string().c_str());
#else
  m_resFiles.emplace_back("arial.ttf");

  m_resFiles.emplace_back("oes_shader.yml");
  m_resFiles.emplace_back("default_shader.yml");
  m_resFiles.emplace_back("font_shader.yml");
  m_resFiles.emplace_back("base_shader.yml");
  m_resFiles.emplace_back("blur_shader.yml");
  m_resFiles.emplace_back("color_invert_shader.yml");
  m_resFiles.emplace_back("exposure_shader.yml");
  m_resFiles.emplace_back("gamma_shader.yml");
  m_resFiles.emplace_back("grayscale_shader.yml");
  m_resFiles.emplace_back("hue_shader.yml");
  m_resFiles.emplace_back("monochrome_shader.yml");
  m_resFiles.emplace_back("mosaic_shader.yml");
  m_resFiles.emplace_back("opacity_shader.yml");
  m_resFiles.emplace_back("rgb_shader.yml");
  m_resFiles.emplace_back("sharpen_shader.yml");
  m_resFiles.emplace_back("time_color_shader.yml");
  m_resFiles.emplace_back("whirlpool_shader.yml");
  m_resFiles.emplace_back("white_balance_shader.yml");
  m_resFiles.emplace_back("fire_shader.yml");
  m_resFiles.emplace_back("rgba2gray_shader.yml");
  m_resFiles.emplace_back("polygon_shader.yml");
  m_resFiles.emplace_back("base_model_shader.yml");

  m_resFiles.emplace_back("haarcascade_frontalface_alt.xml");

  m_baseShaderFiles.emplace_back("oes_shader.yml");
  m_baseShaderFiles.emplace_back("default_shader.yml");
  m_otherShaderFiles.emplace_back("font_shader.yml");
#endif
  return false;
}

void ResManager::registerFonts(const std::string &name, std::shared_ptr<Font> font) {
  if (font == nullptr)
    return;

  Log::d(Log::RES_TAG, "ResManager::registerFonts %s", name.c_str());

  font->Init();
  font->Load();

  if (!font->Loaded()) {
    Log::w(Log::RES_TAG,
           "ResManager::registerFonts %s failed",
           name.c_str());

    font->DeInit();
    font = nullptr;
    return;
  }

  m_fonts.Add(name, font);

  Log::v(Log::RES_TAG,
         "ResManager::registerFonts %s success", name.c_str());
}

/**************************************************************************************************/
const char *ResManager::s_oes = "oes";
const char *ResManager::s_copier = "copier";

const char *ResManager::s_oesVertex =
    "#version 300 es\n"
    "\n"
    "  layout(location = 0) in vec4 aPosition;\n"
    "  layout(location = 1) in vec2 aTexCoords;\n"
    "  out vec2 vTexCoords;\n"
    "\n"
    "  void main() {\n"
    "  vTexCoords = vec2(aTexCoords.s, 1.0-aTexCoords.t);\n"
    "  gl_Position = aPosition;\n"
    "  }";
const char *ResManager::s_oesFragment =
    "#version 300 es\n"
    "\n"
    "  #extension GL_OES_EGL_image_external_essl3 : require\n"
    "\n"
    "  precision mediump float;\n"
    "  uniform samplerExternalOES uTexSampler;\n"
    "\n"
    "  in highp vec2 vTexCoords;\n"
    "  layout(location = 0) out vec4 oFragColor;\n"
    "  layout(location = 1) out vec4 oOtherFragColor;\n"
    "\n"
    "  void main() {\n"
    "  oFragColor = texture(uTexSampler, vTexCoords);\n"
    "  oOtherFragColor = oFragColor;\n"
    "  }";
const char *ResManager::s_copierVertex =
    "#version 300 es\n"
    "\n"
    "  layout(location = 0) in vec4 aPosition;\n"
    "  layout(location = 1) in vec2 aTexCoords;\n"
    "  out vec2 vTexCoords;\n"
    "\n"
    "  void main() {\n"
    "  vTexCoords = aTexCoords;\n"
    "  gl_Position = aPosition;\n"
    "  }";
const char *ResManager::s_copierFragment =
    "#version 300 es\n"
    "\n"
    "  precision mediump float;\n"
    "  uniform sampler2D uTexSampler;\n"
    "  in highp vec2 vTexCoords;\n"
    "  out vec4 oFragColor;\n"
    "\n"
    "  void main() {\n"
    "  oFragColor = texture(uTexSampler, vTexCoords);\n"
    "  }";