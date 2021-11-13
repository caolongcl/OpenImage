//
// Created by caolong on 2021/11/7.
//

#pragma once

#include <utils/Utils.hpp>
#include <process/base/ThreadBufferProcessTask.hpp>
#include <process/base/ProcessUtils.hpp>
#include <process/yolov5/Yolov5.hpp>

namespace clt {
  class ObjectDetector final : public IBufferProcessTask {
  ClassDeclare(ObjectDetector)
  ClassWrapper(ThreadBufferProcessTask)
  public:
    ObjectDetector();

    ~ObjectDetector() = default;

    bool Init() override;

    void DeInit() override;

    void Process(std::shared_ptr<Buffer> buf) override;

  private:
    void process(const Buffer &buf);

  private:
    std::shared_ptr<Yolov5> m_yolov5;
    TimeStatics m_timeStatics;
  };
}
