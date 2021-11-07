//
// Created by caolong on 2020/8/14.
//

#include <render/Flow.hpp>
#include <render/copier/Surface.hpp>

using namespace clt;

Surface::Surface(std::string name)
    : m_name(std::move(name)) {

}

bool Surface::Init(ANativeWindow *window) {
  assert(window != nullptr);

  Flow::Self()->GetInvokeThread()->DestroyWindowSurface(m_name);
  Flow::Self()->GetInvokeThread()->CreateWindowSurface(m_name, window);
  return true;
}

void Surface::DeInit() {
  Flow::Self()->GetInvokeThread()->DestroyWindowSurface(m_name);
}

void Surface::Prepare() {
  Flow::Self()->GetInvokeThread()->ActiveContext(m_name);
}

void Surface::Swap() {
  Flow::Self()->GetInvokeThread()->Update(m_name);
}

void Surface::SetPresentationTime(khronos_stime_nanoseconds_t nsecs) {
  Flow::Self()->GetInvokeThread()->SetPresentationTime(m_name, nsecs);
}