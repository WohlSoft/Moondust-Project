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

#include "raster_font.h"

#include <graphics/gl_renderer.h>
#include <common_features/fmt_format_ne.h>
#include <common_features/logger.h>
#include <IniProcessor/ini_processing.h>
#include <Utils/files.h>
#include <DirManager/dirman.h>

#include "ttf_font.h"
#include "font_manager_private.h"

RasterFont::RasterFont() : BaseFontEngine()
{
    static uint64_t fontNumberCount = 0;//This number will be used as default font name
    m_letterWidth    = 0;
    m_letterHeight   = 0;
    m_spaceWidth     = 0;
    m_interLetterSpace = 0;
    m_newlineOffset  = 0;
    m_matrixWidth    = 0;
    m_matrixHeight   = 0;
    m_isReady        = false;
    m_ttfBorders     = false;
    m_fontName       = fmt::format_ne("font{0}", fontNumberCount++);
}

RasterFont::RasterFont(const RasterFont &rf) : BaseFontEngine(rf)
{
    m_letterWidth      = rf.m_letterWidth;
    m_letterHeight     = rf.m_letterHeight;
    m_interLetterSpace = rf.m_interLetterSpace;
    m_spaceWidth       = rf.m_spaceWidth;
    m_newlineOffset    = rf.m_newlineOffset;
    m_matrixWidth      = rf.m_matrixWidth;
    m_matrixHeight     = rf.m_matrixHeight;
    m_isReady          = rf.m_isReady;
    m_ttfBorders       = rf.m_ttfBorders;
    m_charMap          = rf.m_charMap;
    m_texturesBank     = rf.m_texturesBank;
    m_fontName         = rf.m_fontName;
}

RasterFont::~RasterFont()
{
    for(PGE_Texture &t : m_texturesBank)
        GlRenderer::deleteTexture(t);
    m_texturesBank.clear();
}

void RasterFont::loadFont(std::string font_ini)
{
    if(!Files::fileExists(font_ini))
    {
        pLogWarning("Can't load font %s: file not exist", font_ini.c_str());
        return;
    }

    std::string root = DirMan(Files::dirname(font_ini)).absolutePath() + "/";
    IniProcessing font(font_ini);

    size_t tables = 0;
    font.beginGroup("font");
    font.read("tables", tables, 0);
    font.read("name", m_fontName, m_fontName);
    font.read("ttf-borders", m_ttfBorders, false);
    font.read("space-width", m_spaceWidth, 0);
    font.read("interletter-space", m_interLetterSpace, 0);
    font.read("newline-offset", m_newlineOffset, 0);
    font.endGroup();
    std::vector<std::string> tables_list;
    tables_list.reserve(tables);

    font.beginGroup("tables");

    for(size_t i = 1; i <= tables; i++)
    {
        std::string table;
        font.read(fmt::format_ne("table{0}", i).c_str(), table, "");
        if(!table.empty())
            tables_list.push_back(table);
    }

    font.endGroup();

    for(std::string &tbl : tables_list)
        loadFontMap(root + tbl);
}

void RasterFont::loadFontMap(std::string fontmap_ini)
{
    std::string root = DirMan(Files::dirname(fontmap_ini)).absolutePath() + "/";

    if(!Files::fileExists(fontmap_ini))
    {
        pLogWarning("Can't load font map %s: file not exist", fontmap_ini.c_str());
        return;
    }

    IniProcessing font(fontmap_ini);
    std::string texFile;
    uint32_t w = m_letterWidth, h = m_letterHeight;
    font.beginGroup("font-map");
    font.read("texture", texFile, "");
    font.read("width", w, 0);
    font.read("height", h, 0);
    m_matrixWidth = w;
    m_matrixHeight = h;

    if((w <= 0) || (h <= 0))
    {
        pLogWarning("Wrong width and height values! %d x %d",  w, h);
        return;
    }

    if(!Files::fileExists(root + texFile))
    {
        pLogWarning("Failed to load font texture! file not exists: %s",
                    (root + texFile).c_str());
        return;
    }

    PGE_Texture fontTexture;
    GlRenderer::loadTextureP(fontTexture, root + texFile);

    if(!fontTexture.inited)
        pLogWarning("Failed to load font texture! Invalid image!");

    m_texturesBank.push_back(fontTexture);
    PGE_Texture *loadedTexture = &m_texturesBank.back();

    if((m_letterWidth == 0) || (m_letterHeight == 0))
    {
        m_letterWidth    = static_cast<uint32_t>(fontTexture.w) / w;
        m_letterHeight   = static_cast<uint32_t>(fontTexture.h) / h;

        if(m_spaceWidth == 0)
            m_spaceWidth = m_letterWidth;

        if(m_newlineOffset == 0)
            m_newlineOffset = m_letterHeight;
    }

    font.endGroup();
    font.beginGroup("entries");
    std::vector<std::string> entries = font.allKeys();

    for(std::string &x : entries)
    {
        std::string charPosX = "0", charPosY = "0";

        std::string::size_type begPos = 0;
        std::string::size_type endPos = x.find('-', begPos);

        //QStringList tmp = x.split('-');
        if(endPos == std::string::npos)
            endPos = x.size();//Use entire string

        charPosX = x.substr(begPos, endPos);

        if(charPosX.empty())
        {
            pLogDebug("=invalid-X=%d=", x.c_str());
            continue;
        }

        if(m_matrixWidth > 1)
        {
            if(endPos == x.size())
            {
                pLogWarning("=invalid-Y=%d= in the raster font map %s", x.c_str(), fontmap_ini.c_str());
                continue;
            }
            begPos = endPos + 1;//+1 to skip '-' divider
            endPos = x.find('-', begPos);
            if(endPos == std::string::npos)
                endPos = x.size();//Use entire string
            charPosY = x.substr(begPos, endPos);
            if(charPosY.empty())
            {
                pLogWarning("=invalid-Y=%d= in the raster font map %s", x.c_str(), fontmap_ini.c_str());
                continue;
            }
        }

        std::string charX;
        font.read(x.c_str(), charX, "");

        /*Format of entry: X23
         * X - UTF-8 Symbol
         * 2 - padding left [for non-mono fonts]
         * 3 - padding right [for non-mono fonts]
        */
        if(charX.empty())
            continue;

        std::u32string ucharX = std_to_utf32(charX);
        char32_t ch = ucharX[0];
        //qDebug()<<"=char=" << ch << "=id="<<charPosX.toInt()<<charPosY.toInt()<<"=";
        RasChar rch;
        #ifndef DEBUG_BUILD
        try
        {
        #endif
            rch.tx              =  loadedTexture;
            rch.l               =  std::stof(charPosY.c_str()) / m_matrixWidth;
            rch.r               = (std::stof(charPosY.c_str()) + 1.0f) / m_matrixWidth;
            rch.padding_left    = (ucharX.size() > 1) ? char2int(ucharX[1]) : 0;
            rch.padding_right   = (ucharX.size() > 2) ? char2int(ucharX[2]) : 0;
            rch.t               =  std::stof(charPosX.c_str()) / m_matrixHeight;
            rch.b               = (std::stof(charPosX.c_str()) + 1.0f) / m_matrixHeight;
            rch.valid = true;
        #ifndef DEBUG_BUILD
        }
        catch(std::exception &e)
        {
            pLogWarning("Invalid entry of font map: entry: %s, reason: %s, file: %s", x.c_str(), e.what(), fontmap_ini.c_str());
        }
        #endif

        m_charMap[ch] = rch;
    }

    font.endGroup();

    if(!m_charMap.empty())
        m_isReady = true;
}


