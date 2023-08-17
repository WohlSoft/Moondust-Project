/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2023 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <memory>
#include <FreeImageLite.h>
#include <QtDebug>

#include <common_features/logger.h>

#include "Graphics/bitmask2rgba.h"
#include "graphics_funcs.h"



FIBITMAP *GraphicsHelps::loadImage(QString file, bool convertTo32bit)
{
    QFile fileMap(file);

    FIBITMAP *img = nullptr;

    if(!fileMap.open(QIODevice::ReadOnly))
        return nullptr;

    unsigned int m_size = static_cast<unsigned int>(fileMap.size());
    unsigned char *m = fileMap.map(0, fileMap.size());

    if(m)
    {
        FIMEMORY *imgMEM = FreeImage_OpenMemory(m, m_size);
        FREE_IMAGE_FORMAT formato = FreeImage_GetFileTypeFromMemory(imgMEM);

        if(formato  == FIF_UNKNOWN)
        {
            fileMap.unmap(m);
            return nullptr;
        }

        img = FreeImage_LoadFromMemory(formato, imgMEM, 0);
        FreeImage_CloseMemory(imgMEM);

        fileMap.unmap(m);
    }
    else
    {
        FREE_IMAGE_FORMAT formato = FreeImage_GetFileType(file.toUtf8().data(), 0);
        if(formato  == FIF_UNKNOWN)
            return nullptr;
        img = FreeImage_Load(formato, file.toUtf8().data());
    }

    if(!img)
        return nullptr;

    unsigned int bpp = FreeImage_GetBPP(img);

    if(convertTo32bit && bpp != 32)
    {
        FIBITMAP *temp = FreeImage_ConvertTo32Bits(img);
        FreeImage_Unload(img);
        if(!temp)
            return nullptr;
        img = temp;
    }

    return img;
}

void GraphicsHelps::mergeWithMask(FIBITMAP *image, QString pathToMask, QPixmap *maskFallback)
{
    if(!image) return;

    if(!QFile::exists(pathToMask) && !maskFallback)
        return; //Nothing to do.

    FIBITMAP *mask = loadImage(pathToMask, true);
    if(!mask && maskFallback)
        getMaskFromRGBA(*maskFallback, mask);

    if(!mask)
        return;//Nothing to do.

    bitmask_to_rgba(image, mask);

    FreeImage_Unload(mask);
}

void GraphicsHelps::getMaskFromRGBA(const QPixmap &srcimage, QImage &mask)
{
    unsigned int img_w   = static_cast<unsigned int>(srcimage.width());
    unsigned int img_h   = static_cast<unsigned int>(srcimage.height());

    QImage image = srcimage.toImage();
    mask = QImage(int(img_w), int(img_h), QImage::Format_ARGB32);
    QRgb Fpix;
    for(unsigned int y = 0; (y < img_h); y++)
    {
        for(unsigned int x = 0; (x < img_w); x++)
        {
            Fpix = image.pixel(int(x), int(y));
            int gray = (255 - qAlpha(Fpix));
            mask.setPixel(int(x), int(y), qRgba(gray, gray, gray, 0xFF));
        }
    }
}

void GraphicsHelps::getMaskFromRGBA(const QPixmap &srcimage, FIBITMAP *&mask)
{
    unsigned int img_w   = static_cast<unsigned int>(srcimage.width());
    unsigned int img_h   = static_cast<unsigned int>(srcimage.height());

    QImage image = srcimage.toImage();

    mask = FreeImage_AllocateT(FIT_BITMAP,
                               int(img_w), int(img_h),
                               32,
                               FI_RGBA_RED_MASK,
                               FI_RGBA_GREEN_MASK,
                               FI_RGBA_BLUE_MASK);

    RGBQUAD Npix = {0x0, 0x0, 0x0, 0xFF};

    for(unsigned int y = 0; (y < img_h); y++)
    {
        for(unsigned int x = 0; (x < img_w); x++)
        {
            uint8_t gray = uint8_t(255 - qAlpha(image.pixel(int(x), (int(img_h) - 1) - int(y)) ));
            Npix.rgbRed  = gray;
            Npix.rgbGreen = gray;
            Npix.rgbBlue = gray;
            Npix.rgbReserved = 0xFF;
            FreeImage_SetPixelColor(mask,  x, y, &Npix);
        }
    }
}

void GraphicsHelps::loadMaskedImage(QString rootDir, QString in_imgName, QString &out_maskName, QPixmap &out_Img, QImage &, QString &out_errStr)
{
    loadMaskedImage(rootDir, in_imgName, out_maskName, out_Img, out_errStr);
}

