//
// Created by caolong on 2020/8/17.
//

#pragma once

#include <process/base/IPixelReader.hpp>
#include <softarch/GLUtils.hpp>

namespace clt {

  /**
   * 从RGBA格式的纹理中读取颜色数据
   */
  class PixelReaderPbo final : public IPixelReader {
  public:
    PixelReaderPbo();

    ~PixelReaderPbo() = default;

    bool Init() override;

    void DeInit() override;

    void Read(std::shared_ptr<Texture> tex, std::shared_ptr<Buffer> buf) override;

    void Update(const std::size_t width, const std::size_t height) override;

  private:
    void createPbos();

    void deletePbos();

    /**
     * 从纹理中中读取数据拷贝到buf中
     * @param buf
     */
    void readFromPbos(const std::shared_ptr<Texture> &tex, const std::shared_ptr<Buffer> &buf);

  private:
    static const int s_pboCount = 2;

  private:
    GLuint m_fbo;
    GLuint m_pbo[s_pboCount];
    int m_curPboIndex;
    std::size_t m_width;
    std::size_t m_height;
    std::size_t m_pboBytes;

    std::mutex m_mutex;
  };

}
