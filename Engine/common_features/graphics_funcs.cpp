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
#include <QRgb>
#include <QFileInfo>
#include <QMessageBox>
#include <QtOpenGL/QGLWidget>

#include "graphics_funcs.h"
#include <EasyBMP/EasyBMP.h>
extern "C"{
#include <giflib/gif_lib.h>
}

#include <QtDebug>

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
    QFile img(file);
    if(!img.exists())
        return QImage();
    if(!img.open(QIODevice::ReadOnly))
        return QImage();

    QByteArray mg=img.read(5); //Get magic number!
    if(mg.size()<2) // too short!
        return QImage();
    char *magic = mg.data();
    img.close();

    //Detect PNG 89 50 4e 47
    if( (magic[0]=='\x89')&&(magic[1]=='\x50')&&(magic[2]=='\x4e')&&(magic[3]=='\x47') )
    {
        QImage image = QImage( file );
        return image;
    } else
    //Detect GIF 47 49 46 38
    if( (magic[0]=='\x47')&&(magic[1]=='\x49')&&(magic[2]=='\x46')&&(magic[3]=='\x38') )
    {
        QImage image = QImage( file );
        //QImage image = fromGIF( file );
        return image;
    }
    else
    //Detect BMP 42 4d
    if( (magic[0]=='\x42')&&(magic[1]=='\x4d') )
    {
        QImage image = fromBMP( file );
        return image;
    }

    //Another formats, supported by Qt :P
    QImage image = QImage( file );
    return image;
}

PGE_Texture GraphicsHelps::loadTexture(PGE_Texture &target, QString path, QString maskPath)
{
    QImage sourceImage;
    // Load the OpenGL texture
    sourceImage = loadQImage(path); // Gives us the information to make the texture

    if(sourceImage.isNull())
    {
        SDL_Quit();
        //if(ErrorCheck::hardMode)
        //{
            QMessageBox::critical(NULL, "Texture error",
                QString("Error loading of image file: \n%1\nReason: %2.")
                .arg(path).arg(QFileInfo(path).exists()?"wrong image format":"file not exist"), QMessageBox::Ok);
            exit(1);
        //}
        return target;
    }

    //Apply Alpha mask
    if(!maskPath.isEmpty() && QFileInfo(maskPath).exists())
    {
        QImage maskImage = loadQImage(maskPath);
        sourceImage = setAlphaMask(sourceImage, maskImage);
    }

    sourceImage=sourceImage.convertToFormat(QImage::Format_ARGB32);
    QRgb upperColor = sourceImage.pixel(0,0);
    target.ColorUpper.r = float(qRed(upperColor))/255.0f;
    target.ColorUpper.g = float(qGreen(upperColor))/255.0f;
    target.ColorUpper.b = float(qBlue(upperColor))/255.0f;

    QRgb lowerColor = sourceImage.pixel(0, sourceImage.height()-1);
    target.ColorLower.r = float(qRed(lowerColor))/255.0f;
    target.ColorLower.g = float(qGreen(lowerColor))/255.0f;
    target.ColorLower.b = float(qBlue(lowerColor))/255.0f;

    //qDebug() << path << sourceImage.size();

    sourceImage = QGLWidget::convertToGLFormat(sourceImage).mirrored(false, true);

    target.nOfColors = 4;
    target.format = GL_RGBA;

    glEnable(GL_TEXTURE_2D);
    // Have OpenGL generate a texture object handle for us
    glGenTextures( 1, &(target.texture) );

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


QPixmap GraphicsHelps::squareImage(QPixmap image, QSize targetSize=QSize(0,0) )
{
    QPixmap target = QPixmap(targetSize);
    target.fill(Qt::transparent);
    QPixmap source;

    if( ( targetSize.width() < image.width() ) || ( targetSize.height() < image.height() ))
        source = image.scaled(targetSize, Qt::KeepAspectRatio);
    else
        source = image;

    QPainter p(&target);

    int targetX = qRound( ( ( qreal(target.width()) - qreal(source.width()) ) / 2 ) );
    int targetY = qRound( ( ( qreal(target.height()) - qreal(source.height()) ) / 2 ) );

    p.drawPixmap( targetX, targetY,source.width(),source.height(), source );

    p.end();
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

void GraphicsHelps::flipHorizontally( SDL_Surface*& image )
{
    // create a copy of the image
    SDL_Surface* flipped_image = SDL_CreateRGBSurface( SDL_SWSURFACE, image->w, image->h, image->format->BitsPerPixel,
        image->format->Rmask, image->format->Gmask, image->format->Bmask, image->format->Amask );

    // loop through pixels
    for( int y=0; y<image->h; y++ )
    {
        for( int x=0; x<image->w; x++ )
        {
            // copy pixels, but reverse the x pixels!
            putpixel( flipped_image, x, y, getpixel(image, image->w - x - 1, y) );
        }
    }

    // free original and assign flipped to it
    SDL_FreeSurface( image );
    image = flipped_image;
}

void GraphicsHelps::flipVertically( SDL_Surface*& image )
{
    // create a copy of the image
    SDL_Surface* flipped_image = SDL_CreateRGBSurface( SDL_SWSURFACE, image->w, image->h, image->format->BitsPerPixel,
        image->format->Rmask, image->format->Gmask, image->format->Bmask, image->format->Amask );

    // loop through pixels
    for( int y=0; y<image->h; y++ )
    {
        for( int x=0; x<image->w; x++ )
        {
            // copy pixels, but reverse the x pixels!
            putpixel( flipped_image, x, y, getpixel(image, x, image->h - y-1) );
        }
    }

    // free original and assign flipped to it
    SDL_FreeSurface( image );
    image = flipped_image;
}

Uint32 GraphicsHelps::getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16 *)p;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;

    case 4:
        return *(Uint32 *)p;
        break;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

void GraphicsHelps::putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}


void GraphicsHelps::loadMaskedImage(QString rootDir, QString in_imgName, QString &out_maskName, QString &out_errStr)
{
    if( in_imgName.isEmpty() )
    {
        out_errStr = "Image filename isn't defined";
        return;
    }

    if(!QFile(rootDir+in_imgName).exists())
    {
        out_errStr="image file is not exist: "+rootDir+in_imgName;
        return;
    }

    out_maskName=in_imgName;
    int i = out_maskName.size()-1;
    for( ;i>0; i--)
    {
        if(out_maskName[i]=='.')
        {
            out_maskName.insert(i, 'm');
            break;
        }
    }

    if(i==0)
    {
        out_maskName = "";
    }
    out_errStr = "";
}
