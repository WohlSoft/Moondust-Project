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

#include <QPixmap>
#include <QPainter>
#include <QImage>

#undef main
#include <SDL2/SDL.h> // SDL 2 Library
#include <SDL2/SDL_opengl.h>
#undef main

#include "pge_texture.h"

#ifndef GRAPHICS_FUNCS_H
#define GRAPHICS_FUNCS_H


class GraphicsHelps
{
public:
    static QImage setAlphaMask(QImage image, QImage mask);
    static QImage fromBMP(QString& file);
    static QImage loadQImage(QString file);
    static PGE_Texture loadTexture(PGE_Texture &target, QString path, QString maskPath="");
    static QPixmap squareImage(QPixmap image, QSize targetSize);
    static SDL_Surface *QImage_toSDLSurface(const QImage &sourceImage);

    static void flipHorizontally(SDL_Surface *&image);
    static void flipVertically(SDL_Surface *&image);
    static Uint32 getpixel(SDL_Surface *surface, int x, int y);
    static void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
    static void loadMaskedImage(QString rootDir, QString in_imgName, QString &out_maskName, QString &out_errStr);
};

#endif // GRAPHICS_FUNCS_H
