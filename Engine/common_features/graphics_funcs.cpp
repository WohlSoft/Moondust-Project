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

#include <QPixmap>
#include <QImage>
#include <QFileInfo>
#include <QtOpenGL/QGLWidget>
#include "graphics_funcs.h"
#include "../../_Libs/EasyBMP/EasyBMP.h"

QImage GraphicsHelps::setAlphaMask(QImage image, QImage mask)
{
    if(mask.isNull())
        return image;

    if(image.isNull())
        return image;

    QImage target = image;
    QImage newmask = mask;

    if(target.size()!= newmask.size())
    {
        newmask = newmask.copy(0,0, target.width(), target.height());
    }

    newmask.invertPixels();

    target.setAlphaChannel(newmask);

    return target;
}

QImage GraphicsHelps::fromBMP(QString &file)
{
    QImage errImg;

    BMP tarBMP;
    if(!tarBMP.ReadFromFile( file.toLocal8Bit().data() )){
        return errImg; //Check if empty with errImg.isNull();
    }

    QImage bmpImg(tarBMP.TellWidth(),tarBMP.TellHeight(),QImage::Format_RGB666);

    for(int x = 0; x < tarBMP.TellWidth(); x++){
        for(int y = 0; y < tarBMP.TellHeight(); y++){
            RGBApixel pixAt = tarBMP.GetPixel(x,y);
            bmpImg.setPixel(x,y,qRgb(pixAt.Red, pixAt.Green, pixAt.Blue));
        }
    }

    return bmpImg;
}

//QPixmap GraphicsHelps::loadPixmap(QString file)
//{
//    return QPixmap::fromImage(loadQImage(file));
//}

QImage GraphicsHelps::loadQImage(QString file)
{
    QImage image = QImage( file );
    if(image.isNull())
        image = fromBMP( file);
    return image;
}

PGE_Texture GraphicsHelps::loadTexture(QString path, QString maskPath)
{
    PGE_Texture target;

    QImage sourceImage;
    // Load the OpenGL texture
    sourceImage = loadQImage(path); // Gives us the information to make the texture

    //Apply Alpha mask
    if(!maskPath.isEmpty() && QFileInfo(maskPath).exists())
    {
        QImage maskImage = loadQImage(maskPath);
        sourceImage = setAlphaMask(sourceImage, maskImage);
    }

    sourceImage = QGLWidget::convertToGLFormat(sourceImage).mirrored(false, true);

    target.nOfColors = 4;
    target.format = GL_RGBA;

    glEnable(GL_TEXTURE_2D);
    // Have OpenGL generate a texture object handle for us
    glGenTextures( 1, &target.texture );

    // Bind the texture object
    glBindTexture( GL_TEXTURE_2D, target.texture );

    // Edit the texture object's image data using the information SDL_Surface gives us
    target.w = sourceImage.width();
    target.h = sourceImage.height();
    // Set the texture's stretching properties

    // Bind the texture object
    glBindTexture( GL_TEXTURE_2D, target.texture );

    glTexImage2D(GL_TEXTURE_2D, 0, target.nOfColors, sourceImage.width(), sourceImage.height(),
         0, target.format, GL_UNSIGNED_BYTE, sourceImage.bits() );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glDisable(GL_TEXTURE_2D);


    return target;
}



SDL_Surface* GraphicsHelps::QImage_toSDLSurface(const QImage &sourceImage)
{
    // Ensure that the source image is in the correct pixel format
    QImage image = sourceImage;
    if (image.format() != QImage::Format_ARGB32)
        image = image.convertToFormat(QImage::Format_ARGB32);

    // QImage stores each pixel in ARGB format
    // Mask appropriately for the endianness
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    Uint32 amask = 0x000000ff;
    Uint32 rmask = 0x0000ff00;
    Uint32 gmask = 0x00ff0000;
    Uint32 bmask = 0xff000000;
#else
    Uint32 amask = 0xff000000;
    Uint32 rmask = 0x00ff0000;
    Uint32 gmask = 0x0000ff00;
    Uint32 bmask = 0x000000ff;
#endif

    return SDL_CreateRGBSurfaceFrom((void*)image.constBits(),
        image.width(), image.height(), image.depth(), image.bytesPerLine(),
        rmask, gmask, bmask, amask);
}
