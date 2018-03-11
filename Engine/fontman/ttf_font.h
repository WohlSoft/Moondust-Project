/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TTF_FONT_H
#define TTF_FONT_H


#include <unordered_map>
#include <Utils/vptrlist.h>
#include <common_features/pge_texture.h>

#ifdef HAVE_STDINT_H
#undef HAVE_STDINT_H //Avoid a warning about HAVE_STDINT_H's redefinition
#endif
#include <freetype2/ft2build.h>
#include <freetype2/freetype/freetype.h>
#include <freetype2/freetype/ftglyph.h>
#include <freetype2/freetype/ftoutln.h>
#include <freetype2/freetype/fttrigon.h>

#include "font_engine_base.h"

//! FreeType library descriptor
extern FT_Library  g_ft;

bool initializeFreeType();
void closeFreeType();

class TtfFont : public BaseFontEngine
{
public:
    TtfFont();
    TtfFont(const TtfFont &tf) = delete;
    TtfFont &operator=(const TtfFont &tf) = delete;
    TtfFont(TtfFont &&tf) = default;
    virtual ~TtfFont();

    bool loadFont(const std::string &path);
    bool loadFont(const char* mem, size_t size);


    PGE_Size textSize(std::string &text,
                      uint32_t max_line_lenght = 0,
                      bool cut = false, uint32_t fontSize = 14);

    void printText(const std::string &text,
                   int32_t x, int32_t y,
                   float Red = 1.f, float Green = 1.f, float Blue = 1.f, float Alpha = 1.f,
                   uint32_t fontSize = 14);

    bool isLoaded();

    std::string getFontName();

private:
    //! font is fine
    bool m_isReady = false;
    //! Handalable name of the font
    std::string m_fontName;

    //! Recently used pixel size of the font
    uint32_t     m_recentPixelSize = 10;

    //! Загруженное лицо шрифта
    FT_Face      m_face = nullptr;

    struct TheGlyph
    {
        TheGlyph();
        PGE_Texture *tx     = nullptr;
        uint32_t width  = 0;
        uint32_t height = 0;
        int32_t  left   = 0;
        int32_t  top    = 0;
        int32_t  advance = 0;
        FT_Pos   glyph_width = 0;
    };

    //! Default dummy glyph
    static const TheGlyph dummyGlyph;

    const TheGlyph &getGlyph(uint32_t fontSize, char32_t character);
    const TheGlyph &loadGlyph(uint32_t fontSize, char32_t character);

    typedef std::unordered_map<char32_t, TheGlyph> CharMap;
    typedef std::unordered_map<uint32_t, CharMap>  SizeCharMap;

    SizeCharMap m_charMap;
    VPtrList<PGE_Texture > m_texturesBank;
};

#endif // TTF_FONT_H
