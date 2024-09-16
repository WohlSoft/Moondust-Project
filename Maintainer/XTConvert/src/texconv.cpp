#include <FreeImageLite.h>

#include "texconv.h"

namespace XTConvert
{

void write_uint16_le(uint8_t* dest, uint16_t src)
{
    dest[0] = uint8_t(src >> 0);
    dest[1] = uint8_t(src >> 8);
}

uint16_t color_to_rgb5a1(liq_color color)
{
    return (uint16_t(color.a >= 128) << 15)
        | (uint16_t(color.r >> 3) << 10)
        | (uint16_t(color.g >> 3) << 5)
        | (uint16_t(color.b >> 3) << 0);
}

static bool s_blit(FIBITMAP* dest, FIBITMAP* src, int sx, int sy, int dx, int dy, int w, int h)
{
    FIBITMAP* view = FreeImage_CreateView(src, sx, sy, sx + w, sy + h);
    if(!view)
        return false;

    bool succ = FreeImage_Paste(dest, view, dx, dy, 255);
    FreeImage_Unload(view);

    return succ;
}

bool shrink_player_texture(FIBITMAP** image, bool char5)
{
    const int orig_frame_w = 100;
    const int orig_frame_h = 100;
    const int orig_cols = 10;
    const int orig_rows = 10;

    const int new_cols = 0;
// def do_sheet(fn, frames, out, w, h, cols, rows, make_flippable=False):
//     ow = 100
//     oh = 100
//     ocols = 10
//     orows = 10

//     cmd = f'convert -size {w*cols}x{h*rows*2} xc:none -compose Replace -gravity NorthWest -define trim:edges=east '

//     for df, sf in enumerate(frames):
//         for dir in [-1, 1]:
//             o_coord = dir * sf + 49
//             sx = ow * (o_coord // orows)
//             sy = oh * (o_coord % orows)

//             dx = w * (df // rows)
//             dy = h * (df % rows)

//             flip = False

//             dw = w

//             if dir == -1:
//                 dx = (cols - 1) * w - dx
//                 dy += rows * h

//                 if make_flippable:
//                     flip = (sf == 0)

//                     if not flip:
//                         trim = check_trim(fn, sx, sy, w, h)
//                         print(f'lo-{sf} = {trim}')
//                         dx += trim
//                         dw -= trim

//                     if sf == 25 or sf == 26:
//                         flip = True
//                 elif sf == 0:
//                     continue

//             cmd += blit(fn, sx, sy, dx, dy, dw, h, flip)

    return true;
}

PaletteTex::PaletteTex(const uint8_t* pixels_rgba, int width, int height)
{
    m_attr = liq_attr_create();
    if(!m_attr)
        return;

    liq_set_min_posterization(m_attr, 3);

    if(!pixels_rgba)
        return;

    m_image = liq_image_create_rgba(m_attr, pixels_rgba, width, height, 0);
}

PaletteTex::~PaletteTex()
{
    if(m_result)
    {
        liq_result_destroy(m_result);
        m_result = nullptr;
    }

    if(m_image)
    {
        liq_image_destroy(m_image);
        m_image = nullptr;
    }

    if(m_attr)
    {
        liq_attr_destroy(m_attr);
        m_attr = nullptr;
    }
}

bool PaletteTex::convert(PaletteSize size, int min_quality)
{
    if(!m_attr || !m_image || min_quality < 0 || min_quality > 100)
        return false;

    if(m_result)
    {
        liq_result_destroy(m_result);
        m_result = nullptr;
    }

    if(size == HALF)
        liq_set_max_colors(m_attr, 16);
    else if(size == BYTE)
        liq_set_max_colors(m_attr, 256);

    liq_set_quality(m_attr, min_quality, 100);

    if(liq_image_quantize(m_image, m_attr, &m_result) != LIQ_OK)
        return false;

    liq_set_dithering_level(m_result, 0);

    m_indexes.resize(liq_image_get_height(m_image) * liq_image_get_width(m_image));

    if(liq_write_remapped_image(m_result, m_image, m_indexes.data(), m_indexes.size()) != LIQ_OK)
    {
        m_indexes.clear();
        return false;
    }

    const liq_palette* palette = liq_get_palette(m_result);
    if(size == HALF)
        m_palette.resize(16);
    else
        m_palette.resize(256);

    if(palette->count > m_palette.size())
        return false;

    for(size_t i = 0; i < palette->count; i++)
        m_palette[i] = palette->entries[i];

    return true;
}

} // namespace XTConvert
