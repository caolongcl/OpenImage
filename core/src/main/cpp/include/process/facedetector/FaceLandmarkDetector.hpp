//
// Created by caolong on 2021/11/17.
//

#pragma once

#include <utils/Utils.hpp>
#include <process/base/ThreadBufferProcessTask.hpp>
#include <process/base/ProcessUtils.hpp>

namespace clt {
  class FaceLandmark;

  class FaceLandmarkDetector final : public IBufferProcessTask {
  ClassDeclare(FaceLandmarkDetector)
  ClassWrapper(ThreadBufferProcessTask)
  public:
    FaceLandmarkDetector();

    ~FaceLandmarkDetector() = default;

    bool Init() override;

    void DeInit() override;

    void Process(std::shared_ptr<Buffer> buf) override;

    void Process(std::shared_ptr<Buffer> buf, Task &&task) override;

  private:
    void process(const Buffer &buf);

  private:
    std::shared_ptr<FaceLandmark> m_faceLandmark;
    TimeStatics m_timeStatics;
  };
}
