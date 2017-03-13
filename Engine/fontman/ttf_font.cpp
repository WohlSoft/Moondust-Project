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

#include "ttf_font.h"
#include <SDL2/SDL_assert.h>
#include <graphics/gl_renderer.h>
#include <common_features/logger.h>

#include "font_manager_private.h"

//! FreeType library descriptor
FT_Library  g_ft = nullptr;


bool initializeFreeType()
{
    FT_Error error = FT_Init_FreeType(&g_ft);
    SDL_assert_release(error == 0);
    return true;
}

void closeFreeType()
{
    if(g_ft)
    {
        FT_Done_FreeType(g_ft);
        g_ft = nullptr;
    }
}




TtfFont::TtfFont() : BaseFontEngine()
{
    SDL_assert_release(g_ft);
}

TtfFont::~TtfFont()
{
    for(PGE_Texture &t : m_texturesBank)
        GlRenderer::deleteTexture(t);
    m_texturesBank.clear();

    if(m_face)
        FT_Done_Face(m_face);

    m_face = nullptr;
}

bool TtfFont::loadFont(const std::string &path)
{
    SDL_assert_release(g_ft);
    FT_Error error = FT_New_Face(g_ft, path.c_str(), 0, &m_face);
    SDL_assert(error == 0);
    if(error)
        return false;
    error = FT_Set_Pixel_Sizes(m_face, 0, m_pixelSize);
    SDL_assert(error == 0);
    if(error)
        return false;
    error = FT_Select_Charmap(m_face, ft_encoding_unicode);
    SDL_assert(error == 0);
    if(error)
        return false;

    m_fontName.append(m_face->family_name);
    m_fontName.push_back(' ');
    m_fontName.append(m_face->style_name);
    m_isReady = true;
    return true;
}

bool TtfFont::loadFont(const char *mem, size_t size)
{
    SDL_assert_release(g_ft);
    FT_Error error = FT_New_Memory_Face(g_ft, reinterpret_cast<const FT_Byte *>(mem),
                                        static_cast<FT_Long>(size), 0, &m_face);
    SDL_assert(error == 0);
    if(error)
        return false;
    error = FT_Set_Pixel_Sizes(m_face, 0, m_pixelSize);
    SDL_assert(error == 0);
    if(error)
        return false;
    error = FT_Select_Charmap(m_face, ft_encoding_unicode);
    SDL_assert(error == 0);
    if(error)
        return false;
    m_isReady = true;
    return true;
}

PGE_Size TtfFont::textSize(std::string &text,
                           uint32_t max_line_lenght,
                           bool cut, uint32_t fontSize)
{
    SDL_assert_release(g_ft);
    if(text.empty())
        return PGE_Size(0, 0);

    size_t lastspace = 0; //!< index of last found space character
    size_t count     = 1; //!< Count of lines
    uint32_t maxWidth     = 0; //!< detected maximal width of message

    uint32_t widthSumm    = 0;
    uint32_t widthSummMax = 0;

    if(cut)
    {
        std::string::size_type i = text.find('\n');
        if(i != std::string::npos)
            text.erase(i, text.size() - i);
    }

    /****************Word wrap*********************/
    uint32_t x = 0;
    for(size_t i = 0; i < text.size(); i++, x++)
    {
        char &cx = text[i];
        UTF8 uch = static_cast<unsigned char>(cx);

        switch(cx)
        {
        case '\r':
            break;

        case '\t':
        {
            //Fake tabulation
            size_t space = (4 * fontSize);
            widthSumm += (space - ((widthSumm / space) % 4));
            if(widthSumm > widthSummMax)
                widthSummMax = widthSumm;
            break;
        }

        case '\n':
        {
            lastspace = 0;
            if((maxWidth < x) && (maxWidth < max_line_lenght))
                maxWidth = x;
            x = 0;
            widthSumm = 0;
            count++;
            break;
        }

        default:
        {
            TheGlyph &glyph = getGlyph(fontSize, get_utf8_char(&cx));
            widthSumm += (glyph.advance>>6);
            if(widthSumm > widthSummMax)
                widthSummMax = widthSumm;
            break;
        }

        }//Switch

        if((max_line_lenght > 0) && (x >= max_line_lenght)) //If lenght more than allowed
        {
            maxWidth = x;
            if(lastspace > 0)
            {
                text[lastspace] = '\n';
                i = lastspace - 1;
                lastspace = 0;
            }
            else
            {
                text.insert(i, 1, '\n');
                x = 0;
                count++;
            }
        }
        i += static_cast<size_t>(trailingBytesForUTF8[uch]);
    }

    if(count == 1)
        maxWidth = static_cast<uint32_t>(text.length());

    /****************Word wrap*end*****************/
    return PGE_Size(static_cast<int32_t>(widthSummMax), static_cast<int32_t>((fontSize * 1.5) * count));
}

