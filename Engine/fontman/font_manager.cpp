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

#include "font_manager.h"

#include <common_features/app_path.h>
#include <common_features/graphics_funcs.h>
#include <common_features/logger.h>
#include <data_configs/config_manager.h>
#include <graphics/gl_renderer.h>

#include <Utils/files.h>
#include <DirManager/dirman.h>
#include <IniProcessor/ini_processing.h>
#include <fmt/fmt_format.h>

#include <vector>

#include <common_features/graphics_funcs.h>

#include "font_manager_private.h"

RasterFont              *FontManager::rFont = nullptr;
VPtrList<RasterFont>    FontManager::rasterFonts;

bool                    FontManager::isInit = false;
FontManager::FontsHash  FontManager::fonts;

int     FontManager::fontID;
bool    FontManager::double_pixled = false;

void FontManager::initBasic()
{
    #ifdef PGE_TTF
    if(!defaultFont)
        defaultFont = new QFont();

    fontID = QFontDatabase::addApplicationFont(":/PressStart2P.ttf");
    fontID = QFontDatabase::addApplicationFont(":/JosefinSans-Regular.ttf");
    #endif
    double_pixled = false;

    #ifdef PGE_TTF
    //Josefin Sans
    QString family("Monospace");

    if(!QFontDatabase::applicationFontFamilies(fontID).empty())
        family = QFontDatabase::applicationFontFamilies(fontID).at(0);

    defaultFont->setFamily(family);//font.setWeight(14);
    //#ifdef __APPLE__
    //  defaultFont->setPixelSize(16);
    //#else
    defaultFont->setPixelSize(16);
    //#endif
    defaultFont->setStyleStrategy(QFont::PreferBitmap);
    defaultFont->setLetterSpacing(QFont::AbsoluteSpacing, 1);
    //defaultFont = buildFont_RW(":/PressStart2P.ttf", 14);
    #endif

    isInit = true;
}

void FontManager::initFull()
{
    double_pixled = ConfigManager::setup_fonts.double_pixled;

    /***************Load raster font support****************/
    DirMan fontsDir(ConfigManager::config_dirSTD + "/fonts");

    std::vector<std::string> files;
    fontsDir.getListOfFiles(files, {".font.ini"});
    std::sort(files.begin(), files.end());
    for(std::string &fonFile : files)
    {
        RasterFont rfont;
        rasterFonts.push_back(rfont);
        {
            RasterFont& rf = rasterFonts.back();
            rf.loadFont(fontsDir.absolutePath() + "/" + fonFile);

            if(!rf.isLoaded())   //Pop broken font from array
                rasterFonts.pop_back();
            else   //Register font name in a table
                fonts.insert({rf.getFontName(), rasterFonts.size() - 1});
        }
    }

    if(!rasterFonts.empty())
        rFont = &rasterFonts.front();
}

void FontManager::quit()
{
    fonts.clear();
    rasterFonts.clear();
}

size_t FontManager::utf8_strlen(const char *str)
{
    size_t size = 0;
    while(str)
    {
        size_t  charLen = 1 + static_cast<size_t>(trailingBytesForUTF8[static_cast<UTF8>(*str)]);
        size += charLen;
        str  += charLen;
    }
    return size;
}

size_t FontManager::utf8_strlen(const char *str, size_t len)
{
    size_t utf8_pos = 0;
    while(str && len > 0)
    {
        size_t  charLen = 1 + static_cast<size_t>(trailingBytesForUTF8[static_cast<UTF8>(*str)]);
        str  += charLen;
        len  -= charLen;
        utf8_pos++;
    }
    return utf8_pos;
}

size_t FontManager::utf8_strlen(const std::string &str)
{
    return utf8_strlen(str.c_str(), str.size());
}

