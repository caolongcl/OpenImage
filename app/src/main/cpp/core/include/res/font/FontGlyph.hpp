//
// Created by caolong on 2020/8/14.
//

#pragma once

#include <softarch/VarType.hpp>
#include <render/texture/Texture.hpp>

namespace clt {

    /**
     * 单个字符的字形信息
     * 只支持128个ASCII码
     */
    struct CharGlyph {
        CharGlyph() = default;

        ~CharGlyph() = default;

        char c;
        int index;

        bool valid{false};
        Integer2 size{0, 0};    // 字形大小
        Integer2 bearing{0, 0}; // 从基准线到字形左部/顶部的偏移值
        Integer2 advance{0, 0}; // 原点距下一个字形原点的距离
        int pitch{0}; // 一行的长度包括padding
        Texture::BufferPtr buffer{nullptr};
    };

/**
   * 将所有字符拼接成一个纹理
   */
    class CharsGroupGlyph {
    public:
        CharsGroupGlyph() = default;

        ~CharsGroupGlyph() = default;

        /**
         * 添加一个字符 glyph
         * @param charGlyph
         */
        void AddChar(const CharGlyph &charGlyph);

        /**
         * 将字符bitmap合并成大的纹理
         * @param texture
         * @return
         */
        bool Merge(const std::shared_ptr<Texture> &texture);

        /**
         * 获取单个字符字形信息
         * @param c
         * @return
         */
        const CharGlyph &GetCharGlyph(const char c);

        /**
         * 获取每个字符容器的大小
         * @return
         */
        const Float2 &GetCharRectSize();

        /**
         * 获取拼接的bimap的大小
         * @return
         */
        const Float2 &GetCharGroupSize();

    private:
        std::map<const char, CharGlyph> m_charsGlyph;
        Float2 m_charRectSize{0, 0}; // 每一个字符容器大小
        Float2 m_charGroupSize{0, 0}; // 拼接后大小
    };

}
