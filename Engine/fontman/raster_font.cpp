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

#include <fmt/fmt_format.h>
#include <graphics/gl_renderer.h>
#include <common_features/logger.h>
#include <IniProcessor/ini_processing.h>
#include <Utils/files.h>
#include <DirManager/dirman.h>

#include "font_manager_private.h"

RasterFont::RasterFont()
{
    letter_width    = 0;
    letter_height   = 0;
    space_width     = 0;
    interletter_space = 0;
    newline_offset  = 0;
    matrix_width    = 0;
    matrix_height   = 0;
    isReady         = false;
    ttf_borders     = false;
    fontName        = fmt::format("font{0}", rand());
}

RasterFont::RasterFont(const RasterFont &rf)
{
    letter_width      = rf.letter_width;
    letter_height     = rf.letter_height;
    interletter_space = rf.interletter_space;
    space_width       = rf.space_width;
    newline_offset    = rf.newline_offset;
    matrix_width      = rf.matrix_width;
    matrix_height     = rf.matrix_height;
    isReady           = rf.isReady;
    ttf_borders       = rf.ttf_borders;
    fontMap           = rf.fontMap;
    textures          = rf.textures;
    fontName          = rf.fontName;
}

RasterFont::~RasterFont()
{
    for(PGE_Texture &t : textures)
        GlRenderer::deleteTexture(t);
    textures.clear();
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
    font.read("name", fontName, fontName);
    font.read("ttf-borders", ttf_borders, false);
    font.read("space-width", space_width, 0);
    font.read("interletter-space", interletter_space, 0);
    font.read("newline-offset", newline_offset, 0);
    font.endGroup();
    std::vector<std::string> tables_list;
    tables_list.reserve(tables);

    font.beginGroup("tables");

    for(size_t i = 1; i <= tables; i++)
    {
        std::string table;
        font.read(fmt::format("table{0}", i).c_str(), table, "");
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
    int w = letter_width, h = letter_height;
    font.beginGroup("font-map");
    font.read("texture", texFile, "");
    font.read("width", w, 0);
    font.read("height", h, 0);
    matrix_width = w;
    matrix_height = h;

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

    textures.push_back(fontTexture);
    PGE_Texture *loadedTexture = &textures.back();

    if((letter_width == 0) || (letter_height == 0))
    {
        letter_width    = fontTexture.w / w;
        letter_height   = fontTexture.h / h;

        if(space_width == 0)
            space_width = letter_width;

        if(newline_offset == 0)
            newline_offset = letter_height;
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

        if(matrix_width > 1)
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
            rch.l               =  std::stof(charPosY.c_str()) / matrix_width;
            rch.r               = (std::stof(charPosY.c_str()) + 1.0f) / matrix_width;
            rch.padding_left    = (ucharX.size() > 1) ? char2int(ucharX[1]) : 0;
            rch.padding_right   = (ucharX.size() > 2) ? char2int(ucharX[2]) : 0;
            rch.t               =  std::stof(charPosX.c_str()) / matrix_height;
            rch.b               = (std::stof(charPosX.c_str()) + 1.0f) / matrix_height;
            rch.valid = true;
        #ifndef DEBUG_BUILD
        }
        catch(std::exception &e)
        {
            pLogWarning("Invalid entry of font map: entry: %s, reason: %s, file: %s", x.c_str(), e.what(), fontmap_ini.c_str());
        }
        #endif

        fontMap[ch] = rch;
    }

    font.endGroup();

    if(!fontMap.empty())
        isReady = true;
}


PGE_Size RasterFont::textSize(std::string &text, int max_line_lenght, bool cut)
{
    if(text.empty())
        return PGE_Size(0, 0);

    size_t lastspace = 0; //!< index of last found space character
    size_t count     = 1; //!< Count of lines
    int maxWidth     = 0; //!< detected maximal width of message

    int widthSumm    = 0;
    int widthSummMax = 0;

    if(cut)
    {
        std::string::size_type i = text.find('\n');
        if(i != std::string::npos)
            text.erase(i, text.size() - i);
    }

    /****************Word wrap*********************/
    int x = 0;
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
            size_t spaceSize = static_cast<size_t>(space_width + interletter_space / 2);
            size_t tabMult = 4 - ((static_cast<size_t>(widthSumm) / spaceSize) % 4);
            widthSumm += static_cast<size_t>(space_width + interletter_space / 2) * tabMult;
            if(widthSumm > widthSummMax)
                widthSummMax = widthSumm;
            break;
        }

        case ' ':
        {
            lastspace = i;
            widthSumm += space_width + interletter_space / 2;
            if(widthSumm > widthSummMax)
                widthSummMax = widthSumm;
            break;
        }

        case '\n':
        {
            lastspace = 0;
            if((maxWidth < x) && (maxWidth < max_line_lenght)) maxWidth = x;
            x = 0;
            widthSumm = 0;
            count++;
            break;
        }

        default:
        {
            CharMap::iterator rc = fontMap.find(get_utf8_char(&cx));
            if(rc != fontMap.end())
            {
                RasChar &rch = rc->second;
                widthSumm += (letter_width - rch.padding_left - rch.padding_right + interletter_space);
                if(widthSumm > widthSummMax)
                    widthSummMax = widthSumm;
            }
            else
            {
                widthSumm += (letter_width + interletter_space);
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
        maxWidth = static_cast<int>(text.length());

    /****************Word wrap*end*****************/
    return PGE_Size(widthSummMax, newline_offset * static_cast<int>(count));
}


void RasterFont::printText(const std::string &text,
                           int x, int y,
                           float Red, float Green, float Blue, float Alpha)
{
    if(fontMap.empty() || text.empty())
        return;

    int offsetX = 0;
    int offsetY = 0;
    GLint w = letter_width;
    GLint h = letter_height;

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
            offsetY += newline_offset;
            continue;

        case '\t':
            offsetX += offsetX + offsetX % w;
            continue;

        case ' ':
            offsetX += space_width + interletter_space / 2;
            continue;
        }

        RasChar rch = fontMap[get_utf8_char(strIt)];
        if(rch.valid)
        {
            GlRenderer::BindTexture(rch.tx);
            GlRenderer::setTextureColor(Red, Green, Blue, Alpha);
            GlRenderer::renderTextureCur(x + offsetX - rch.padding_left,
                                         y + offsetY,
                                         w,
                                         h,
                                         rch.t,
                                         rch.b,
                                         rch.l,
                                         rch.r);
            offsetX += w - rch.padding_left - rch.padding_right + interletter_space;
        }
        else
        {
            #ifdef PGE_TTF
            PGE_Texture c;
            if(ttf_borders)
                FontManager::getChar2(cx, letter_width, c);
            else
                FontManager::getChar1(cx, letter_width, c);
            GlRenderer::setTextureColor(Red, Green, Blue, Alpha);
            GlRenderer::renderTexture(&c, x + offsetX, y + offsetY);
            offsetX += c.w + interletter_space;
            #else
            offsetX += interletter_space;
            #endif
        }
        strIt += static_cast<size_t>(trailingBytesForUTF8[ucx]);
    }

    GlRenderer::UnBindTexture();
}

bool RasterFont::isLoaded()
{
    return isReady;
}

std::string RasterFont::getFontName()
{
    return fontName;
}