size_t FontManager::utf8_substrlen(const std::string &str, size_t utf8len)
{
    size_t pos = 0;
    size_t utf8_pos = 0;
    const char *cstr = str.c_str();
    size_t len = str.length();
    while(cstr && len > 0)
    {
        size_t  charLen = 1 + static_cast<size_t>(trailingBytesForUTF8[static_cast<UTF8>(*cstr)]);
        if(utf8_pos == utf8len)
            return pos;
        cstr += charLen;
        pos  += charLen;
        len  -= charLen;
        utf8_pos++;
    }
    return pos;
}

std::string FontManager::utf8_substr(const std::string &str, size_t utf8_begin, size_t utf8_len)
{
    size_t utf8_pos = 0;
    const char *cstr = str.c_str();
    size_t len = str.length();
    std::string out;
    while(cstr && (len > 0) && (utf8_len > 0))
    {
        size_t  charLen = 1 + static_cast<size_t>(trailingBytesForUTF8[static_cast<UTF8>(*cstr)]);
        if(utf8_pos >= utf8_begin)
        {
            out.append(cstr, charLen);
            utf8_len--;
        }
        cstr += charLen;
        len  -= charLen;
        utf8_pos++;
    }
    return out;
}

void FontManager::utf8_pop_back(std::string &str)
{
    size_t pos = 0;
    const char*cstr = str.c_str();
    size_t len = str.length();
    while(cstr && len > 0)
    {
        size_t  charLen = 1 + static_cast<size_t>(trailingBytesForUTF8[static_cast<UTF8>(*cstr)]);
        len  -= charLen;
        if(len == 0)
        {
            str.erase(str.begin() + static_cast<std::string::difference_type>(pos), str.end());
            return;
        }
        cstr += charLen;
        pos  += charLen;
    }
}

void FontManager::utf8_erase_at(std::string &str, size_t begin)
{
    size_t pos = 0;
    size_t utf8_pos = 0;
    const char*cstr = str.c_str();
    size_t len = str.length();
    while(cstr && len > 0)
    {
        size_t  charLen = 1 + static_cast<size_t>(trailingBytesForUTF8[static_cast<UTF8>(*cstr)]);
        if(utf8_pos == begin)
        {
            str.erase(str.begin() + static_cast<std::string::difference_type>(pos), str.end());
            return;
        }
        cstr += charLen;
        pos  += charLen;
        len  -= charLen;
        utf8_pos++;
    }
}

void FontManager::utf8_erase_before(std::string &str, size_t end)
{
    size_t pos = 0;
    size_t utf8_pos = 0;
    const char*cstr = str.c_str();
    size_t len = str.length();
    while(cstr && len > 0)
    {
        size_t  charLen = 1 + static_cast<size_t>(trailingBytesForUTF8[static_cast<UTF8>(*cstr)]);
        if(utf8_pos == end)
        {
            str.erase(str.begin(), str.begin() + static_cast<std::string::difference_type>(pos) + 1);
            return;
        }
        cstr += charLen;
        pos  += charLen;
        len  -= charLen;
        utf8_pos++;
    }
}

PGE_Size FontManager::textSize(std::string &text, int fontID, int max_line_lenght, bool cut, int ttfFontPixelSize)
{
    assert(isInit && "Font manager is not initialized!");

    if(!isInit)
        return PGE_Size(27 * 20, static_cast<int>(std::count(text.begin(), text.end(), '\n') + 1) * 20);

    if(text.empty())
        return PGE_Size(0, 0);

    if(max_line_lenght <= 0)
        max_line_lenght = 1000;

    if(cut)
    {
        std::string::size_type i = text.find('\n');
        if(i != std::string::npos)
            text.erase(i, text.size() - i);
    }

    //Use Raster font
    if((fontID >= 0) && (static_cast<size_t>(fontID) < rasterFonts.size()))
    {
        if(rasterFonts[fontID].isLoaded())
            return rasterFonts[fontID].textSize(text, max_line_lenght);
    }

    #ifdef PGE_TTF
    //Use TTF font
    QFont fnt = font();

    if(ttfFontPixelSize > 0)
        fnt.setPixelSize(ttfFontPixelSize);

    QFontMetrics meter(fnt);
    QString qtext = QString::fromStdString(text);
    optimizeText(qtext, max_line_lenght);
    QSize meterSize = meter.boundingRect(qtext).size();
    return PGE_Size(meterSize.width(), meterSize.height());
    #else
    //assert(false && "TTF FONTS SUPPORT IS DISABLED!");
    (void)ttfFontPixelSize;
    return PGE_Size(27 * 20, static_cast<int>(std::count(text.begin(), text.end(), '\n') + 1) * 20);
    #endif
}

