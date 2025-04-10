/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2025 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
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

extern bool initializeFreeType();
extern void closeFreeType();

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
                      uint32_t max_line_length = 0,
                      bool cut = false, uint32_t fontSize = 14);

    void printText(const std::string &text,
                   int32_t x, int32_t y,
                   float Red = 1.f, float Green = 1.f, float Blue = 1.f, float Alpha = 1.f,
                   uint32_t fontSize = 14);

    bool isLoaded();

    std::string getFontName();

    /**
     * @brief Draw a single glyph
     * @param u8char Pointer to UTF8 multi-byte character
     * @param x X position to draw
     * @param y Y position do draw
     * @param fontSize Size of font
     * @param scaleSize Scale rendered texture
     * @param Red Red color level from 0.0 to 1.0
     * @param Green Green color level from 0.0 to 1.0
     * @param Blue Blue color level from 0.0 to 1.0
     * @param Alpha Transparency level from 0.0 to 1.0
     * @return Width of the glypth
     */
    uint32_t drawGlyph(const char* u8char,
                       int32_t x, int32_t y, uint32_t fontSize, double scaleSize = 1.0,
                       float Red = 1.f, float Green = 1.f, float Blue = 1.f, float Alpha = 1.f);

    struct TheGlyphInfo
    {
        uint32_t width  = 0;
        uint32_t height = 0;
        int32_t  left   = 0;
        int32_t  top    = 0;
        int32_t  advance = 0;
        FT_Pos   glyph_width = 0;
    };

    TheGlyphInfo getGlyphInfo(const char *u8char, uint32_t fontSize);

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
        TheGlyph() = default;
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
