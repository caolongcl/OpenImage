//
// Created by caolong on 2021/11/17.
//

#include <process/facedetector/FaceLandmarkDetector.hpp>
#include <res/ResManager.hpp>
#include <utils/Utils.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <render/Flow.hpp>
#include <render/polygon/Polygon.hpp>
#include <softarch/Msg.hpp>
#include <render/copier/Copier.hpp>
#include <process/facedetector/FaceLandmark.hpp>

using namespace clt;

FaceLandmarkDetector::FaceLandmarkDetector()
  : m_faceLandmark(new FaceLandmark()),
    m_timeStatics() {
}

bool FaceLandmarkDetector::Init() {
  Log::v(target, "FaceLandmarkDetector::Init");

  ModelNames modelNames;
  modelNames.faceBin = ResManager::Self()->GetResAbsolutePath("/yolov5/yoloface-500k.bin");
  modelNames.faceParam = ResManager::Self()->GetResAbsolutePath("/yolov5/yoloface-500k.param");
  modelNames.landmarkBin = ResManager::Self()->GetResAbsolutePath("/yolov5/landmark106.bin");
  modelNames.landmarkParam = ResManager::Self()->GetResAbsolutePath("/yolov5/landmark106.param");

  m_faceLandmark->Init(modelNames, false);

  m_timeStatics.Init();

  return true;
}

void FaceLandmarkDetector::DeInit() {
  // 清空人脸检测绘制消息
  Flow::Self()->SendMsg(PolygonMsg(Copier::target, Copier::msg_process));
  Flow::Self()->SendMsg(TextMsg(Copier::target, Copier::msg_process_info));

  m_faceLandmark->DeInit();
  m_timeStatics.DeInit();

  Log::v(target, "FaceLandmarkDetector::DeInit");
}

void FaceLandmarkDetector::Process(std::shared_ptr<Buffer> buf) {
  long lastTime = Utils::CurTimeMilli();

  process(*buf);

  Log::n(target, "face landmark detect period %d ms", (Utils::CurTimeMilli() - lastTime));

  Log::debug([this, lastTime]() {
    m_timeStatics.Update(lastTime, [](long period) {
      TextInfo textInfo("face landmark detect:" + std::to_string(period) + "ms");
      textInfo.position = {100.0f, 200.0f};
      Flow::Self()->SendMsg(
        TextMsg(Copier::target, Copier::msg_process_info,
                std::make_shared<TextMsgData>(std::move(textInfo))));
    });
  });
}

void FaceLandmarkDetector::Process(std::shared_ptr<Buffer> buf, Task &&task) {
  Process(buf);
  task();
}

void FaceLandmarkDetector::process(const Buffer &buf) {
  std::vector<FaceLandmark::Face> faceList;

  try {
    cv::Mat frame(buf.height, buf.width, CV_8UC4, buf.data.get());
    cv::cvtColor(frame, frame, cv::COLOR_BGRA2RGB);
    cv::flip(frame, frame, 0); // 上下翻转

//    ResManager::Self()->SaveMatImage("FaceDetectorGray", frame);
    faceList = m_faceLandmark->Detect(frame);

  } catch (std::exception &e) {
    Log::e(target, "face landmark detect exception occur %s", e.what());
  }

  if (faceList.empty()) {
    Flow::Self()->SendMsg(PolygonMsg(Copier::target, Copier::msg_process));
    return;
  }

  std::vector<PolygonObject> objects;
  objects.reserve(faceList.size());

  Log::d(target, "----------------------------------------------------------------------------------------");
  int index = 0;
  for (auto &face : faceList) {
    Log::d(target, "%.2f (%.2f %.2f %.2f %.2f)", face.prob,
           face.rect.x, face.rect.y, face.rect.width, face.rect.height);

    objects.emplace_back(Float2(buf.width, buf.height),
                         Float4{face.rect.x, face.rect.y, face.rect.width, face.rect.height},
                         Colors[index % ColorsSize],
                         std::to_string(face.prob));

    std::vector<Float2> points;
    points.reserve(face.points.size());
    for (auto &point : face.points) {
      points.emplace_back(point.x, point.y);
    }
    objects.emplace_back(Float2(buf.width, buf.height),
                         std::move(points),
                         Colors[index % ColorsSize],
                         "", true);
    ++index;
  }

  Flow::Self()->SendMsg(
    PolygonMsg(Copier::target, Copier::msg_process,
               std::make_shared<PolygonMsgData>(std::move(objects))));
}
