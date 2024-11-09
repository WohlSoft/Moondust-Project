#include <QString>
#include <QFile>

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

    FIBITMAP *dest = FreeImage_Allocate(src_w / 2, src_h / 2, 32);

    if(!dest)
        return nullptr;

    uint32_t *dest_pixels  = reinterpret_cast<uint32_t*>(FreeImage_GetBits(dest));
    auto dest_pitch_px = static_cast<uint32_t>(FreeImage_GetPitch(dest)) / 4;

    for(uint32_t src_y = 0, dest_y = 0; dest_y < src_h / 2; src_y += 2, dest_y += 1)
    {
        for(uint32_t src_x = 0, dest_x = 0; dest_x < src_w / 2; src_x += 2, dest_x += 1)
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

    for(uint32_t y = 0; y < fh || y < bh; ++y)
    {
        for(uint32_t x = 0; x < fw || x < bw; ++x)
        {
            bool bp_present = y < bh && x < bw;
            bool fp_present = y < fh && x < fw;

            BYTE *fp = line1 + (y * fpitch) + (x * 4);
            BYTE *bp = line2 + (y * bpitch) + (x * 4);

            // accept vanilla GIFs that target 16-bit color depth
            // note that missing back pixels are white and absent front pixels are black
            bool bp_is_white = !bp_present || (bp[0] >= 0xf8 && bp[1] >= 0xf8 && bp[2] >= 0xf8);
            bool fp_is_white =  fp_present && (fp[0] >= 0xf8 && fp[1] >= 0xf8 && fp[2] >= 0xf8);
            bool bp_is_black =  bp_present && (bp[0] <  0x08 && bp[1] <  0x08 && bp[2] <  0x08);
            bool fp_is_black = !fp_present || (fp[0] <  0x08 && fp[1] <  0x08 && fp[2] <  0x08);

            // mask pixel is black: buffer replaced with front pixel
            if(bp_is_black)
                continue;

            // front pixel is white: buffer replaced with front pixel
            if(fp_is_white)
                continue;

            // back pixel is white and front pixel is black: buffer preserved
            if(bp_is_white && fp_is_black)
                continue;

            // pixel is matching with the front (i.e. is not an example of the lazily-made sprite)
            if(bp_present && fp_present && bp[0] == fp[0] && bp[1] == fp[1] && bp[2] == fp[2])
                continue;

            return true;
        }
    }

    return false;
}
