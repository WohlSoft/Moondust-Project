/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2015 Vitaly Novichkov <admin@wohlnet.ru>
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

//#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_opengl.h>
#include <QString>
#include <QFont>
#include <QSize>
#include <QMap>
#include <QRgb>
#include <QRegExp>


#include <common_features/pge_texture.h>
class RasterFont
{
public:
    RasterFont();
    RasterFont(const RasterFont &rf);
    ~RasterFont();
    void  loadFont(QString font_ini);
    void  loadFontMap(QString fontmap_ini);
    QSize textSize(QString &text, int max_line_lenght=0, bool cut=false);
    void printText(QString text, int x, int y, float Red=1.f, float Green=1.f, float Blue=1.f, float Alpha=1.f);
    bool isLoaded();
    QString getFontName();
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
    QString fontName;   //!< Handalable name of the font

    struct RasChar
    {
        RasChar() {valid=false;}
        bool valid;
        GLuint tx;
        int padding_left;  //!< Crop left
        int padding_right; //!< Crop right
        float l;//!< left
        float t;//!< top
        float b;//!< bottom
        float r;//!< right
    };
    QHash<QChar, RasChar > fontMap; //!< Table of available characters
    QRegExp first_line_only;        //!< This regular expression will remove evervything starts from first linefeed

    //Font textures cache
    QList<PGE_Texture > textures;   //!< Bank of loaded textures
};



class FontManager
{
public:
    static void init();
    static void quit();
    //static TTF_Font *buildFont(QString _fontPath, GLint size);
    //static TTF_Font *buildFont_RW(QString _fontPath, GLint size);

    static QSize textSize(QString &text, int fontID, int max_line_lenght=0, bool cut=false);
    static int getFontID(QString fontName);

    static GLuint getChar1(QChar _x);
    static GLuint getChar2(QChar _x);

    static QFont font();
    enum DefaultFont
    {
        DefaultTTF_Font=-1,
        DefaultRaster=0
    };
    static void printText(QString text, int x, int y, int font=DefaultRaster,
                          float Red=1.0, float Green=1.0, float Blue=1.0, float Alpha=1.0, int ttf_FontSize=14);
    static void printTextTTF(QString text, int x, int y, int pointSize, QRgb color=qRgba(255,255,255,255));


    static QList<RasterFont> rasterFonts;//!< Complete array of raster fonts
    static RasterFont *rFont;//!< Default raster font

    static void optimizeText(QString &text, int max_line_lenght, int *numLines=0, int *numCols=0);
    static QString cropText(QString text, int max_symbols);

    /****Deprecated functions*******/
    static void SDL_string_texture_create(QFont &font, QRgb color, QString &text, GLuint *texture, bool borders=false);
    static void SDL_string_texture_create(QFont &font, QRect limitRect, int fontFlags, QRgb color,
                                          QString &text, GLuint *texture, bool borders=false);

    static void SDL_string_render2D(GLuint x, GLuint y, GLuint *texture);
    static GLuint TextToTexture(QString text, QRect rectangle, int alignFlags, bool borders=false);

private:
    static bool isInit;
    //static TTF_Font * defaultFont;
    static GLuint textTexture;
    static QHash<QChar, GLuint> fontTable_1;
    static QHash<QChar, GLuint> fontTable_2;
    static QHash<QString, int> fonts;
    static int fontID;
    static bool double_pixled;

    static QFont *defaultFont;
};

#endif // FONT_MANAGER_H
