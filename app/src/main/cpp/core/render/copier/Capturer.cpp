//
// Created by caolong on 2020/7/14.
//

#include <render/copier/Capturer.hpp>
#include <render/copier/Copier.hpp>
#include <render/copier/Surface.hpp>

using namespace clt;

Capturer::Capturer()
        : m_surface(nullptr),
          m_copier(nullptr) {

}

bool Capturer::Init(std::shared_ptr<Copier> copier) {
    Log::v(Log::RENDER_TAG, "Capturer::Init");

    m_copier = std::move(copier);
    m_surface = nullptr;
    return true;
}

void Capturer::DeInit() {
    if (m_surface != nullptr) {
        m_surface->DeInit();
    }

    m_copier = nullptr;
    m_surface = nullptr;
    Log::v(Log::RENDER_TAG, "Capturer::DeInit");
}

void Capturer::RegisterSurface(std::shared_ptr<Surface> surface) {
    if (m_surface != nullptr) {
        m_surface->DeInit();
    }

    m_surface = std::move(surface);
}

void Capturer::Render() {
    if (m_surface != nullptr && m_copier != nullptr) {
        m_surface->Prepare();
        m_copier->CopyPreFrame(m_copier->GetPreviewViewport());
        m_surface->Swap();
    }
}