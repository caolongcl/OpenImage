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
  Flow::Self()->SendMsg(PolygonMsg(Copier::target, Copier::msg_process));
  Flow::Self()->SendMsg(TextMsg(Copier::target, Copier::msg_process_info));

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
        TextMsg(Copier::target, Copier::msg_process_info,
                std::make_shared<TextMsgData>(std::move(textInfo))));
    });
  });
}

void ObjectDetector::Process(std::shared_ptr<Buffer> buf, Task &&task) {
  Process(buf);
  task();
}

void ObjectDetector::process(const Buffer &buf) {
  std::vector<Yolov5::BoxInfo> boxInfo;

  try {
    cv::Mat frame(buf.height, buf.width, CV_8UC4, buf.data.get());
    cv::cvtColor(frame, frame, cv::COLOR_BGRA2RGB);
    cv::flip(frame, frame, 0); // 上下翻转

//    ResManager::Self()->SaveMatImage("FaceDetectorGray", frame);
    boxInfo = m_yolov5->Detect(frame, 0.3, 0.7);

  } catch (std::exception &e) {
    Log::e(target, "object detect exception occur %s", e.what());
  }

  if (boxInfo.empty()) {
    Flow::Self()->SendMsg(PolygonMsg(Copier::target, Copier::msg_process));
    return;
  }

  std::vector<PolygonObject> objects;
  objects.reserve(boxInfo.size());

  Log::d(target, "----------------------------------------------------------------------------------------");
  for (auto &box : boxInfo) {
    Log::d(target, "%s %.2f (%.2f %.2f %.2f %.2f)", m_yolov5->LabelStr(box.label).c_str(), box.score,
           box.x1, box.y1, (box.x2 - box.x1 + 1),
           (box.y2 - box.y1 + 1));

    objects.emplace_back(Float2(buf.width, buf.height),
                         Float4{(float) box.x1, (float) box.y1,
                                (float) (box.x2 - box.x1 + 1), (box.y2 - box.y1 + 1)},
                         Colors[box.label % ColorsSize],
                         m_yolov5->LabelStr(box.label) + " " + std::to_string(box.score));
  }

  Flow::Self()->SendMsg(
    PolygonMsg(Copier::target, Copier::msg_process,
               std::make_shared<PolygonMsgData>(std::move(objects))));
}
