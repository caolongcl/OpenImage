//
// Created by caolong on 2020/8/17.
//

#pragma once

#include <softarch/std.hpp>

namespace clt {

  /**
   * 存储图像数据
   * 比如从 ProcessPipe 管道中读取的纹理像素值
   */
  struct Buffer final {
    using DataType = unsigned char;
    using DataPtrType = unsigned char *;
    using ValueType = std::shared_ptr<DataType>;

    Buffer() : data(nullptr), bytes(0), width(0), height(0), channel(0) {}

    Buffer(std::size_t w, std::size_t h, std::size_t c = 4)
      : width(w), height(h), channel(c) {
      bytes = width * height * channel * sizeof(DataType);

      ValueType
        value(new DataType[bytes], [](const DataType *const b) { delete[] b; });
      data = std::move(value);
    }

    ~Buffer() {
      data = nullptr;
      bytes = 0;
      width = 0;
      height = 0;
      channel = 0;
    }

    Buffer(ValueType &d, std::size_t w, std::size_t h, std::size_t c = 4)
      : width(w), height(h), channel(c) {
      bytes = width * height * channel * sizeof(DataType);

      ValueType
        value(new DataType[bytes], [](const DataType *const b) { delete[] b; });
      data = std::move(value);
      std::memmove(data.get(), d.get(), bytes);
    }

    Buffer(ValueType &&d, std::size_t w, std::size_t h, std::size_t c = 4)
      : data(std::move(d)), width(w), height(h), channel(c) {
      bytes = width * height * channel * sizeof(DataType);
      d = nullptr;
    }

    Buffer(const Buffer &buf)
      : bytes(buf.bytes), width(buf.width), height(buf.height), channel(buf.channel) {
      ValueType
        value(new DataType[bytes], [](const DataType *const b) { delete[] b; });
      data = value;
      std::memmove(data.get(), buf.data.get(), bytes);
    }

    Buffer &operator=(const Buffer &buf) {
      bytes = buf.bytes;
      width = buf.width;
      height = buf.height;
      channel = buf.channel;

      ValueType
        value(new DataType[bytes], [](const DataType *const b) { delete[] b; });
      data = value;
      std::memmove(data.get(), buf.data.get(), bytes);

      return *this;
    }

    Buffer(Buffer &&buf)
      : data(std::move(buf.data)), bytes(buf.bytes), width(buf.width), height(buf.height),
        channel(buf.channel) {
      buf.data = nullptr;
      buf.bytes = 0;
      buf.width = 0;
      buf.height = 0;
      buf.channel = 0;
    }

    Buffer &operator=(Buffer &&buf) {
      data = std::move(buf.data);
      bytes = buf.bytes;
      width = buf.width;
      height = buf.height;
      channel = buf.channel;

      buf.data = nullptr;
      buf.bytes = 0;
      buf.width = 0;
      buf.height = 0;
      buf.channel = 0;

      return *this;
    }

    /**
     * 拷贝buf内容
     * @param buf
     */
    void Copy(const Buffer &buf) {
      assert(identityType(buf));
      std::memcpy(data.get(), buf.data.get(), bytes);
    }

    friend void Copy(Buffer &dst, const Buffer &src) {
      dst.Copy(src);
    }

    bool identityType(const Buffer &buf) const {
      return width == buf.width && height == buf.height && channel == buf.channel;
    }

    operator bool() {
      return data != nullptr && bytes != 0 && width != 0 && height != 0 && channel != 0;
    }

    ValueType data;
    std::size_t bytes;
    std::size_t width;
    std::size_t height;
    std::size_t channel;
  };

}
