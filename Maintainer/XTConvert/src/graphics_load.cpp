/*
 * XTConvert - a tool to package asset packs and episodes for TheXTech game engine.
 * Copyright (c) 2024 ds-sloth
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

#include <QString>
#include <QFile>
#include <QDebug>

#include <FreeImageLite.h>

#include "graphics_load.h"

GraphicsLoad::FreeImage_Sentinel::FreeImage_Sentinel()
{
    FreeImage_Initialise();
}

GraphicsLoad::FreeImage_Sentinel::~FreeImage_Sentinel()
{
    FreeImage_DeInitialise();
}

FIBITMAP* GraphicsLoad::loadImage(const QString& path)
{
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly))
        return nullptr;

    QByteArray got = file.readAll();

    FIMEMORY* imgMEM = FreeImage_OpenMemory(reinterpret_cast<BYTE*>(got.data()),
                                            static_cast<unsigned int>(got.size()));

    FREE_IMAGE_FORMAT formato = FreeImage_GetFileTypeFromMemory(imgMEM);

    if(formato == FIF_UNKNOWN)
    {
        FreeImage_CloseMemory(imgMEM);
        return nullptr;
    }

    FIBITMAP* img = FreeImage_LoadFromMemory(formato, imgMEM, 0);
    FreeImage_CloseMemory(imgMEM);

    if(!img)
        return nullptr;

    if(FreeImage_GetBPP(img) == 32)
        return img;

    FIBITMAP* img_rgba = FreeImage_ConvertTo32Bits(img);

    FreeImage_Unload(img);

    return img_rgba;
}

void GraphicsLoad::RGBAToMask(FIBITMAP* mask)
{
    if(!mask)
        return;

    // convert alpha channel to luminance
    unsigned int img_w  = FreeImage_GetWidth(mask);
    unsigned int img_h  = FreeImage_GetHeight(mask);

    uint8_t *pixel_data = reinterpret_cast<uint8_t*>(FreeImage_GetBits(mask));
    auto dest_px_stride = static_cast<uint32_t>(FreeImage_GetPitch(mask));

    for(unsigned int y = 0; (y < img_h); y++)
    {
        for(unsigned int x = 0; (x < img_w); x++)
        {
            uint8_t* pixel = &pixel_data[dest_px_stride * y + 4 * x];

            uint8_t grey = 0xFF - pixel[3];
            pixel[0] = grey;
            pixel[1] = grey;
            pixel[2] = grey;
            pixel[3] = 0xFF;
        }
    }
}

static constexpr RGBQUAD s_bitblitBG = {0, 0, 0, 0xFF};

void GraphicsLoad::mergeWithMask(FIBITMAP *image, FIBITMAP *mask)
{
    unsigned int img_w = FreeImage_GetWidth(image);
    unsigned int img_h = FreeImage_GetHeight(image);
    unsigned int img_pitch = FreeImage_GetPitch(image);
    unsigned int mask_w = FreeImage_GetWidth(mask);
    unsigned int mask_h = FreeImage_GetHeight(mask);
    unsigned int mask_pitch = FreeImage_GetPitch(mask);
    BYTE *img_bits  = FreeImage_GetBits(image);
    BYTE *mask_bits = FreeImage_GetBits(mask);
    BYTE *FPixP = nullptr;
    BYTE *SPixP = mask_bits;
    RGBQUAD Npix = {0x00, 0x00, 0x00, 0xFF};   //Destination pixel color
    BYTE Wpix[] = {0xFF, 0xFF, 0xFF, 0xFF};   //Dummy white pixel
    unsigned short newAlpha = 0xFF; //Calculated destination alpha-value

    bool endOfY = false;
    unsigned int ym = mask_h - 1;
    unsigned int y = img_h - 1;

    while(1)
    {
        FPixP = img_bits + (img_pitch * y);
        if(!endOfY)
            SPixP = mask_bits + (mask_pitch * ym);

        for(unsigned int x = 0; (x < img_w); x++)
        {
            Npix.rgbBlue = ((SPixP[FI_RGBA_BLUE] & s_bitblitBG.rgbBlue) | FPixP[FI_RGBA_BLUE]);
            Npix.rgbGreen = ((SPixP[FI_RGBA_GREEN] & s_bitblitBG.rgbGreen) | FPixP[FI_RGBA_GREEN]);
            Npix.rgbRed = ((SPixP[FI_RGBA_RED] & s_bitblitBG.rgbRed) | FPixP[FI_RGBA_RED]);

            // these terms (mask pixel and not front pixel, bitwise) represent the values that could be seen in SMBX 1.3
            newAlpha = 255 - ((static_cast<unsigned short>(SPixP[FI_RGBA_RED] & ~FPixP[FI_RGBA_RED]) +
                               static_cast<unsigned short>(SPixP[FI_RGBA_GREEN] & ~FPixP[FI_RGBA_GREEN]) +
                               static_cast<unsigned short>(SPixP[FI_RGBA_BLUE] & ~FPixP[FI_RGBA_BLUE])) / 3);

            if(newAlpha < 16)
                newAlpha = 0;

            if(newAlpha > 240)
                newAlpha = 255;

            FPixP[FI_RGBA_BLUE]  = Npix.rgbBlue;
            FPixP[FI_RGBA_GREEN] = Npix.rgbGreen;
            FPixP[FI_RGBA_RED]   = Npix.rgbRed;
            FPixP[FI_RGBA_ALPHA] = static_cast<BYTE>(newAlpha);
            FPixP += 4;

            if(x >= mask_w - 1 || endOfY)
                SPixP = Wpix;
            else
                SPixP += 4;
        }

        if(ym == 0)
        {
            endOfY = true;
            SPixP = Wpix;
        }
        else
            ym--;

        if(y == 0)
            break;
        y--;
    }
}

FIBITMAP *GraphicsLoad::fast2xScaleDown(FIBITMAP *image)
{
    if(!image)
        return nullptr;

    if(FreeImage_GetBPP(image) != 32)
        return nullptr;

    auto src_w = static_cast<uint32_t>(FreeImage_GetWidth(image));
    auto src_h = static_cast<uint32_t>(FreeImage_GetHeight(image));
    const uint32_t *src_pixels  = reinterpret_cast<uint32_t*>(FreeImage_GetBits(image));
    auto src_pitch_px = static_cast<uint32_t>(FreeImage_GetPitch(image)) / 4;

    // consider rounding up instead of down in the future
    auto dest_w = src_w / 2;
    auto dest_h = src_h / 2;

    if(dest_w == 0)
        dest_w = 1;

    if(dest_h == 0)
        dest_h = 1;

    FIBITMAP *dest = FreeImage_Allocate(dest_w, dest_h, 32);

    if(!dest)
        return nullptr;

    uint32_t *dest_pixels = reinterpret_cast<uint32_t*>(FreeImage_GetBits(dest));
    auto dest_pitch_px = static_cast<uint32_t>(FreeImage_GetPitch(dest)) / 4;

    for(uint32_t src_y = 0, dest_y = 0; dest_y < dest_h; src_y += 2, dest_y += 1)
    {
        for(uint32_t src_x = 0, dest_x = 0; dest_x < dest_w; src_x += 2, dest_x += 1)
        {
            dest_pixels[dest_y * dest_pitch_px + dest_x] = src_pixels[src_y * src_pitch_px + src_x];
        }
    }

    return dest;
}

bool GraphicsLoad::validateBitmaskRequired(FIBITMAP *image, FIBITMAP *mask)
{
    if(!image || !mask)
        return false;

    auto fw = static_cast<uint32_t>(FreeImage_GetWidth(image));
    auto fh = static_cast<uint32_t>(FreeImage_GetHeight(image));
    auto fpitch = static_cast<uint32_t>(FreeImage_GetPitch(image));
    BYTE *fimg_bits  = FreeImage_GetBits(image);

    auto bw = static_cast<uint32_t>(FreeImage_GetWidth(mask));
    auto bh = static_cast<uint32_t>(FreeImage_GetHeight(mask));
    auto bpitch = static_cast<uint32_t>(FreeImage_GetPitch(mask));
    BYTE *bimg_bits  = FreeImage_GetBits(mask);

    BYTE *line1 = fimg_bits;
    BYTE *line2 = bimg_bits;

    int blend_count = 0;
    int blend_bits = 0;

    BYTE fp_default[4] = {0, 0, 0, 0};
    BYTE bp_default[4] = {255, 255, 255, 255};

    for(uint32_t y = 0; y < fh || y < bh; ++y)
    {
        for(uint32_t x = 0; x < fw || x < bw; ++x)
        {
            bool bp_present = y < bh && x < bw;
            bool fp_present = y < fh && x < fw;

            BYTE *fp = line1 + (y * fpitch) + (x * 4);
            BYTE *bp = line2 + (y * bpitch) + (x * 4);

            if(!fp_present)
                fp = fp_default;

            if(!bp_present)
                bp = bp_default;

            // accept vanilla GIFs that target 16-bit color depth
            // note that missing back pixels are white and absent front pixels are black
            bool bp_is_white = bp[0] >= 0xf0 && bp[1] >= 0xf0 && bp[2] >= 0xf0;
            bool fp_is_black = fp[0] <  0x10 && fp[1] <  0x10 && fp[2] <  0x10;

            // back pixel is white and front pixel is black: buffer preserved
            if(bp_is_white && fp_is_black)
                continue;

            // front contains back: buffer replaced with front pixel
            // (this subsumes mask pixel black, front pixel white, and matching cases)
            if((bp[0] & ~fp[0] & 0xf8) == 0 && (bp[1] & ~fp[1] & 0xf8) == 0 && (bp[2] & ~fp[2] & 0xf8) == 0)
                continue;

            blend_count++;
            blend_bits += (bp[0] & ~fp[0]) / 0x08;
            blend_bits += (bp[1] & ~fp[1]) / 0x08;
            blend_bits += (bp[2] & ~fp[2]) / 0x08;
        }
    }

    return (blend_bits >= 10000);
}

bool GraphicsLoad::validateForDepthTest(FIBITMAP *image)
{
    if(!image)
        return false;

    auto w = static_cast<uint32_t>(FreeImage_GetWidth(image));
    auto h = static_cast<uint32_t>(FreeImage_GetHeight(image));
    auto pitch = static_cast<uint32_t>(FreeImage_GetPitch(image));
    BYTE *img_bits  = FreeImage_GetBits(image);

    for(uint32_t y = 0; y < h; ++y)
    {
        for(uint32_t x = 0; x < w; ++x)
        {
            BYTE *alpha = img_bits + (y * pitch) + (x * 4) + 3;

            // vanilla game used 5 bits per channel, so we set the alpha test as >= 0x08
            if(*alpha < 0x08 || *alpha >= 0xf8)
                continue;

            // qInfo() << "alpha " << *alpha << " at " << x << ", " << y;

            return false;
        }
    }

    return true;
}
