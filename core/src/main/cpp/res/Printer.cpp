//
// Created by caolong on 2020/7/24.
//

#include <res/ResManager.hpp>
#include <res/Printer.hpp>
#include <render/Flow.hpp>
#include <res/font/Font.hpp>

using namespace clt;

std::mutex Printer::s_mutex;

bool Printer::Init() {
  Log::v(target, "Printer::Init");
  return true;
}

void Printer::DeInit() {
  Log::v(target, "Printer::DeInit");
}

void Printer::Print(const TextInfo &info) {
  assert(Flow::Self()->InvokeInRender()); // 只能在渲染线程绘制

  if (ResManager::Self()->GetGUIFont() != nullptr) {
    ResManager::Self()->GetGUIFont()->RenderText(info);
  }
}