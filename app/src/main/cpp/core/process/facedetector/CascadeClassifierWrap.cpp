//
// Created by caolong on 2020/8/12.
//

#include <process/facedetector/CascadeClassifierWrap.hpp>

using namespace clt;

CascadeClassifierWrap::CascadeClassifierWrap(std::string file)
    : m_detector(nullptr),
      m_file(std::move(file)) {

}

bool CascadeClassifierWrap::Init() {
  auto mainDetector =
      cv::makePtr<CascadeDetectorAdapter>(cv::makePtr<cv::CascadeClassifier>(m_file));
  auto trackingDetector =
      cv::makePtr<CascadeDetectorAdapter>(cv::makePtr<cv::CascadeClassifier>(m_file));

  m_detector = std::make_shared<DetectorAggregator>(mainDetector, trackingDetector);

  m_detector->mainDetector->setScaleFactor(1.1);
//  m_detector->trackingDetector->setScaleFactor(1.1);

  assert(m_detector);

  return true;
}

void CascadeClassifierWrap::DeInit() {
  m_detector = nullptr;
}

void CascadeClassifierWrap::SetMinObjectSize(int size) {
  if (size > 0 && m_detector) {
    m_detector->mainDetector->setMinObjectSize(cv::Size(size, size));
//    m_detector->trackingDetector->setMinObjectSize(cv::Size(size, size));
  }
}

void CascadeClassifierWrap::SetMinNeighbors(int n) {
  if (n > 0 && m_detector) {
    m_detector->mainDetector->setMinNeighbours(n);
//    m_detector->trackingDetector->setMinNeighbours(n);
  }
}

void CascadeClassifierWrap::Start() {
  if (m_detector) {
    m_detector->tracker->run();
  }
}

void CascadeClassifierWrap::Stop() {
  if (m_detector) {
    m_detector->tracker->stop();
  }
}

void CascadeClassifierWrap::Detect(const cv::Mat &image, std::vector<cv::Rect> &objects) {
  if (m_detector) {
//    m_detector->mainDetector->detect(image, objects);
//    m_detector->trackingDetector->detect(image, objects);
    m_detector->tracker->process(image);
    m_detector->tracker->getObjects(objects);
  }
}