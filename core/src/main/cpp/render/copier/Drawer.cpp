//
// Created by caolong on 2020/6/18.
//

#include <render/copier/Copier.hpp>
#include <render/copier/Drawer.hpp>
#include <render/copier/Surface.hpp>


using namespace clt;

Drawer::Drawer()
    : m_surface(nullptr),
      m_copier(nullptr) {
}

bool Drawer::Init(std::shared_ptr<Copier> copier) {
  Log::v(Log::RENDER_TAG, "Drawer::Init");

  m_copier = std::move(copier);
  m_surface = nullptr;

  return true;
}

void Drawer::DeInit() {
  if (m_surface != nullptr) {
    m_surface->DeInit();
  }

  m_copier = nullptr;
  m_surface = nullptr;

  Log::v(Log::RENDER_TAG, "Drawer::DeInit");
}

void Drawer::RegisterSurface(std::shared_ptr<Surface> surface) {
  if (m_surface != nullptr) {
    m_surface->DeInit();
  }
  m_surface = std::move(surface);
}

void Drawer::Render() {
  if (m_surface != nullptr && m_copier != nullptr) {
    m_surface->Prepare();
    m_copier->CopyPostFrame(m_copier->GetDisplayViewport());
    m_surface->Swap();
  }
}

