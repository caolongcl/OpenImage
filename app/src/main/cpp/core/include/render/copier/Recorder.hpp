//
// Created by caolong on 2020/8/10.
//

#pragma once

#include <softarch/std.hpp>
#include <softarch/IComFunc.hpp>
#include <render/copier/ICopierSurface.hpp>

namespace clt {

  class Copier;

  class Surface;

  class Recorder final
      : public IComFunc<std::shared_ptr<Copier>>,
        public ICopierSurface<std::shared_ptr<Surface>> {
  public:
    Recorder();

    ~Recorder() = default;

    /**
     * Copier 是渲染数据的来源，是 FilterPipe 的输出
     * @return
     */
    bool Init(std::shared_ptr<Copier>) override;

    void DeInit() override;

    /**
     * Surface 是渲染数据的目的地
     * @param args
     */
    void RegisterSurface(std::shared_ptr<Surface> args) override;

    void Record(bool start, int fps, int bitrate);

    /**
     * Copier 拷贝一帧到 Surface 上
     */
    void Render() override;

    bool Recording() const;

    void GetRecordParams(int &fps, int &bitrate) const;

  private:
    int m_fps{};
    int m_bitrate{};
    long m_startTime{};
    int m_encodedFrameCount{};
    bool m_recording;

    std::shared_ptr<Surface> m_surface;
    std::shared_ptr<Copier> m_copier;
  };

}