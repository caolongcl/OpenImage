//
// Created by caolong on 2021/11/17.
//

#include <process/facedetector/FaceLandmark.hpp>
#include <utils/Log.hpp>

using namespace clt;

FaceLandmark::FaceLandmark()
  : m_useGPU(true) {

}


bool FaceLandmark::Init(const ModelNames &modelNames, bool useGPU) {
  bool hasGPU = ncnn::get_gpu_count() > 0;
  m_useGPU = useGPU && hasGPU;

  Log::d(target, "has gpu %d, use gpu %d", hasGPU, m_useGPU);

  m_faceNet.opt.use_vulkan_compute = m_useGPU;
  m_faceNet.opt.use_fp16_arithmetic = true;
  m_faceNet.load_param(modelNames.faceParam.c_str());
  m_faceNet.load_model(modelNames.faceBin.c_str());

  m_landmarkNet.opt.use_vulkan_compute = m_useGPU;  // gpu
  m_landmarkNet.opt.use_fp16_arithmetic = true;  // fp16运算加速
  m_landmarkNet.load_param(modelNames.landmarkParam.c_str());
  m_landmarkNet.load_model(modelNames.landmarkBin.c_str());

  return true;
}

void FaceLandmark::DeInit() {
  m_faceNet.clear();
  m_landmarkNet.clear();
}

std::vector<FaceLandmark::Face> FaceLandmark::Detect(const cv::Mat &image) {
  int inWidth = image.cols;
  int inHeight = image.rows;

  ncnn::Mat in = ncnn::Mat::from_pixels_resize(image.data, ncnn::Mat::PIXEL_RGB, inWidth, inHeight,
                                               s_detectorSizeWidth, s_detectorSizeHeight);

  float norm[3] = {1 / 255.f, 1 / 255.f, 1 / 255.f};
  float mean[3] = {0, 0, 0};
  in.substract_mean_normalize(mean, norm);

  auto ex = m_faceNet.create_extractor();
  ex.set_light_mode(true);
  ex.set_num_threads(4);
  if (m_useGPU) {
    ex.set_vulkan_compute(m_useGPU);
  }
  ex.input("data", in);
  ncnn::Mat out;
  ex.extract("output", out);

  std::vector<Face> faceList;
  for (int i = 0; i < out.h; i++) {
    float x1, y1, x2, y2, score, label;
    float pw, ph, cx, cy;
    const float *values = out.row(i);

    x1 = values[2] * (float) inWidth;
    y1 = values[3] * (float) inHeight;
    x2 = values[4] * (float) inWidth;
    y2 = values[5] * (float) inHeight;

    pw = x2 - x1;
    ph = y2 - y1;
    cx = x1 + 0.5f * pw;
    cy = y1 + 0.5f * ph;

    x1 = cx - 0.55f * pw;
    y1 = cy - 0.35f * ph;
    x2 = cx + 0.55f * pw;
    y2 = cy + 0.55f * ph;

    label = values[0];
    score = values[1];

    //处理坐标越界问题
    x1 = std::clamp(x1, 0.0f, (float) inWidth);
    y1 = std::clamp(y1, 0.0f, (float) inHeight);
    x2 = std::clamp(x2, 0.0f, (float) inWidth);
    y2 = std::clamp(y2, 0.0f, (float) inHeight);

    if (x2 - x1 > 66 && y2 - y1 > 66) {
      auto rect = cv::Rect2f(x1, y1, x2 - x1, y2 - y1);
      cv::Mat faceRoi = image(rect).clone();
      std::vector<cv::Point2f> keyPoints = landmark(faceRoi, s_landmarkSizeWidth, s_landmarkSizeHeight, x1, y1);
      faceList.emplace_back(score, rect, keyPoints);
    }
  }
  return faceList;
}


std::vector<cv::Point2f> FaceLandmark::landmark(cv::Mat &image,
                                                int poseSizeWidth, int poseSizeHeight,
                                                float x1, float y1) {
  int inWidth = image.cols;
  int inHeight = image.rows;
  ncnn::Mat in = ncnn::Mat::from_pixels_resize(image.data, ncnn::Mat::PIXEL_RGB,
                                               inWidth, inHeight, s_landmarkSizeWidth, s_landmarkSizeHeight);

  const float mean_vals[3] = {127.5f, 127.5f, 127.5f};
  const float norm_vals[3] = {1 / 127.5f, 1 / 127.5f, 1 / 127.5f};
  in.substract_mean_normalize(mean_vals, norm_vals);

  auto ex = m_landmarkNet.create_extractor();
  ex.set_light_mode(true);
  ex.set_num_threads(4);
  if (m_useGPU) {
    ex.set_vulkan_compute(m_useGPU);
  }
  ex.input("data", in);
  ncnn::Mat out;
  ex.extract("bn6_3_bn6_3_scale", out);

  float sw, sh;
  sw = (float) inWidth / (float) s_landmarkSizeWidth;
  sh = (float) inHeight / (float) s_landmarkSizeHeight;

  std::vector<cv::Point2f> points;
  points.reserve(106);
  for (int i = 0; i < 106; i++) {
    float px, py;
    px = out[i * 2] * s_landmarkSizeWidth * sw + x1;
    py = out[i * 2 + 1] * s_landmarkSizeHeight * sh + y1;
    points.emplace_back(cv::Point2f(px, py));
  }

  return points;
}