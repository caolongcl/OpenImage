//
// Created by caolong on 2020/8/10.
//

#pragma once

#include <utils/Utils.hpp>
#include <process/base/BufferProcessTask.hpp>
#include <process/facedetector/CascadeClassifierWrap.hpp>
#include <process/base/ProcessUtils.hpp>

namespace clt {

  class FaceDetector final : public IBufferProcessTask {
  ClassDeclare(FaceDetector)
  ClassWrapper(BufferProcessTask)
  public:
    FaceDetector();

    ~FaceDetector() = default;

    bool Init() override;

    void DeInit() override;

    void Process(std::shared_ptr<Buffer> buf) override;

  private:
    void process(const Buffer &buf);

  private:
    std::shared_ptr<CascadeClassifierFaceDetector> m_faceCascade;
    TimeStatics m_timeStatics;
  };

}
