/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <QSysInfo>

#include "graphics_funcs.h"
#include "file_mapper.h"
#include "logger.h"

#ifdef DEBUG_BUILD
#include <QElapsedTimer>
#endif

#include <ConfigPackManager/image_size.h>

#include <_resources/resource.h>

#ifdef _WIN32
#include <SDL2/SDL_syswm.h>
#define FREEIMAGE_LIB 1
#endif
#include <FreeImageLite.h>

void GraphicsHelps::initFreeImage()
{
    FreeImage_Initialise();
}

void GraphicsHelps::closeFreeImage()
{
    FreeImage_DeInitialise();
}

FIBITMAP *GraphicsHelps::loadImage(QString file, bool convertTo32bit)
{
#ifdef DEBUG_BUILD
    QElapsedTimer loadingTime;
    QElapsedTimer fReadTime;
    QElapsedTimer imgConvTime;
    loadingTime.start();
    fReadTime.start();
#endif
#if  defined(__unix__) || defined(__APPLE__) || defined(_WIN32)
    PGE_FileMapper fileMap;

    if(!fileMap.open_file(file.toUtf8().data()))
        return NULL;

    FIMEMORY *imgMEM = FreeImage_OpenMemory(reinterpret_cast<unsigned char *>(fileMap.data),
                                            static_cast<unsigned int>(fileMap.size));
    FREE_IMAGE_FORMAT formato = FreeImage_GetFileTypeFromMemory(imgMEM);

    if(formato  == FIF_UNKNOWN)
        return NULL;

    FIBITMAP *img = FreeImage_LoadFromMemory(formato, imgMEM, 0);
    FreeImage_CloseMemory(imgMEM);
    fileMap.close_file();

    if(!img)
        return NULL;

#else
    FREE_IMAGE_FORMAT formato = FreeImage_GetFileType(file.toUtf8().data(), 0);

    if(formato  == FIF_UNKNOWN)
        return NULL;

    FIBITMAP *img = FreeImage_Load(formato, file.toUtf8().data());

    if(!img)
        return NULL;

#endif
#ifdef DEBUG_BUILD
    long long fReadTimeElapsed = static_cast<long long>(fReadTime.elapsed());
    long long imgConvertElapsed = 0;
#endif

    if(convertTo32bit)
    {
#ifdef DEBUG_BUILD
        imgConvTime.start();
#endif
        FIBITMAP *temp;
        temp = FreeImage_ConvertTo32Bits(img);

        if(!temp)
            return NULL;

        FreeImage_Unload(img);
        img = temp;
#ifdef DEBUG_BUILD
        imgConvertElapsed = static_cast<long long>(imgConvTime.elapsed());
#endif
    }

#ifdef DEBUG_BUILD
    std::string file_s = file.toStdString();
    D_pLogDebug("File read of texture %s passed in %d milliseconds", file_s.c_str(), fReadTimeElapsed);
    D_pLogDebug("Conv to 32-bit of %s passed in %d milliseconds", file_s.c_str(), imgConvertElapsed);
    D_pLogDebug("Total Loading of image %s passed in %d milliseconds", file_s.c_str(), static_cast<long long>(loadingTime.elapsed()));
#endif
    return img;
}

FIBITMAP *GraphicsHelps::loadImageRC(const char *file)
{
    unsigned char *memory = nullptr;
    size_t fileSize = 0;
    if(!RES_getMem(file, memory, fileSize))
    {
        pLogCritical("Resource file \"%s\" is not found!", file);
        return nullptr;
    }
    FIMEMORY *imgMEM = FreeImage_OpenMemory(memory, static_cast<FI_DWORD>(fileSize));
    FREE_IMAGE_FORMAT formato = FreeImage_GetFileTypeFromMemory(imgMEM);

    if(formato == FIF_UNKNOWN)
        return nullptr;

    FIBITMAP *img = FreeImage_LoadFromMemory(formato, imgMEM, 0);
    FreeImage_CloseMemory(imgMEM);

    if(!img)
        return nullptr;

    FIBITMAP *temp;
    temp = FreeImage_ConvertTo32Bits(img);

    if(!temp)
        return nullptr;

    FreeImage_Unload(img);
    img = temp;
    return img;
}

void GraphicsHelps::closeImage(FIBITMAP *img)
{
    FreeImage_Unload(img);
}

SDL_Surface *GraphicsHelps::fi2sdl(FIBITMAP *img)
{
    int h = static_cast<int>(FreeImage_GetHeight(img));
    int w = static_cast<int>(FreeImage_GetWidth(img));
    FreeImage_FlipVertical(img);
    SDL_Surface *surf = SDL_CreateRGBSurfaceFrom(FreeImage_GetBits(img),
                        w, h, 32, w * 4, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, FI_RGBA_ALPHA_MASK);
    return surf;
}

