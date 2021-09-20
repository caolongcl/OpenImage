//
// Created by caolong on 2020/8/18.
//

#pragma once

#include <softarch/std.hpp>
#include <softarch/IComFunc.hpp>

namespace clt {

  class Texture;

  class Buffer;

  /**
   * 读取像素的方法
   */
  struct IPixelReader : public IComFunc<>,
                        public IComUpdate<const std::size_t, const std::size_t> {

    IPixelReader() = default;

    virtual ~ IPixelReader() = default;

    /**
     * 从纹理中读取数据到buf中
     * @param id
     * @param buf
     */
    virtual void Read(std::shared_ptr<Texture> tex, std::shared_ptr<Buffer> buf) = 0;
  };

}
