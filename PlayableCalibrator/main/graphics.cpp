/*
 * SMBX64 Playble Character Sprite Calibrator, a free tool for playable srite design
 * This is a part of the Platformer Game Engine by Wohlstand, a free platform for game making
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

#include "graphics.h"

#ifdef _WIN32
#define FREEIMAGE_LIB 1
#endif
#include <FreeImageLite.h>
#include <QFile>

static FIBITMAP *loadImage(QString file, bool convertTo32bit)
{
#if  0//defined(__unix__) || defined(__APPLE__) || defined(_WIN32)
    FileMapper fileMap;

    if(!fileMap.open_file(file.toUtf8().data()))
        return nullptr;

    FIMEMORY *imgMEM = FreeImage_OpenMemory(reinterpret_cast<unsigned char *>(fileMap.data()),
                                            static_cast<unsigned int>(fileMap.size()));
    FREE_IMAGE_FORMAT formato = FreeImage_GetFileTypeFromMemory(imgMEM);

    if(formato  == FIF_UNKNOWN)
        return nullptr;

    FIBITMAP *img = FreeImage_LoadFromMemory(formato, imgMEM, 0);
    FreeImage_CloseMemory(imgMEM);
    fileMap.close_file();

    if(!img)
        return nullptr;

#else
    FREE_IMAGE_FORMAT formato = FreeImage_GetFileType(file.toUtf8().data(), 0);

    if(formato  == FIF_UNKNOWN)
        return nullptr;

    FIBITMAP *img = FreeImage_Load(formato, file.toUtf8().data());

    if(!img)
        return nullptr;

#endif

    if(convertTo32bit)
    {
        FIBITMAP *temp;
        temp = FreeImage_ConvertTo32Bits(img);

        if(!temp)
            return nullptr;

        FreeImage_Unload(img);
        img = temp;
    }

    return img;
}

static void mergeWithMask(FIBITMAP *image, QString pathToMask)
{
    if(!image) return;

    if(!QFile::exists(pathToMask))
        return; //Nothing to do

    FIBITMAP *mask = loadImage(pathToMask, true);

    if(!mask) return;//Nothing to do

    unsigned int img_w = FreeImage_GetWidth(image);
    unsigned int img_h = FreeImage_GetHeight(image);
    unsigned int mask_w = FreeImage_GetWidth(mask);
    unsigned int mask_h = FreeImage_GetHeight(mask);
    BYTE *img_bits  = FreeImage_GetBits(image);
    BYTE *mask_bits = FreeImage_GetBits(mask);
    BYTE *FPixP = img_bits;
    BYTE *SPixP = mask_bits;
    RGBQUAD Npix = {0x00, 0x00, 0x00, 0xFF};   //Destination pixel color
    unsigned short newAlpha = 255; //Calculated destination alpha-value

    for(unsigned int y = 0; (y < img_h) && (y < mask_h); y++)
    {
        for(unsigned int x = 0; (x < img_w) && (x < mask_w); x++)
        {
            Npix.rgbBlue = ((SPixP[FI_RGBA_BLUE] & 0x7F) | FPixP[FI_RGBA_BLUE]);
            Npix.rgbGreen = ((SPixP[FI_RGBA_GREEN] & 0x7F) | FPixP[FI_RGBA_GREEN]);
            Npix.rgbRed = ((SPixP[FI_RGBA_RED] & 0x7F) | FPixP[FI_RGBA_RED]);
            newAlpha = 255 - ((static_cast<unsigned short>(SPixP[FI_RGBA_RED]) +
                               static_cast<unsigned short>(SPixP[FI_RGBA_GREEN]) +
                               static_cast<unsigned short>(SPixP[FI_RGBA_BLUE])) / 3);

            if((SPixP[FI_RGBA_RED] > 240u) //is almost White
               && (SPixP[FI_RGBA_GREEN] > 240u)
               && (SPixP[FI_RGBA_BLUE] > 240u))
                newAlpha = 0;

            newAlpha += ((short(FPixP[FI_RGBA_RED]) +
                          short(FPixP[FI_RGBA_GREEN]) +
                          short(FPixP[FI_RGBA_BLUE])) / 3);

            if(newAlpha > 255)
                newAlpha = 255;

            FPixP[FI_RGBA_BLUE]  = Npix.rgbBlue;
            FPixP[FI_RGBA_GREEN] = Npix.rgbGreen;
            FPixP[FI_RGBA_RED]   = Npix.rgbRed;
            FPixP[FI_RGBA_ALPHA] = static_cast<BYTE>(newAlpha);
            FPixP += 4;
            SPixP += 4;
        }
    }

    FreeImage_Unload(mask);
}

static inline uint8_t subtractAlpha(const uint8_t channel, const uint8_t alpha)
{
    int16_t ch = static_cast<int16_t>(channel) - static_cast<int16_t>(alpha);
    if(ch < 0)
        ch = 0;
    return static_cast<uint8_t>(ch);
}

static void splitRGBAtoBitBlt(FIBITMAP *&image, FIBITMAP *&mask)
{
    unsigned int img_w   = FreeImage_GetWidth(image);
    unsigned int img_h   = FreeImage_GetHeight(image);

    mask = FreeImage_AllocateT(FIT_BITMAP,
                               img_w, img_h,
                               FreeImage_GetBPP(image),
                               FreeImage_GetRedMask(image),
                               FreeImage_GetGreenMask(image),
                               FreeImage_GetBlueMask(image));

    RGBQUAD Fpix;
    RGBQUAD Npix = {0x0, 0x0, 0x0, 0xFF};

    for(unsigned int y = 0; (y < img_h); y++)
    {
        for(unsigned int x = 0; (x < img_w); x++)
        {
            FreeImage_GetPixelColor(image, x, y, &Fpix);

            uint8_t grey = (255 - Fpix.rgbReserved);
            Npix.rgbRed  = grey;
            Npix.rgbGreen = grey;
            Npix.rgbBlue = grey;
            Npix.rgbReserved = 255;

            Fpix.rgbRed  = subtractAlpha(Fpix.rgbRed, grey);
            Fpix.rgbGreen = subtractAlpha(Fpix.rgbGreen, grey);
            Fpix.rgbBlue = subtractAlpha(Fpix.rgbBlue, grey);
            Fpix.rgbReserved = 255;

            FreeImage_SetPixelColor(image, x, y, &Fpix);
            FreeImage_SetPixelColor(mask,  x, y, &Npix);
        }
    }
}

static void loadQImage(QImage &target, QString file, QString maskPath = "")
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
            mergeWithMask(img, maskPath);
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



void Graphics::init()
{
    FreeImage_Initialise();
}

void Graphics::quit()
{
    FreeImage_DeInitialise();
}

void Graphics::getGifMask(QString &mask, const QString &front)
{
    mask = front;
    //Make mask filename
    int dotPos = mask.lastIndexOf('.');
    if(dotPos < 0)
        mask.push_back('m');
    else
        mask.insert(dotPos, 'm');
}


bool Graphics::loadMaskedImage(QString rootDir, QString in_imgName, QString &out_maskName, QPixmap &out_Img, QString *out_errStr)
{
    if(in_imgName.isEmpty())
    {
        if(out_errStr)
            *out_errStr = "Image filename isn't defined";
        return false;
    }

    if(!QFile::exists(rootDir + in_imgName))
    {
        if(out_errStr)
            *out_errStr = "image file is not exist: " + rootDir + in_imgName;
        return false;
    }

    out_maskName = in_imgName;
    getGifMask(out_maskName, in_imgName);

    QImage target;

    loadQImage(target, rootDir + in_imgName, rootDir + out_maskName);

    if(target.isNull())
    {
        if(out_errStr)
            *out_errStr = "Broken image file " + rootDir + in_imgName;
        return false;
    }

    //GraphicsHelps::mergeToRGBA(out_Img, out_Mask, rootDir+in_imgName, rootDir + out_maskName);
    out_Img = QPixmap::fromImage(target);

    if(out_Img.isNull())
    {
        if(out_errStr)
            *out_errStr = "Broken image file " + rootDir + in_imgName;
        return false;
    }

    if(out_errStr)
        out_errStr->clear();
    return true;
}

bool Graphics::toMaskedGif(QImage &img, QString &path)
{
    FIBITMAP* fimg = FreeImage_AllocateT(FIT_BITMAP,
                                        img.width(), img.height(), 32,
                                        FI_RGBA_RED_MASK,
                                        FI_RGBA_GREEN_MASK,
                                        FI_RGBA_BLUE_MASK);
    FIBITMAP* fmsk = NULL;

    if(!fimg)
        return false;

    int imgW = img.width();
    int imgH = img.height();
    for(int y = 0, yr = imgH - 1; y < imgH; y++, yr--)
        for(int x = 0; x < imgW; x++)
        {
            QRgb c = img.pixel(x, y);
            RGBQUAD Npix = {(BYTE)qBlue(c), (BYTE)qGreen(c), (BYTE)qRed(c), (BYTE)qAlpha(c)};
            FreeImage_SetPixelColor(fimg, x, yr, &Npix);
        }

    splitRGBAtoBitBlt(fimg, fmsk);

    QString pathMask;
    getGifMask(pathMask, path);

    if(QFile::exists(path)) // Remove old file
        QFile::remove(path);

    if(QFile::exists(pathMask)) // Remove old file
        QFile::remove(pathMask);

    bool isFail = false;

    if(fimg)
    {
        FIBITMAP *image8 = FreeImage_ColorQuantize(fimg, FIQ_WUQUANT);
        if(image8)
        {
            int ret = FreeImage_Save(FIF_GIF, image8, path.toUtf8().data());
            if(!ret)
            {
                isFail = true;
            }
            FreeImage_Unload(image8);
        }
        else
            isFail = true;
        FreeImage_Unload(fimg);
    }

    if(fmsk)
    {
        FIBITMAP *mask8  = FreeImage_ColorQuantize(fmsk, FIQ_WUQUANT);
        if(mask8)
        {
            int ret = FreeImage_Save(FIF_GIF, mask8, pathMask.toUtf8().data());
            if(!ret)
            {
                isFail = true;
            }
            FreeImage_Unload(mask8);
        }
        else
            isFail = true;
        FreeImage_Unload(fmsk);
    }

    return !isFail;
}
