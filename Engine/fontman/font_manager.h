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

#include "font_engine_base.h"

namespace FontManager
{
/*
 * UTF-8 helpers
 */
size_t       utf8_strlen(const char *str);
size_t       utf8_strlen(const char *str, size_t len);
size_t       utf8_strlen(const std::string &str);
/**
 * @brief Get byte lenght from character lenght from begin to utf8len
 * @param str input UTF-8 string
 * @param utf8len Desireed character lenght
 * @return Byte-lenght of desired character lenght
 */
size_t       utf8_substrlen(const std::string &str, size_t utf8len);
std::string  utf8_substr(const std::string &str, size_t utf8_begin, size_t utf8_len);
void         utf8_pop_back(std::string &str);
void         utf8_erase_at(std::string &str, size_t begin);
void         utf8_erase_before(std::string &str, size_t end);


//! Initialize basic support for font rendering withouth config pack
void initBasic();
//! Full initialization of font renderith include external fonts
void initFull();
//! De-Initialize font manager and clear memory
void quit();

enum DefaultFont
{
    TTF_Font        = -2,
    DefaultTTF_Font = -1,
    DefaultRaster   = 0
};

/**
 * @brief Get pixel metrics of the text block
 * @param text Text fragment to measure
 * @param fontID Font-ID
 * @param max_line_lenght max line width
 * @param cut First line only, ignore all next lines
 * @param ttfFontPixelSize Point size for the TTF fonts
 * @return
 */
PGE_Size textSize(std::string &text, int    fontID,
                 uint32_t max_line_lenght = 0,
                 bool   cut = false,
                 uint32_t ttfFontSize = 14);

/**
 * @brief Retreive Font-ID from the font name
 * @param fontName Name of the font
 * @return FontID key
 */
int      getFontID(std::string fontName);

/**
 * @brief Print the text fragment to the screen
 * @param text Text fragment to print
 * @param x X position on the screen
 * @param y Y position on the screen
 * @param font Font-ID
 * @param Red Red color level from 0.0 to 1.0
 * @param Green Green color level from 0.0 to 1.0
 * @param Blue Blue color level from 0.0 to 1.0
 * @param Alpha Alpha-channel level from 0.0 to 1.0
 * @param ttf_FontSize Point size for the TTF fonts
 */
void printText(std::string text,
                      int x, int y, int font = DefaultRaster,
                      float Red=1.0, float Green=1.0, float Blue=1.0, float Alpha=1.0,
                      uint32_t ttf_FontSize = 14);

/**
 * @brief Clean-Up text fragment to fit into the character width
 * @param [_inout] text Text to optimize
 * @param [_in]  max_line_lenght max line lenght in characters
 * @param [_out] numLines Total line in the text fragment after optimization
 * @param [_out] numCols Total columns in the text fragment after optimization
 */
void           optimizeText(std::string &text,
                            size_t max_line_lenght,
                            int *numLines = 0,
                            int *numCols = 0);

/**
 * @brief Strip text outing of max count of symbols
 * @param text input text
 * @param max_symbols Maximal character count limit
 * @return Begin of string which is fit into desired lenght limit
 */
std::string     cropText(std::string text, size_t max_symbols);

} //namespace FontManager

#endif // FONT_MANAGER_H
