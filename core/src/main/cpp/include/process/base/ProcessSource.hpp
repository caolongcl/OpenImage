//
// Created by caolong on 2021/11/14.
//

#pragma once

#include <softarch/std.hpp>
#include <softarch/IComFunc.hpp>
#include <softarch/RingQueue.hpp>
#include <process/base/IPixelReader.hpp>

namespace clt {
  class TextureBuffer;

  class Texture;

  class PixelReaderPbo;

  using SourceItem = std::shared_ptr<TextureBuffer>;
  using SoureceQueue = RingQueue<SourceItem>;

  struct IFeeder {
    using Feeder = std::function<void(std::shared_ptr<Texture> &)>;

    IFeeder() = default;

    virtual ~IFeeder() = default;

    virtual void Feed(const Feeder &feeder) = 0;

    virtual void Reset() = 0;
  };

  struct IEater {
    using Eater = std::function<void(SourceItem &)>;

    IEater() = default;

    virtual ~IEater() = default;

    virtual bool Eat(const Eater &eater) = 0;
  };


  class ProcessSource final : public IComFunc<>,
                              public IComUpdate<const std::size_t, const std::size_t>,
                              public IFeeder,
                              public IEater,
                              public IPixelReader {
  ClassDeclare(ProcessSource);
  public:
    class TextureBufferRingQueue final : public SoureceQueue,
                                         public IComFunc<>,
                                         public IComUpdate<const std::size_t, const std::size_t> {
    public:
      TextureBufferRingQueue(int size);

      ~TextureBufferRingQueue() = default;

      bool Init() override;

      void Update(const std::size_t width, const std::size_t height) override;

      void DeInit() override;
    };

  public:
    ProcessSource(int size);

    ~ProcessSource() = default;

    bool Init() override;

    void Update(const std::size_t width, const std::size_t height) override;

    void DeInit() override;

    void Feed(const Feeder &feeder) override;

    bool Eat(const Eater &eater) override;

    void Read(std::shared_ptr<Texture> tex, std::shared_ptr<Buffer> buf) override;

    void Reset() override;

  private:
    std::shared_ptr<TextureBufferRingQueue> m_queue;
    std::shared_ptr<PixelReaderPbo> m_pixelReader;
    int m_size;
  };
}
