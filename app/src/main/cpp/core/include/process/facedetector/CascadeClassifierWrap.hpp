//
// Created by caolong on 2020/8/12.
//

#pragma once

#include <opencv2/core.hpp>
#include <opencv2/objdetect.hpp>
#include <utils/Log.hpp>
#include <softarch/IComFunc.hpp>

namespace clt {

  class CascadeDetectorAdapter : public cv::DetectionBasedTracker::IDetector {
  public:
    CascadeDetectorAdapter(cv::Ptr<cv::CascadeClassifier> detector) :
        IDetector(),
        Detector(detector) {
      Log::v(Log::PROCESSOR_TAG, "CascadeDetectorAdapter::Detect::Detect");
      CV_Assert(detector);
    }

    void detect(const cv::Mat &Image, std::vector<cv::Rect> &objects) {
//    Log::v(Log::PROCESSOR_TAG, "CascadeDetectorAdapter::Detect: begin");
//    Log::v(Log::PROCESSOR_TAG,
//           "CascadeDetectorAdapter::Detect: scaleFactor=%.2f, "
//           "minNeighbours=%d, minObjSize=(%dx%d), maxObjSize=(%dx%d)",
//           scaleFactor,
//           minNeighbours,
//           minObjSize.width,
//           minObjSize.height,
//           maxObjSize.width,
//           maxObjSize.height);
      Detector->detectMultiScale(Image,
                                 objects,
                                 scaleFactor,
                                 minNeighbours,
                                 2,
                                 minObjSize,
                                 maxObjSize);
//    Log::v(Log::PROCESSOR_TAG, "CascadeDetectorAdapter::Detect: end");
    }

    virtual ~CascadeDetectorAdapter() {
      Log::v(Log::PROCESSOR_TAG, "CascadeDetectorAdapter::Detect::~Detect");
    }

  private:
    CascadeDetectorAdapter() = delete;

    cv::Ptr<cv::CascadeClassifier> Detector;
  };

  struct DetectorAggregator {
    cv::Ptr<CascadeDetectorAdapter> mainDetector;
    cv::Ptr<CascadeDetectorAdapter> trackingDetector;

    cv::Ptr<cv::DetectionBasedTracker> tracker;

    DetectorAggregator(cv::Ptr<CascadeDetectorAdapter> &_mainDetector,
                       cv::Ptr<CascadeDetectorAdapter> &_trackingDetector) :
        mainDetector(_mainDetector),
        trackingDetector(_trackingDetector) {
      CV_Assert(_mainDetector);
      CV_Assert(_trackingDetector);

      cv::DetectionBasedTracker::Parameters DetectorParams;
      tracker =
          cv::makePtr<cv::DetectionBasedTracker>(mainDetector, trackingDetector,
                                                 DetectorParams);
    }
  };

/**
 *
 */
  class CascadeClassifierWrap final : public IComFunc<> {
  public:
    explicit CascadeClassifierWrap(std::string file);

    ~CascadeClassifierWrap() = default;

    bool Init() override;

    void DeInit() override;

    void SetMinObjectSize(int size);

    void SetMinNeighbors(int n);

    void Start();

    void Stop();

    void Detect(const cv::Mat &image, std::vector<cv::Rect> &objects);

  private:
    std::shared_ptr<DetectorAggregator> m_detector;
    std::string m_file;
  };

  // 简单人脸检测
  class CascadeClassifierFaceDetector final : public IComFunc<const std::string &> {
  public:
    CascadeClassifierFaceDetector() {

    }

    bool Init(const std::string &file) override {
      if (!m_classifier.load(file)) {
        Log::e(Log::PROCESSOR_TAG, "CascadeClassifierFaceDetector load error %s",
               file.c_str());
        return false;
      }

      return true;
    }

    void DeInit() override {
    }

    void Detect(const cv::Mat &image, std::vector<cv::Rect> &objects) {
      std::vector<int> rejLevel;
      std::vector<double> levelW;
      std::vector<cv::Rect> innerObjects;
      m_classifier.detectMultiScale(image, innerObjects, rejLevel, levelW, 1.1, 2, 0,
                                    cv::Size(10, 10), cv::Size(180, 180), true);
      for (int i = 0; i < innerObjects.size(); ++i) {
        if (rejLevel[i] >= 22) {
          objects.push_back(innerObjects[i]);
        }
      }
//            m_classifier.detectMultiScale(Image, objects, 1.1, 2, 0, cv::Size(30, 30),cv::Size(60,60));
    }

    ~CascadeClassifierFaceDetector() = default;

  private:
    cv::CascadeClassifier m_classifier;
  };

}