void FontManager::optimizeText(std::string &text, size_t max_line_lenght, int *numLines, int *numCols)
{
    /****************Word wrap*********************/
    size_t  lastspace = 0;
    int     count = 1;
    size_t  maxWidth = 0;

    for(size_t x = 0, i = 0; i < text.size(); i++, x++)
    {
        switch(text[i])
        {
        case '\t':
        case ' ':
            lastspace = i;
            break;

        case '\n':
            lastspace = 0;
            if((maxWidth < x) && (maxWidth < max_line_lenght))
                maxWidth = x;
            x = 0;
            count++;
            break;
        }

        if(x >= max_line_lenght) //If lenght more than allowed
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

        UTF8 uch = static_cast<unsigned char>(text[i]);
        i += static_cast<size_t>(trailingBytesForUTF8[uch]);
    }

    if(count == 1)
        maxWidth = text.length();

    /****************Word wrap*end*****************/

    if(numLines)
        *numLines = count;

    if(numCols)
        *numCols = static_cast<int>(maxWidth);
}

std::string FontManager::cropText(std::string text, size_t max_symbols)
{
    if(max_symbols == 0)
        return text;

    size_t utf8len = utf8_substrlen(text, max_symbols);
    if(text.size() > utf8len)
    {
        text.erase(utf8len, text.size() - utf8len);
        text.append("...");
    }
    return text;
}



void FontManager::printText(std::string text, int x, int y, int font, float Red, float Green, float Blue, float Alpha, int ttf_FontSize)
{
    if(!isInit) return;

    if(text.empty())
        return;

    if((font >= 0) && (static_cast<size_t>(font) < rasterFonts.size()))
    {
        if(rasterFonts[font].isLoaded())
        {
            rasterFonts[font].printText(text, x, y, Red, Green, Blue, Alpha);
            return;
        }
    }
    #ifdef PGE_TTF
    else
    if(font == DefaultTTF_Font)
    {
        int offsetX = 0;
        int offsetY = 0;
        int height = 32;
        int width = 32;
        GlRenderer::setTextureColor(Red, Green, Blue, Alpha);

        for(char &cx : text)
        {
            switch(cx)
            {
            case '\n':
                offsetX = 0;
                offsetY += height;
                continue;

            case '\t':
                offsetX += offsetX + offsetX % width;
                continue;
            }

            PGE_Texture c;
            getChar2(cx, ttf_FontSize, c);
            GlRenderer::renderTexture(&c, x + offsetX, y + offsetY);
            width = c.w;
            height = c.h;
            offsetX += c.w;
        }

        return;
    }
    else
        printTextTTF(text, x, y, ttf_FontSize,
                     qRgba(static_cast<int>(Red * 255.0f),
                           static_cast<int>(Green * 255.0f),
                           static_cast<int>(Blue * 255.0f),
                           static_cast<int>(Alpha * 255.0f)));
    #else
    else
    {
        (void)ttf_FontSize;
        //assert(false && "TTF FONTS SUPPORT IS DISABLED!");
    }
    #endif
}

int FontManager::getFontID(std::string fontName)
{
    FontsHash::iterator i = fonts.find(fontName);
    if(i == fonts.end())
        return DefaultRaster;
    else
        return i->second;
}

