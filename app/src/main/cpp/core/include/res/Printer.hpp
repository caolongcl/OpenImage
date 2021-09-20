//
// Created by caolong on 2020/8/6.
//

#pragma once

#include <softarch/ComType.hpp>
#include <softarch/VarType.hpp>
#include <softarch/IComFunc.hpp>
#include <res/IRenderText.hpp>
#include <res/font/Font.hpp>

namespace clt {

  class Printer final
      : public IComFunc<>,
        public IPrint {
  public:

    bool Init() override;

    void DeInit() override;

    void Print(const TextInfo &info) override;

    /**
     * 绘制任务
     */
    struct PrintTask {
      using InnerTask = std::function<TextInfo(const Viewport &displayViewport, const Viewport &realViewport)>;

      PrintTask(InnerTask &&_task) : task(std::forward<InnerTask>(_task)) {}

      void operator()(const Viewport &displayVp, const Viewport &realVp) {
        Printer::Self()->Print(task(displayVp, realVp));
      }

      InnerTask task;
    };

    /**
     * singleton
     */
  public:
    static std::shared_ptr<Printer> Self() {
      std::lock_guard<std::mutex> locker(s_mutex);
      static std::shared_ptr<Printer> instance(new Printer());
      return instance;
    }

    Printer(const Printer &) = delete;

    Printer &operator=(const Printer &) = delete;

    Printer(Printer &&) = delete;

    Printer &operator=(Printer &&) = delete;

    ~Printer() = default;

  private:
    Printer() = default;

    static std::mutex s_mutex;
  };
}