void GraphicsHelps::loadMaskedImage(QString rootDir, QString in_imgName, QString &out_maskName, QPixmap &out_Img, QString &out_errStr)
{
    if(in_imgName.isEmpty())
    {
        out_errStr = "Image filename isn't defined";
        return;
    }

    if(!QFile::exists(rootDir + in_imgName))
    {
        out_errStr = "image file is not exist: " + rootDir + in_imgName;
        return;
    }

    out_maskName = in_imgName;
    int i = out_maskName.size() - 1;

    for(; i > 0; i--)
    {
        if(out_maskName[i] == '.')
        {
            out_maskName.insert(i, 'm');
            break;
        }
    }

    QImage target;

    if(i == 0)
    {
        out_maskName = "";
        loadQImage(target, rootDir + in_imgName);
    }
    else
        loadQImage(target, rootDir + in_imgName, rootDir + out_maskName);

    if(target.isNull())
    {
        out_errStr = "Broken image file " + rootDir + in_imgName;
        return;
    }

    //GraphicsHelps::mergeToRGBA(out_Img, out_Mask, rootDir+in_imgName, rootDir + out_maskName);
    out_Img = QPixmap::fromImage(target);

    if(out_Img.isNull())
    {
        out_errStr = "Broken image file " + rootDir + in_imgName;
        return;
    }

    out_errStr = "";
}

void GraphicsHelps::loadIconOpt(QString rootDir, QString in_imgName, QPixmap &out_Img)
{
    out_Img = QPixmap(); // null it

    if(in_imgName.isEmpty())
        return;

    if(!QFile::exists(rootDir + in_imgName))
        return;

    out_Img.load(rootDir + in_imgName);
}

QPixmap GraphicsHelps::loadPixmap(QString file)
{
    return QPixmap::fromImage(loadQImage(file));
}

QImage GraphicsHelps::loadQImage(QString file)
{
    if(file.startsWith(':'))
    {
        //Load from resources!
        QImage image = QImage(file);
        return image;
    }
    else    //Load via FreeImage
    {
        FIBITMAP *img = loadImage(file, true);

        if(img)
        {
            BYTE *bits = FreeImage_GetBits(img);
            int width  = int(FreeImage_GetWidth(img));
            int height = int(FreeImage_GetHeight(img));
            QImage target(width, height, QImage::Format_ARGB32);

            for(int y = height - 1; y >= 0; y--)
            {
                for(int x = 0; x < width; x++)
                {
                    target.setPixel(x, y, qRgba(bits[FI_RGBA_RED],
                                                bits[FI_RGBA_GREEN],
                                                bits[FI_RGBA_BLUE],
                                                bits[FI_RGBA_ALPHA]));
                    bits += 4;
                }
            }

            FreeImage_Unload(img);
            return target;
        }
        else
            return QImage();
    }
}

void GraphicsHelps::loadQImage(QImage &target, QString file, QString maskPath, QPixmap *fallbackMask)
{
    if(file.startsWith(':'))
    {
        //Load from resources!
        target = QImage(file);
        return;
    }
    else    //Load via FreeImage
    {
        FIBITMAP *img = loadImage(file, true);

        if(img)
        {
            mergeWithMask(img, maskPath, fallbackMask);
            BYTE *bits = FreeImage_GetBits(img);
            int width  = int(FreeImage_GetWidth(img));
            int height = int(FreeImage_GetHeight(img));
            target = QImage(width, height, QImage::Format_ARGB32);

            for(int y = height - 1; y >= 0; y--)
            {
                for(int x = 0; x < width; x++)
                {
                    target.setPixel(x, y, qRgba(bits[FI_RGBA_RED],
                                                bits[FI_RGBA_GREEN],
                                                bits[FI_RGBA_BLUE],
                                                bits[FI_RGBA_ALPHA]));
                    bits += 4;
                }
            }

            FreeImage_Unload(img);
            return;
        }
        else
            return;
    }
}

void GraphicsHelps::squareImageR(QPixmap &imageInOut, QSize targetSize)
{
    if(imageInOut.isNull())
        return;

    if(targetSize.isNull())
    {
        if(imageInOut.width() > imageInOut.height())
        {
            targetSize.setWidth(imageInOut.height());
            targetSize.setHeight(imageInOut.height());
        }
        else
        {
            targetSize.setWidth(imageInOut.width());
            targetSize.setHeight(imageInOut.width());
        }
    }

    QPixmap source;

    if((targetSize.width() < imageInOut.width()) || (targetSize.height() < imageInOut.height()))
        source = imageInOut.scaled(targetSize, Qt::KeepAspectRatio, Qt::FastTransformation);
    else
        source = imageInOut;

    imageInOut = QPixmap(targetSize);
    imageInOut.fill(Qt::transparent);
    QPainter p(&imageInOut);
    int targetX = qRound(((qreal(imageInOut.width()) - qreal(source.width())) / 2));
    int targetY = qRound(((qreal(imageInOut.height()) - qreal(source.height())) / 2));
    p.drawPixmap(targetX, targetY, source.width(), source.height(), source);
    p.end();
}

QPixmap GraphicsHelps::drawDigitFont(int number)
{
    QPixmap font = QPixmap(":/fonts/font_digits.png");
    QString text = QString::number(number);
    QPixmap img(text.size() * 18, 16);
    img.fill(Qt::transparent);
    QPainter p(&img);

    for(int i = 0; i < text.size(); i++)
        p.drawPixmap(QRect(18 * i, 0, 16, 16), font, QRect(0, QString(text[i]).toInt() * 16, 16, 16));

    p.end();
    return img;
}
