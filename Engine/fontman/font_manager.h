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

#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H

#include <common_features/pge_texture.h>
#include <common_features/size.h>
#include <Utils/vptrlist.h>

#include <string>
#include <vector>
#include <unordered_map>


#include "raster_font.h"

class FontManager
{
public:
    static void initBasic();
    static void initFull();
    static void quit();

    static size_t       utf8_strlen(const char *str);
    static size_t       utf8_strlen(const char *str, size_t len);
    static size_t       utf8_strlen(const std::string &str);
    static size_t       utf8_substrlen(const std::string &str, size_t utf8len);
    static std::string  utf8_substr(const std::string &str, size_t utf8_begin, size_t utf8_len);
    static void         utf8_pop_back(std::string &str);
    static void         utf8_erase_at(std::string &str, size_t begin);
    static void         utf8_erase_before(std::string &str, size_t end);

    static PGE_Size textSize(std::string &text,
                             int    fontID,
                             int    max_line_lenght = 0,
                             bool   cut = false,
                             int    ttfFontPixelSize = -1);

    static int getFontID(std::string fontName);

    enum DefaultFont
    {
        TTF_Font        = -2,
        DefaultTTF_Font = -1,
        DefaultRaster   = 0
    };

    static void printText(std::string text,
                          int x, int y, int font = DefaultRaster,
                          float Red=1.0, float Green=1.0, float Blue=1.0, float Alpha=1.0,
                          int ttf_FontSize = 14);

    static VPtrList<RasterFont> rasterFonts;//!< Complete array of raster fonts
    static RasterFont *rFont;//!< Default raster font

    static void optimizeText(std::string &text,
                             size_t max_line_lenght,
                             int *numLines = 0,
                             int *numCols = 0);

    static std::string cropText(std::string text, size_t max_symbols);

private:
    static bool isInit;

    typedef std::unordered_map<std::string, int> FontsHash;
    static FontsHash fonts;

    static int  fontID;
    static bool double_pixled;
};

#endif // FONT_MANAGER_H
