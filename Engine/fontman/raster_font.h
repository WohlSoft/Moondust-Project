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

#ifndef RASTER_FONT_H
#define RASTER_FONT_H

#include <string>
#include <unordered_map>
#include <Utils/vptrlist.h>
#include <common_features/pge_texture.h>
#include <common_features/rect.h>
#include <common_features/size.h>

class RasterFont
{
public:
    RasterFont();
    RasterFont(const RasterFont &rf);
    ~RasterFont();
    void  loadFont(std::string font_ini);
    void  loadFontMap(std::string fontmap_ini);
    PGE_Size textSize(std::string &text, int max_line_lenght=0, bool cut=false);
    void printText(const std::string &text, int x, int y, float Red=1.f, float Green=1.f, float Blue=1.f, float Alpha=1.f);
    bool isLoaded();
    std::string getFontName();

private:
    bool isReady;       //!<font is fine
    bool ttf_borders;   //!<Enable borders on backup ttf font render [all unknown characters will be rendered as TTF]
    int letter_width;   //!<Width of one letter
    int letter_height;  //!<Height of one letter
    int interletter_space;//!< Space between printing letters
    int space_width;    //!<Width of space symbol
    int newline_offset; //!< Distance between top of one line and top of next

    int matrix_width;   //!< Width of font matrix
    int matrix_height;  //!< Width of font matrix
    std::string fontName;   //!< Handalable name of the font

    struct RasChar
    {
        bool valid = false;
        PGE_Texture* tx     = nullptr;
        int padding_left    = 0;  //!< Crop left
        int padding_right   = 0; //!< Crop right
        float l = 1.0f;//!< left
        float t = 1.0f;//!< top
        float b = 1.0f;//!< bottom
        float r = 1.0f;//!< right
    };

    typedef std::unordered_map<char32_t, RasChar > CharMap;

    //! Table of available characters
    CharMap fontMap;

    //! Bank of loaded textures
    VPtrList<PGE_Texture > textures;
};

#endif // RASTER_FONT_H