void TtfFont::printText(const std::string &text,
                        int32_t x, int32_t y,
                        float Red, float Green, float Blue, float Alpha,
                        uint32_t fontSize)
{
    SDL_assert_release(g_ft);
    if(text.empty())
        return;

    uint32_t offsetX = 0;
    uint32_t offsetY = 0;

    const char *strIt  = text.c_str();
    const char *strEnd = strIt + text.size();
    for(; strIt < strEnd; strIt++)
    {
        const char &cx = *strIt;
        UTF8 ucx = static_cast<unsigned char>(cx);

        switch(cx)
        {
        case '\n':
            offsetX = 0;
            offsetY += (fontSize * 1.5);
            continue;

        case '\t':
            //Fake tabulation
            offsetX += offsetX + offsetX % uint32_t(fontSize * 1.5);
            continue;
//        case ' ':
//            offsetX += m_spaceWidth + m_interLetterSpace / 2;
//            continue;
        }

        TheGlyph &glyph = getGlyph(fontSize, get_utf8_char(&cx));
        GlRenderer::setTextureColor(Red, Green, Blue, Alpha);
        int32_t glyph_x = x + static_cast<int32_t>(offsetX);
        int32_t glyph_y = y + static_cast<int32_t>(offsetY + fontSize);
        GlRenderer::renderTexture(glyph.tx,
                                  static_cast<float>(glyph_x + glyph.left),
                                  static_cast<float>(glyph_y - glyph.top),
                                  glyph.width,
                                  glyph.height
                                  );
        offsetX += (glyph.advance>>6);

        strIt += static_cast<size_t>(trailingBytesForUTF8[ucx]);
    }
}

bool TtfFont::isLoaded()
{
    return m_isReady;
}

std::string TtfFont::getFontName()
{
    return m_fontName;
}

TtfFont::TheGlyph &TtfFont::getGlyph(uint32_t fontSize, char32_t character)
{
    SizeCharMap::iterator fSize = m_charMap.find(fontSize);
    if(fSize != m_charMap.end())
    {
        CharMap::iterator rc = fSize->second.find(character);
        if(rc != fSize->second.end())
            return rc->second;
        return loadGlyph(fontSize, character);
    }

    return loadGlyph(fontSize, character);
}

TtfFont::TheGlyph &TtfFont::loadGlyph(uint32_t fontSize, char32_t character)
{
    FT_Error     error = 0;
    TheGlyph     t_glyph;

    if(m_pixelSize != fontSize)
    {
        error = FT_Set_Pixel_Sizes(m_face, 0, fontSize);
        SDL_assert_release(error == 0);
        m_pixelSize = fontSize;
    }

    error = FT_Load_Char(m_face, character, FT_LOAD_RENDER);
    SDL_assert_release(error == 0); //FIXME: return dummy glypg instead

    FT_GlyphSlot glyph  = m_face->glyph;
    FT_Bitmap &bitmap   = glyph->bitmap;
    uint32_t width      = bitmap.width;
    uint32_t height     = bitmap.rows;

    uint8_t *image = new uint8_t[4 * width * height];
    if(bitmap.pitch >= 0)
    {
        for(uint32_t w = 0; w < width; w++)
        {
            for(uint32_t h = 0; h < height; h++)
            {
                uint8_t color = bitmap.buffer[static_cast<uint32_t>(bitmap.pitch) * ((height - 1) - h) + w];
                image[(4 * width) * (height - 1 - h) + (4 * w)] = color;
                image[(4 * width) * (height - 1 - h) + (4 * w + 1)] = color;
                image[(4 * width) * (height - 1 - h) + (4 * w + 2)] = color;
                image[(4 * width) * (height - 1 - h) + (4 * w + 3)] = color;
            }
        }
    }
    else if(bitmap.pitch < 0)
    {
        for(uint32_t w = 0; w < width; w++)
        {
            for(uint32_t h = 0; h < height; h++)
            {
                uint8_t color = *(bitmap.buffer - (static_cast<uint32_t>(bitmap.pitch) * (h)) + w);
                image[(4 * width)*h + (4 * w)] = color;
                image[(4 * width)*h + (4 * w + 1)] = color;
                image[(4 * width)*h + (4 * w + 2)] = color;
                image[(4 * width)*h + (4 * w + 3)] = color;
            }
        }
    }

    m_texturesBank.emplace_back();
    PGE_Texture &texture = m_texturesBank.back();
    texture.nOfColors   = GL_RGBA;
    texture.format      = GL_BGRA;

    GlRenderer::loadRawTextureP(texture, image, width, height);
    t_glyph.tx      = &texture;
    t_glyph.width   = width;
    t_glyph.height  = height;
    t_glyph.left    = glyph->bitmap_left;
    t_glyph.top     = glyph->bitmap_top;
    t_glyph.advance = glyph->advance.x;
    t_glyph.glyph_width = glyph->advance.x;

    delete [] image;

    SizeCharMap::iterator fSize = m_charMap.find(fontSize);
    if(fSize == m_charMap.end())
    {
        m_charMap.insert({fontSize, CharMap()});
        fSize = m_charMap.find(fontSize);
        SDL_assert_release(fSize != m_charMap.end());
    }

    fSize->second.insert({character, t_glyph});
    CharMap::iterator rc = fSize->second.find(character);
    SDL_assert_release(rc != fSize->second.end());

    return rc->second;
}