void GraphicsHelps::mergeWithMask(FIBITMAP *image, QString pathToMask)
{
    if(!image)
        return;

    if(!QFileInfo(pathToMask).exists())
        return; //Nothing to do

    FIBITMAP *mask = loadImage(pathToMask, true);

    if(!mask)
        return;//Nothing to do

    unsigned int img_w = FreeImage_GetWidth(image);
    unsigned int img_h = FreeImage_GetHeight(image);
    unsigned int mask_w = FreeImage_GetWidth(mask);
    unsigned int mask_h = FreeImage_GetHeight(mask);
    BYTE *img_bits  = FreeImage_GetBits(image);
    BYTE *mask_bits = FreeImage_GetBits(mask);
    BYTE *FPixP = img_bits;
    BYTE *SPixP = mask_bits;
    RGBQUAD Npix = {0x00, 0x00, 0x00, 0xFF};   //Destination pixel color
    unsigned short newAlpha = 0xFF; //Calculated destination alpha-value

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

            newAlpha += ((static_cast<unsigned short>(FPixP[FI_RGBA_RED]) +
                          static_cast<unsigned short>(FPixP[FI_RGBA_GREEN]) +
                          static_cast<unsigned short>(FPixP[FI_RGBA_BLUE])) / 3);

            if(newAlpha > 255) newAlpha = 255;

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


bool GraphicsHelps::getImageMetrics(QString imageFile, PGE_Size *imgSize)
{
    if(!imgSize)
        return false;

    FIBITMAP *img = loadImage(imageFile);

    if(!img)
        return false;
    else
    {
        int w = static_cast<int>(FreeImage_GetWidth(img));
        int h = static_cast<int>(FreeImage_GetHeight(img));
        imgSize->setSize(w, h);
        GraphicsHelps::closeImage(img);
    }

    return true;
}


void GraphicsHelps::getMaskedImageInfo(QString rootDir, QString in_imgName, QString &out_maskName, QString &out_errStr, PGE_Size *imgSize)
{
    if(in_imgName.isEmpty())
    {
        out_errStr = "Image filename isn't defined";
        return;
    }

    int errorCode, w, h;

    if(!PGE_ImageInfo::getImageSize(rootDir + in_imgName, &w, &h, &errorCode))
    {
        switch(errorCode)
        {
        case PGE_ImageInfo::ERR_UNSUPPORTED_FILETYPE:
            out_errStr = "Unsupported or corrupted file format: " + rootDir + in_imgName;
            break;

        case PGE_ImageInfo::ERR_NOT_EXISTS:
            out_errStr = "image file is not exist: " + rootDir + in_imgName;
            break;

        case PGE_ImageInfo::ERR_CANT_OPEN:
            out_errStr = "Can't open image file: " + rootDir + in_imgName;
            break;
        }

        return;
    }

    out_maskName = PGE_ImageInfo::getMaskName(in_imgName);
    out_errStr = "";

    if(imgSize)
    {
        imgSize->setWidth(w);
        imgSize->setHeight(h);
    }
}


/*********************Code from Qt*********************/

static inline QRgb qt_gl_convertToGLFormatHelper(QRgb src_pixel, GLenum texture_format)
{
    if(texture_format == GL_BGRA)
    {
        if(QSysInfo::ByteOrder == QSysInfo::BigEndian)
        {
            return ((src_pixel << 24) & 0xff000000)
                   | ((src_pixel >> 24) & 0x000000ff)
                   | ((src_pixel << 8) & 0x00ff0000)
                   | ((src_pixel >> 8) & 0x0000ff00);
        }
        else
            return src_pixel;
    }
    else      // GL_RGBA
    {
        if(QSysInfo::ByteOrder == QSysInfo::BigEndian)
            return (src_pixel << 8) | ((src_pixel >> 24) & 0xff);
        else
        {
            return ((src_pixel << 16) & 0xff0000)
                   | ((src_pixel >> 16) & 0xff)
                   | (src_pixel & 0xff00ff00);
        }
    }
}

static void convertToGLFormatHelper(QImage &dst, const QImage &img, GLenum texture_format)
{
    Q_ASSERT(dst.depth() == 32);
    Q_ASSERT(img.depth() == 32);

    if(dst.size() != img.size())
    {
        int target_width = dst.width();
        int target_height = dst.height();
        qreal sx = target_width / qreal(img.width());
        qreal sy = target_height / qreal(img.height());
        quint32 *dest = reinterpret_cast<quint32 *>(dst.scanLine(0)); // NB! avoid detach here
        const uchar *srcPixels = img.constScanLine(img.height() - 1);
        int sbpl = img.bytesPerLine();
        int dbpl = dst.bytesPerLine();
        int ix = int(0x00010000 / sx);
        int iy = int(0x00010000 / sy);
        quint32 basex = static_cast<quint32>(0.5 * static_cast<double>(ix));
        quint32 srcy = static_cast<quint32>(0.5 * static_cast<double>(iy));

        // scale, swizzle and mirror in one loop
        while(target_height--)
        {
            const uint *src = reinterpret_cast<const quint32 *>(
                                  srcPixels -
                                  (srcy >> 16) * static_cast<unsigned int>(sbpl));
            int srcx = static_cast<int>(basex);

            for(int x = 0; x < target_width; ++x)
            {
                dest[x] = qt_gl_convertToGLFormatHelper(src[srcx >> 16], texture_format);
                srcx += ix;
            }

            dest = reinterpret_cast<quint32 *>((reinterpret_cast<uchar *>(dest)) + dbpl);
            srcy += static_cast<unsigned int>(iy);
        }
    }
    else
    {
        const int width = img.width();
        const int height = img.height();
        const uint *p = reinterpret_cast<const uint *>(img.scanLine(img.height() - 1));
        uint *q = reinterpret_cast<uint *>(dst.scanLine(0));

        if(texture_format == GL_BGRA)
        {
            if(QSysInfo::ByteOrder == QSysInfo::BigEndian)
            {
                // mirror + swizzle
                for(int i = 0; i < height; ++i)
                {
                    const uint *end = p + width;

                    while(p < end)
                    {
                        *q = ((*p << 24) & 0xff000000)
                             | ((*p >> 24) & 0x000000ff)
                             | ((*p << 8) & 0x00ff0000)
                             | ((*p >> 8) & 0x0000ff00);
                        p++;
                        q++;
                    }

                    p -= 2 * width;
                }
            }
            else
            {
                const uint bytesPerLine = static_cast<const uint>(img.bytesPerLine());

                for(int i = 0; i < height; ++i)
                {
                    memcpy(q, p, bytesPerLine);
                    q += width;
                    p -= width;
                }
            }
        }
        else
        {
            if(QSysInfo::ByteOrder == QSysInfo::BigEndian)
            {
                for(int i = 0; i < height; ++i)
                {
                    const uint *end = p + width;

                    while(p < end)
                    {
                        *q = (*p << 8) | ((*p >> 24) & 0xff);
                        p++;
                        q++;
                    }

                    p -= 2 * width;
                }
            }
            else
            {
                for(int i = 0; i < height; ++i)
                {
                    const uint *end = p + width;

                    while(p < end)
                    {
                        *q = ((*p << 16) & 0xff0000) | ((*p >> 16) & 0xff) | (*p & 0xff00ff00);
                        p++;
                        q++;
                    }

                    p -= 2 * width;
                }
            }
        }
    }
}

QImage GraphicsHelps::convertToGLFormat(const QImage &img)
{
    QImage res(img.size(), QImage::Format_ARGB32);
    convertToGLFormatHelper(res, img.convertToFormat(QImage::Format_ARGB32), GL_RGBA);
    return res;
}

bool GraphicsHelps::setWindowIcon(SDL_Window *window, FIBITMAP *img, int iconSize)
{
#ifdef _WIN32
    struct SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);

    if(-1 == SDL_GetWindowWMInfo(window, &wmInfo))
        return false;

    if(wmInfo.subsystem != SDL_SYSWM_WINDOWS)
        return false;

    HWND windowH = wmInfo.info.win.window;
    HICON hicon = NULL;
    BYTE *icon_bmp;
    unsigned int icon_len, y;
    SDL_RWops *dst;
    unsigned int w = FreeImage_GetWidth(img);
    unsigned int h = FreeImage_GetWidth(img);
    Uint8 *bits = (Uint8 *)FreeImage_GetBits(img);
    unsigned int pitch = FreeImage_GetPitch(img);
    /* Create temporary bitmap buffer */
    icon_len = 40 + h * w * 4;
    icon_bmp = SDL_stack_alloc(BYTE, icon_len);
    dst = SDL_RWFromMem(icon_bmp, icon_len);

    if(!dst)
    {
        SDL_stack_free(icon_bmp);
        return false;
    }

    /* Write the BITMAPINFO header */
    SDL_WriteLE32(dst, 40);
    SDL_WriteLE32(dst, w);
    SDL_WriteLE32(dst, h * 2);
    SDL_WriteLE16(dst, 1);
    SDL_WriteLE16(dst, 32);
    SDL_WriteLE32(dst, BI_RGB);
    SDL_WriteLE32(dst, h * w * 4);
    SDL_WriteLE32(dst, 0);
    SDL_WriteLE32(dst, 0);
    SDL_WriteLE32(dst, 0);
    SDL_WriteLE32(dst, 0);
    y = 0;

    do
    {
        Uint8 *src = bits + y * pitch;
        SDL_RWwrite(dst, src, pitch, 1);
    }
    while(++y < h);

    hicon = CreateIconFromResource(icon_bmp, icon_len, TRUE, 0x00030000);
    SDL_RWclose(dst);
    SDL_stack_free(icon_bmp);
    /* Set the icon for the window */
    SendMessage(windowH, WM_SETICON, (iconSize < 32) ? ICON_SMALL : ICON_BIG, (LPARAM) hicon);
#else
    (void)iconSize;
    SDL_Surface *sicon = GraphicsHelps::fi2sdl(img);
    SDL_SetWindowIcon(window, sicon);
    SDL_FreeSurface(sicon);
    const char *error = SDL_GetError();

    if(*error != '\0')
        return false;

#endif
    return true;
}


/*********************Code from Qt**end****************/