PGE_Size RasterFont::textSize(std::string &text, uint32_t max_line_lenght, bool cut, uint32_t)
{
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
            lastspace = i;
            size_t spaceSize = m_spaceWidth + m_interLetterSpace / 2;
            size_t tabMult = 4 - ((widthSumm / spaceSize) % 4);
            widthSumm += static_cast<size_t>(m_spaceWidth + m_interLetterSpace / 2) * tabMult;
            if(widthSumm > widthSummMax)
                widthSummMax = widthSumm;
            break;
        }

        case ' ':
        {
            lastspace = i;
            widthSumm += m_spaceWidth + m_interLetterSpace / 2;
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
            CharMap::iterator rc = m_charMap.find(get_utf8_char(&cx));
            if(rc != m_charMap.end())
            {
                RasChar &rch = rc->second;
                if(rch.valid)
                {
                    widthSumm += (m_letterWidth - rch.padding_left - rch.padding_right + m_interLetterSpace);
                }
                else
                {
                    TtfFont *font = reinterpret_cast<TtfFont*>(FontManager::getDefaultTtfFont());
                    if(font)
                    {
                        TtfFont::TheGlyphInfo glyph = font->getGlyphInfo(&cx, m_letterWidth);
                        widthSumm += glyph.width > 0 ? uint32_t(glyph.advance >> 6) : (m_letterWidth >> 2);
                    } else {
                        widthSumm += m_letterWidth + m_interLetterSpace;
                    }
                }
                if(widthSumm > widthSummMax)
                    widthSummMax = widthSumm;
            }
            else
            {
                widthSumm += (m_letterWidth + m_interLetterSpace);
                if(widthSumm > widthSummMax)
                    widthSummMax = widthSumm;
            }
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
    return PGE_Size(static_cast<int32_t>(widthSummMax), static_cast<int32_t>(m_newlineOffset * count));
}

void RasterFont::printText(const std::string &text,
                           int32_t x, int32_t y,
                           float Red, float Green, float Blue, float Alpha,
                           uint32_t)
{
    if(m_charMap.empty() || text.empty())
        return;

    uint32_t offsetX = 0;
    uint32_t offsetY = 0;
    uint32_t w = m_letterWidth;
    uint32_t h = m_letterHeight;

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
            offsetY += m_newlineOffset;
            continue;

        case '\t':
            //Fake tabulation
            offsetX += offsetX + offsetX % w;
            continue;

        case ' ':
            offsetX += m_spaceWidth + m_interLetterSpace / 2;
            continue;
        }

        RasChar rch = m_charMap[get_utf8_char(strIt)];
        if(rch.valid)
        {
            GlRenderer::BindTexture(rch.tx);
            GlRenderer::setTextureColor(Red, Green, Blue, Alpha);
            GlRenderer::renderTextureCur(x + static_cast<int32_t>(offsetX - rch.padding_left),
                                         y + static_cast<int32_t>(offsetY),
                                         w,
                                         h,
                                         rch.t,
                                         rch.b,
                                         rch.l,
                                         rch.r);
            offsetX += w - rch.padding_left - rch.padding_right + m_interLetterSpace;
        }
        else
        {
            TtfFont *font = reinterpret_cast<TtfFont*>(FontManager::getDefaultTtfFont());
            if(font)
            {
                font->drawGlyph(&cx,
                                x + static_cast<int32_t>(offsetX),
                                y + static_cast<int32_t>(offsetY),
                                w,
                                Red, Green, Blue, Alpha);
                offsetX += w + m_interLetterSpace;
            } else {
                offsetX += m_interLetterSpace;
            }
        }
        strIt += static_cast<size_t>(trailingBytesForUTF8[ucx]);
    }

    GlRenderer::UnBindTexture();
}

bool RasterFont::isLoaded()
{
    return m_isReady;
}

std::string RasterFont::getFontName()
{
    return m_fontName;
}

