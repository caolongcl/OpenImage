//
// Created by caolong on 2020/8/10.
//

#include <res/ResManager.hpp>
#include <utils/Utils.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <process/facedetector/FaceDetector.hpp>
#include <render/Flow.hpp>
#include <render/polygon/Polygon.hpp>
#include <softarch/Msg.hpp>
#include <render/copier/Copier.hpp>

using namespace clt;

FaceDetector::FaceDetector()
  : m_faceCascade(new CascadeClassifierFaceDetector()),
    m_timeStatics() {

}

bool FaceDetector::Init() {
  Log::v(target, "FaceDetector::Init");

  const std::string file =
    ResManager::Self()->GetResAbsolutePath("/opencv/haarcascade_frontalface_alt.xml");
  m_faceCascade->Init(file);

  m_timeStatics.Init();

  return true;
}

void FaceDetector::DeInit() {
  // 清空人脸检测绘制消息
  Flow::Self()->SendMsg(PolygonMsg(Copier::target, Copier::msg_process));
  Flow::Self()->SendMsg(TextMsg(Copier::target, Copier::msg_process_info));

  m_faceCascade->DeInit();
  m_timeStatics.DeInit();

  Log::v(target, "FaceDetector::DeInit");
}

void FaceDetector::Process(std::shared_ptr<Buffer> buf) {
  long lastTime = Utils::CurTimeMilli();

  process(*buf);

  Log::n(target, "face detect period %d ms", (Utils::CurTimeMilli() - lastTime));

  Log::debug([this, lastTime]() {
    m_timeStatics.Update(lastTime, [](long period) {
      TextInfo textInfo("face_detect:" + std::to_string(period) + "ms");
      textInfo.position = {100.0f, 200.0f};
      Flow::Self()->SendMsg(
        TextMsg(Copier::target, Copier::msg_process_info,
                std::make_shared<TextMsgData>(std::move(textInfo))));
    });
  });
}

void FaceDetector::Process(std::shared_ptr<Buffer> buf, Task &&task) {
  Process(buf);
  task();
}

void FaceDetector::process(const Buffer &buf) {
  std::vector<cv::Rect> faces;

  try {
    cv::Mat frame(buf.height, buf.width, CV_8UC4, buf.data.get()); // buf是RGBA的，frame把它当成BGRA的
    cv::Mat frameGray;
    cv::cvtColor(frame, frameGray, cv::COLOR_RGBA2GRAY);
    cv::flip(frameGray, frameGray, 0); // 上下翻转

//        ResManager::Self()->SaveMatImage("FaceDetectorGray", frameGray);

    cv::equalizeHist(frameGray, frameGray);

    m_faceCascade->Detect(frameGray, faces);

  } catch (std::exception &e) {
    Log::e(target, "face detect exception occur %s", e.what());
  }

  if (faces.empty()) {
    Flow::Self()->SendMsg(PolygonMsg(Copier::target, Copier::msg_process));
    return;
  }

  std::vector<PolygonObject> objects;
  objects.reserve(faces.size());

  for (auto &face : faces) {
    Log::n(target, "face region (%d %d %d %d)", face.x, face.y, face.width, face.height);

    objects.emplace_back(Float2(buf.width, buf.height),
                         Float4{(float) face.x, (float) face.y,
                                (float) face.width, (float) face.height},
                         GreenColor, "face_" + std::to_string(objects.size()));
  }

  Flow::Self()->SendMsg(
    PolygonMsg(Copier::target, Copier::msg_process,
               std::make_shared<PolygonMsgData>(std::move(objects))));
}
