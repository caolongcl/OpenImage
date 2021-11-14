//
// Created by caolong on 2021/3/13.
//

#include <process/base/BufferProcessTask.hpp>
#include <process/base/NormalProcessTask.hpp>
#include <process/facedetector/FaceDetector.hpp>
#include <process/ar/CalibrateCamera.hpp>
#include <process/ar/MarkerAR.hpp>
#include <softarch/std.hpp>
#include <utils/Utils.hpp>
#include <process/yolov5/ObjectDetector.hpp>

using namespace clt;

#define CreateProcess(name, process) \
if (name == process::target) {\
return std::make_shared<process::Wrapper>(process::target, std::make_shared<process>());}

std::shared_ptr<IProcessTask> ProcessFactory::Create(const std::string &name) {
  CreateProcess(name, FaceDetector)
  CreateProcess(name, CalibrateCamera)
  CreateProcess(name, MarkerAR)
  CreateProcess(name, ObjectDetector)
  return nullptr;
}

#define DetectProcessType(name, process) \
if (name == process::target) {\
return process::s_type;}

ProcessTaskType ProcessFactory::Type(const std::string &name) {
  DetectProcessType(name, FaceDetector)
  DetectProcessType(name, CalibrateCamera)
  DetectProcessType(name, MarkerAR)
  DetectProcessType(name, ObjectDetector)
  return ProcessTaskType::eUnknownTask;
}
