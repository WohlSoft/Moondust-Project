/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

class RasterFont
{
public:
    RasterFont();
    ~RasterFont();
    void  loadFontMap(QString fontmap_ini);
    QSize textSize(QString &text, int max_line_lenght=0, bool cut=false);
private:
    int letter_width;
    int letter_height;
    struct RasChar
    {
        GLuint tx;
        float l;//!< left
        float t;//!< top
        float b;//!< bottom
        float r;//!< right
    };
    QHash<QChar, RasChar > fontMap;
    QRegExp first_line_only;
    //Font textures cache
    QList<GLuint > textures;
};



class FontManager
{
public:
    static void init();
    static void quit();
    //static TTF_Font *buildFont(QString _fontPath, GLint size);
    //static TTF_Font *buildFont_RW(QString _fontPath, GLint size);
    static void SDL_string_texture_create(QFont &font, QRgb color, QString &text, GLuint *texture, bool borders=false);
    static void SDL_string_texture_create(QFont &font, QRect limitRect, int fontFlags, QRgb color,
                                          QString &text, GLuint *texture, bool borders=false);

    static void SDL_string_render2D(GLuint x, GLuint y, GLuint *texture);
    static GLuint TextToTexture(QString text, QRect rectangle, int alignFlags, bool borders=false);

    static GLuint getChar1(QChar _x);
    static GLuint getChar2(QChar _x);

    static QFont font();

    static void printText(QString text, int x, int y);
    static void printText(QString text, int x, int y, int pointSize, QRgb color=qRgba(255,255,255,255));


private:
    static bool isInit;
    //static TTF_Font * defaultFont;
    static GLuint textTexture;
    static QMap<QChar, GLuint> fontTable_1;
    static QMap<QChar, GLuint> fontTable_2;
    static int fontID;
    static bool double_pixled;

    static QFont *defaultFont;
};

#endif // FONT_MANAGER_H
