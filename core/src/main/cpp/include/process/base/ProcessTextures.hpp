//
// Created by caolong on 2021/3/8.
//

#pragma once

#include <softarch/std.hpp>
#include <softarch/IComFunc.hpp>

namespace clt {
  /**
   * 包装两个特定大小的纹理队列
   */
  class Texture;

  class ProcessTextures final : public IComFunc<>,
                                public IComUpdate<const std::size_t, const std::size_t> {
  public:
    using TextureQueue = std::queue<std::shared_ptr<Texture>>;
  public:
    ProcessTextures();

    ~ProcessTextures() = default;

    bool Init() override;

    void Update(const std::size_t width, const std::size_t height) override;

    void DeInit() override;

    std::shared_ptr<Texture> PopWriteTexture();

    std::shared_ptr<Texture> PopReadTexture();

    void PushWriteTexture(const std::shared_ptr<Texture> &id);

    void PushReadTexture(const std::shared_ptr<Texture> &id);

  private:
    void resetTextureQueue();

  private:
    static const int QUEUE_SIZE = 6;

    TextureQueue m_writer;
    TextureQueue m_reader;

    std::mutex m_mutex; // 同步读写纹理队列
    std::vector<std::shared_ptr<Texture>> m_textures;
  };

};