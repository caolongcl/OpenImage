//
// Created by caolong on 2021/11/14.
//

#pragma once

#include <softarch/std.hpp>
#include <softarch/IComFunc.hpp>

namespace clt {
  class Texture;

  class Buffer;

  class TextureBuffer final : public IComFunc<>,
                              public IComUpdate<const std::size_t, const std::size_t> {
  public:
    TextureBuffer() = default;
    ~TextureBuffer() = default;

    bool Init() override;

    void Update(const std::size_t width, const std::size_t height) override;

    void DeInit() override;

    std::shared_ptr<Texture> &GetTexture() { return m_texture; }

    std::shared_ptr<Buffer> &GetBuffer() { return m_buffer; }
  private:
    std::shared_ptr<Texture> m_texture;
    std::shared_ptr<Buffer> m_buffer;
  };
}
