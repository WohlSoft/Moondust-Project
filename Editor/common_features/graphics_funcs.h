/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef GRAPHICS_FUNCS_H
#define GRAPHICS_FUNCS_H

class GraphicsHelps
{
public:
    static QPixmap setAlphaMask(QPixmap image, QPixmap mask);
    static QImage setAlphaMask_VB(QImage image, QImage mask);
    static bool EnableVBEmulate;
    static void loadMaskedImage(QString rootDir, QString in_imgName, QString &out_maskName, QPixmap &out_Img, QPixmap &out_Mask, QString &out_errStr);

    static QImage fromBMP(QString& file);
    static QPixmap loadPixmap(QString file);
    static QImage loadQImage(QString file);
    static QPixmap squareImage(QPixmap image, QSize targetSize);
    static QPixmap drawDegitFont(int number);

    static bool toGif(QImage& img, QString& path);
};

#endif // GRAPHICS_FUNCS_H
