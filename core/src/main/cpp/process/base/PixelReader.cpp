//
// Created by caolong on 2020/8/17.
//

#include <utils/Utils.hpp>
#include <process/base/PixelReader.hpp>
#include <softarch/Constants.hpp>
#include <render/texture/Texture.hpp>

using namespace clt;

PixelReaderPbo::PixelReaderPbo()
  : m_fbo(0),
    m_pbo{0, 0},
    m_curPboIndex(0),
    m_width(0),
    m_height(0),
    m_pboBytes(0) {

}

bool PixelReaderPbo::Init() {
  m_curPboIndex = 0;
  m_pboBytes = 0;

  gles::CreateFbo(m_fbo);
  createPbos();

  return true;
}

void PixelReaderPbo::DeInit() {
  deletePbos();
  gles::DeleteFbo(m_fbo);
  m_pboBytes = 0;
  m_curPboIndex = 0;
}

void PixelReaderPbo::Update(const std::size_t width, const std::size_t height) {
  m_width = width;
  m_height = height;

  std::size_t bytes = width * height * 4;

  if (m_pboBytes != bytes) {
    m_pboBytes = bytes;
    gles::UploadXboData(m_pbo[0], GL_PIXEL_PACK_BUFFER, GL_STATIC_READ, m_pboBytes, nullptr);
    gles::UploadXboData(m_pbo[1], GL_PIXEL_PACK_BUFFER, GL_STATIC_READ, m_pboBytes, nullptr);
  }
}

void PixelReaderPbo::Read(std::shared_ptr<Texture> tex, std::shared_ptr<Buffer> buf) {
  assert(tex != nullptr);
  assert(buf != nullptr);
  assert(tex->Width() == buf->width && tex->Height() == buf->height);
  assert(*buf && buf->bytes == m_pboBytes);

  long lastTime = Utils::CurTimeMilli();

  readFromPbos(tex, buf);

  Log::n(target, "pixel data produce %d ms", (Utils::CurTimeMilli() - lastTime));
}

void PixelReaderPbo::createPbos() {
  gles::CreateXbo(m_pbo[0]);
  gles::CreateXbo(m_pbo[1]);
}

void PixelReaderPbo::deletePbos() {
  gles::DeleteXbo(m_pbo[0]);
  gles::DeleteXbo(m_pbo[1]);
}

void PixelReaderPbo::readFromPbos(const std::shared_ptr<Texture> &tex,
                                  const std::shared_ptr<Buffer> &buf) {
  glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

  glActiveTexture(Constants::PROCESS_TEXTURE_UNIT);
  glBindTexture(GL_TEXTURE_2D, tex->Id());
  glFramebufferTexture2D(GL_FRAMEBUFFER,
                         GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D,
                         tex->Id(),
                         0);
  glViewport(0, 0, tex->Width(), tex->Height());

  //绑定到第一个PBO
  glBindBuffer(GL_PIXEL_PACK_BUFFER, m_pbo[m_curPboIndex]);
  glReadBuffer(GL_COLOR_ATTACHMENT0);

  // 读取，立即返回
  glReadPixels(0,
               0,
               tex->Width(),
               tex->Height(),
               GL_RGBA,
               GL_UNSIGNED_BYTE, nullptr);

  // 绑定到第二个PBO
  glBindBuffer(GL_PIXEL_PACK_BUFFER, m_pbo[(m_curPboIndex + 1) % s_pboCount]);

  // 映射内存 glMapBufferRange 会等待DMA传输完成，所以需要交替使用 pbo
  auto data = glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, m_pboBytes, GL_MAP_READ_BIT);

  std::memcpy(buf->data.get(), (Buffer::DataPtrType) data, m_pboBytes);

//  Utils::PrintPixel(buf->data.get(), m_pboBytes);

  //解除映射
  glUnmapBuffer(GL_PIXEL_PACK_BUFFER);

  //解除绑定PBO
  glBindTexture(GL_TEXTURE_2D, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
  glBindBuffer(GL_PIXEL_PACK_BUFFER, GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);

  //交换索引
  m_curPboIndex = (m_curPboIndex + 1) % s_pboCount;
}
