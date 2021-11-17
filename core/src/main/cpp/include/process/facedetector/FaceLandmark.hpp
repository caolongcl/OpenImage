//
// Created by caolong on 2021/11/17.
//

#pragma once

#include <softarch/std.hpp>
#include <ncnn/net.h>
#include <opencv2/core.hpp>
#include <softarch/IComFunc.hpp>
#include <opencv2/core/types.hpp>

namespace clt {

  struct ModelNames {
    std::string faceParam;
    std::string faceBin;
    std::string landmarkParam;
    std::string landmarkBin;
  };

  class FaceLandmark final : public IComFunc<const ModelNames &, bool> {
  ClassDeclare(FaceLandmark);
  public:
    struct Face {
      Face(float _prob, const cv::Rect &_rect, const std::vector<cv::Point2f> &_points)
        : prob(_prob), rect(_rect), points(_points) {}
      float prob;
      cv::Rect2f rect;
      std::vector<cv::Point2f> points;
    };

    FaceLandmark();

    ~FaceLandmark() = default;

    bool Init(const ModelNames &, bool useGPU) override;

    void DeInit() override;

    std::vector<Face> Detect(const cv::Mat &image);

  private:
    std::vector<cv::Point2f> landmark(cv::Mat &roi, int poseSizeWidth, int poseSizeHeight, float x1, float y1);

  private:
    static const int s_detectorSizeWidth = 320;
    static const int s_detectorSizeHeight = 256;
    static const int s_landmarkSizeWidth = 112;
    static const int s_landmarkSizeHeight = 112;

    ncnn::Net m_faceNet;
    ncnn::Net m_landmarkNet;
    bool m_useGPU;
  };
}
