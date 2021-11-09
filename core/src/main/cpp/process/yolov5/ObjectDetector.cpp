//
// Created by caolong on 2021/11/7.
//

#include <process/yolov5/ObjectDetector.hpp>
#include <res/ResManager.hpp>
#include <utils/Utils.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <render/Flow.hpp>
#include <render/polygon/Polygon.hpp>
#include <softarch/Msg.hpp>
#include <render/copier/Copier.hpp>

using namespace clt;

ncnn::Mutex ObjectDetector::mutex;

ObjectDetector::ObjectDetector()
  : m_yolov5(new Yolov5()),
    m_timeStatics() {
}

bool ObjectDetector::Init() {
  Log::v(target, "ObjectDetector::Init");

  const std::string modelPath = ResManager::Self()->GetResAbsolutePath("/yolov5/yolov5.bin");
  const std::string paramPath = ResManager::Self()->GetResAbsolutePath("/yolov5/yolov5.param");

  m_yolov5->Init(paramPath, modelPath, false);

  m_timeStatics.Init();

  return true;
}

void ObjectDetector::DeInit() {
  // 清空人脸检测绘制消息
  Flow::Self()->SendMsg(PolygonMsg(Copier::target, Copier::msg_detect_face));
  Flow::Self()->SendMsg(TextMsg(Copier::target, Copier::msg_detect_face_info));

  m_yolov5->DeInit();
  m_timeStatics.DeInit();

  Log::v(target, "ObjectDetector::DeInit");
}

void ObjectDetector::Process(std::shared_ptr<Buffer> buf) {
  long lastTime = Utils::CurTimeMilli();

  process(*buf);

  Log::n(target, "object detect period %d ms", (Utils::CurTimeMilli() - lastTime));

  Log::debug([this, lastTime]() {
    m_timeStatics.Update(lastTime, [](long period) {
      TextInfo textInfo("object_detect:" + std::to_string(period) + "ms");
      textInfo.position = {100.0f, 200.0f};
      Flow::Self()->SendMsg(
        TextMsg(Copier::target, Copier::msg_detect_face_info,
                std::make_shared<TextMsgData>(std::move(textInfo))));
    });
  });
}

void ObjectDetector::process(const Buffer &buf) {
  std::vector<Yolov5::BoxInfo> boxInfo;

  try {
    cv::Mat frame(buf.height, buf.width, CV_8UC4, buf.data.get());
    cv::cvtColor(frame, frame, cv::COLOR_RGBA2RGB);
    cv::flip(frame, frame, 0); // 上下翻转

//        ResManager::Self()->SaveMatImage("FaceDetectorGray", frameGray);
    boxInfo = m_yolov5->Detect(frame, 0.5, 0.5);

  } catch (std::exception &e) {
    Log::e(target, "object detect exception occur %s", e.what());
  }

  if (boxInfo.empty()) {
    Flow::Self()->SendMsg(PolygonMsg(Copier::target, Copier::msg_detect_face));
    return;
  }

  std::vector<PolygonObject> objects;
  objects.reserve(boxInfo.size());

  for (auto &box : boxInfo) {
    Log::d(target, "object %s region (%f %f %f %f)", m_yolov5->LabelStr(box.label).c_str(),
           box.x1, box.y1, (box.y1 - box.x1 + 1),
           (box.y2 - box.x2 + 1));

    objects.emplace_back(Float2(buf.width, buf.height),
                         Float4{(float) box.x1, (float) box.y1,
                                (float) (box.y1 - box.x1 + 1), (box.y2 - box.x2 + 1)},
                         GreenColor, m_yolov5->LabelStr(box.label));
  }

  Flow::Self()->SendMsg(
    PolygonMsg(Copier::target, Copier::msg_detect_face,
               std::make_shared<PolygonMsgData>(std::move(objects))));
}
