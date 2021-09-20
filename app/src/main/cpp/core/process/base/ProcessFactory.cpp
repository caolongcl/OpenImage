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

using namespace clt;

#define CreateProcess(type, process) \
if (type == process::target) {\
return std::make_shared<process::Wrapper>(process::target, std::make_shared<process>());}

std::shared_ptr<IProcessTask> ProcessFactory::Create(const std::string &type) {
  CreateProcess(type, FaceDetector)
  CreateProcess(type, CalibrateCamera)
  CreateProcess(type, MarkerAR)
  return nullptr;
}
