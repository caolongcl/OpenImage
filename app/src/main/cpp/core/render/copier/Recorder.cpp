//
// Created by caolong on 2020/7/14.
//

#include <render/copier/Copier.hpp>
#include <render/Flow.hpp>
#include <utils/Utils.hpp>
#include <render/copier/Recorder.hpp>
#include <render/copier/Surface.hpp>

using namespace clt;

Recorder::Recorder()
        : m_recording(false),
          m_surface(nullptr),
          m_copier(nullptr) {
}

bool Recorder::Init(std::shared_ptr<Copier> copier) {
    Log::v(Log::RENDER_TAG, "Recorder::Init");
    m_copier = std::move(copier);
    m_surface = nullptr;
    return true;
}

void Recorder::DeInit() {
    if (m_surface != nullptr) {
        m_surface->DeInit();
    }

    m_copier = nullptr;
    m_recording = false;
    Log::v(Log::RENDER_TAG, "Recorder::DeInit");
}

void Recorder::RegisterSurface(std::shared_ptr<Surface> surface) {
    if (m_surface != nullptr) {
        m_surface->DeInit();
    }

    m_surface = std::move(surface);
}

void Recorder::Record(bool start, int fps, int bitrate) {
    m_fps = fps;
    m_bitrate = bitrate;
    m_startTime = -1;
    m_encodedFrameCount = 0;

    m_recording = start;

    if (!m_recording && m_surface != nullptr) {
        m_surface->DeInit();
        m_surface = nullptr;
    }
}

void Recorder::Render() {
    if (m_surface == nullptr || m_copier == nullptr)
        return;

    if (!m_recording)
        return;

    if (m_startTime == -1) {
        m_startTime = Utils::CurTimeMilli();
    }

    int expectedFrameCount = (int) std::lround(Utils::CurTimeMilli() / 1000 * m_fps);
    if (expectedFrameCount < m_encodedFrameCount)
        return;

    m_encodedFrameCount++;

    int64_t time = Utils::CurTimeMilli() - m_startTime;

    m_surface->Prepare();
    m_copier->CopyPreFrame(m_copier->GetPreviewViewport());
    m_surface->SetPresentationTime(((khronos_stime_nanoseconds_t) time) * 1000000);
    m_surface->Swap();
}

bool Recorder::Recording() const {
    return m_recording;
}

void Recorder::GetRecordParams(int &fps, int &bitrate) const {
    fps = m_fps;
    bitrate = m_bitrate;
}