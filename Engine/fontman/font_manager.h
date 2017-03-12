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

//#define PGE_TTF

//#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_opengl.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <Utils/vptrlist.h>

#ifdef PGE_TTF
#include <QString>
#endif

#include <common_features/pge_texture.h>
#include <common_features/rect.h>
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
        PGE_Texture* tx = NULL;
        int padding_left = 0;  //!< Crop left
        int padding_right = 0; //!< Crop right
        float l = 1.0f;//!< left
        float t = 1.0f;//!< top
        float b = 1.0f;//!< bottom
        float r = 1.0f;//!< right
    };

    std::unordered_map<char32_t, RasChar > fontMap; //!< Table of available characters

    //Font textures cache
    VPtrList<PGE_Texture > textures;   //!< Bank of loaded textures
};



class FontManager
{
public:
    static void initBasic();
    static void initFull();
    static void quit();
    //static TTF_Font *buildFont(QString _fontPath, GLint size);
    //static TTF_Font *buildFont_RW(QString _fontPath, GLint size);
    static size_t utf8_strlen(const char *str);
    static size_t utf8_strlen(const char *str, size_t len);
    static size_t utf8_strlen(const std::string &str);
    static size_t utf8_substrlen(const std::string &str, size_t utf8len);
    static std::string utf8_substr(const std::string &str, size_t utf8_begin, size_t utf8_len);
    static void   utf8_pop_back(std::string &str);
    static void   utf8_erase_at(std::string &str, size_t begin);
    static void   utf8_erase_before(std::string &str, size_t end);

    static PGE_Size textSize(std::string &text, int fontID, int max_line_lenght=0, bool cut=false, int ttfFontPixelSize = -1);
    #ifdef PGE_TTF
    static int getFontID(QString fontName);
    #endif
    static int getFontID(std::string fontName);

    #ifdef PGE_TTF
    static void getChar1(QChar _x, int px_size, PGE_Texture &tex);
    static void getChar2(QChar _x, int px_size, PGE_Texture &tex);

    static QFont font();
    #endif

    enum DefaultFont
    {
        TTF_Font        = -2,
        DefaultTTF_Font = -1,
        DefaultRaster   = 0
    };
    static void printText(std::string text, int x, int y, int font=DefaultRaster,
                          float Red=1.0, float Green=1.0, float Blue=1.0, float Alpha=1.0, int ttf_FontSize=14);
    #ifdef PGE_TTF
    static void printTextTTF(std::string text, int x, int y, int pointSize, QRgb color=qRgba(255,255,255,255));
    #endif


    static VPtrList<RasterFont> rasterFonts;//!< Complete array of raster fonts
    static RasterFont *rFont;//!< Default raster font

    static void optimizeText(std::string &text, size_t max_line_lenght, int *numLines = 0, int *numCols = 0);
    static std::string cropText(std::string text, size_t max_symbols);

    /****Deprecated functions*******/
    #ifdef PGE_TTF
    static void SDL_string_texture_create(QFont &font, QRgb color, QString &text, PGE_Texture *texture, bool borders=false);
    static void SDL_string_texture_create(QFont &font, PGE_Rect limitRect, int fontFlags, QRgb color,
                                          QString &text, PGE_Texture *texture, bool borders=false);

    struct TTFCharType
    {
        QChar c;
        int px_size = 0;
        bool operator==(const TTFCharType cht) const
        {
            return ((c == cht.c) && (px_size == cht.px_size));
        }
    };
    #endif

private:
    static bool isInit;
    //static TTF_Font * defaultFont;
    //static PGE_Texture textTexture;
    #ifdef PGE_TTF
    friend uint std::unordered_map(const TTFCharType &struc);
    static std::unordered_map<TTFCharType, PGE_Texture> fontTable_1;
    static std::unordered_map<TTFCharType, PGE_Texture> fontTable_2;
    #endif
    typedef std::unordered_map<std::string, int> FontsHash;
    static FontsHash fonts;
    static int  fontID;
    static bool double_pixled;

    #ifdef PGE_TTF
    static QFont *defaultFont;
    #endif
};

#ifdef PGE_TTF
inline uint std::unordered_map(const FontManager::TTFCharType &struc)
{
    uint result = struc.c.unicode();

    result <<=(sizeof(quint8)*8);
    result = result | struc.px_size;

    return result;
}
#endif

#endif // FONT_MANAGER_H
