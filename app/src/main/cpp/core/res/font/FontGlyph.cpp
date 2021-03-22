//
// Created by caolong on 2020/8/14.
//

#include <res/font/FontGlyph.hpp>

using namespace clt;

void CharsGroupGlyph::AddChar(const CharGlyph &charGlyph) {
    m_charsGlyph.emplace(charGlyph.c, charGlyph);
}

bool CharsGroupGlyph::Merge(const std::shared_ptr<Texture> &texture) {
    int maxWidth = 0;
    int maxHeight = 0;

    // 计算所有glyph最大宽高
    for (auto &g : m_charsGlyph) {
        const auto &glyph = g.second;

        if (glyph.pitch > maxWidth)
            maxWidth = glyph.pitch;
        if (glyph.size.h > maxHeight)
            maxHeight = g.second.size.h;
    }
    m_charRectSize.w = static_cast<float>(maxWidth);
    m_charRectSize.h = static_cast<float>(maxHeight);

    int count = m_charsGlyph.size();
    int pitch = (count * maxWidth + 3) / 4 * 4;
    int bytes = pitch * maxHeight;

    m_charGroupSize.w = static_cast<float>(pitch);
    m_charGroupSize.h = static_cast<float>(maxHeight);

    Log::d(Log::RES_TAG, "Glyph count %d max(%d %d) bytes %d; merge wh(%d %d)",
           count, maxWidth, maxHeight, bytes, pitch, maxHeight);

    //
    Texture::BufferPtr
            buffer(new Texture::Buffer[bytes]{0},
                   [](const Texture::Buffer *const b) { delete[] b; });

    // 将count个小Bitmap拼成一个宽pitch高maxHeight的长条bitmap
    for (auto &g : m_charsGlyph) {
        auto &glyph = g.second;
        int index = glyph.index;

        if (glyph.buffer != nullptr) {
            for (int j = 0; j < glyph.size.h; ++j) { // glyph的高
                for (int i = 0; i < glyph.pitch; ++i) {// glyph的宽
                    buffer.get()[j * pitch + index * maxWidth + i] = glyph.buffer.get()[
                            j * glyph.pitch + i];
                }
            }
            glyph.buffer = nullptr;
        }
    }

    // 上传到texture中
    Log::v(Log::RES_TAG, "font texture upload");
    texture->Upload(pitch, maxHeight, buffer);

    return true;
}

const CharGlyph &CharsGroupGlyph::GetCharGlyph(const char c) {
    return m_charsGlyph[c];
}

const Float2 &CharsGroupGlyph::GetCharRectSize() {
    return m_charRectSize;
}

const Float2 &CharsGroupGlyph::GetCharGroupSize() {
    return m_charGroupSize;
}