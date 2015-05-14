/*
 * SMBX64 Playble Character Sprite Calibrator, a free tool for playable srite design
 * This is a part of the Platformer Game Engine by Wohlstand, a free platform for game making
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

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "../../_Libs/EasyBMP/EasyBMP.h"
extern "C"
{
    #include "../../_Libs/giflib/gif_lib.h"
}

#include <QFile>
#include <QImage>

class Graphics
{
public:
    static QImage setAlphaMask(QImage image, QImage mask);
    static bool toGif(QImage& img, QString& path);
    static QImage fromBMP(QString &file);
    static QImage loadQImage(QString file);
};

#endif // GRAPHICS_H
