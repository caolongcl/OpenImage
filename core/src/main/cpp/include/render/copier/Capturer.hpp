//
// Created by caolong on 2020/8/10.
//

#pragma once

#include <softarch/std.hpp>
#include <softarch/IComFunc.hpp>
#include <render/copier/ICopierSurface.hpp>

namespace clt {

  /**
   * 拍照
   */
  class Surface;

  class Copier;

  class Capturer final
    : public IComFunc<std::shared_ptr<Copier>>,
      public ICopierSurface<std::shared_ptr<Surface>> {
  ClassDeclare(Capturer)
  public:
    Capturer();

    ~Capturer() = default;

    bool Init(std::shared_ptr<Copier>) override;

    void DeInit() override;

    void RegisterSurface(std::shared_ptr<Surface> args) override;

    void Render() override;

  private:
    std::shared_ptr<Surface> m_surface;
    std::shared_ptr<Copier> m_copier;
  };

}
