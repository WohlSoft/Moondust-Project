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

bool shrink_player_texture(FIBITMAP** image, bool char5, int px_scale)
{
    const int source_frame_w = 50 * px_scale;
    const int source_frame_h = 50 * px_scale;
    const int source_cols = 10;
    const int source_rows = 10;

    const int dest_frame_w = ((char5) ? 32 : 24) * px_scale;
    const int dest_frame_h = 32 * px_scale;
    const int dest_cols = (char5) ? 4 : 10;
    const int dest_rows_per_dir = 4;
    const int dest_rows = dest_rows_per_dir * 2;

    FIBITMAP* source_image = *image;

    if((int)FreeImage_GetWidth(source_image) != source_frame_w * source_cols || (int)FreeImage_GetHeight(source_image) != source_frame_h * source_rows)
        return false;

    FIBITMAP* dest_image = FreeImage_AllocateT(FIT_BITMAP, dest_frame_w * dest_cols, dest_frame_h * dest_rows, 32);

    if(!dest_image)
        return false;

    int invalid = 0;

    int copy_frame_count = (char5) ? 16 : 38;

    for(int dest_frame = 0; dest_frame < copy_frame_count; dest_frame++)
    {
        int source_frame = (dest_frame < 33) ? dest_frame : dest_frame + 7;

        if(char5)
            source_frame += 1;

        for(int dir = -1; dir <= 1; dir += 2)
        {
            int source_frame_res = dir * source_frame + 49;
            int source_x = source_frame_w * (source_frame_res / source_rows);
            int source_y = source_frame_h * (source_frame_res % source_rows);

            int dest_x = dest_frame_w * (dest_frame / dest_rows_per_dir);
            int dest_y = dest_frame_h * (dest_frame % dest_rows_per_dir);

            if(dir == -1)
            {
                dest_x = (dest_cols - 1) * dest_frame_w - dest_x;
                dest_y += dest_rows_per_dir * dest_frame_h;
            }

            s_blit(dest_image, source_image, source_x, source_y, dest_x, dest_y, dest_frame_w, dest_frame_h);

            // confirm that it's valid
            for(int ox = 0; ox < source_frame_w; ox++)
            {
                for(int oy = 0; oy < source_frame_h; oy++)
                {
                    if(ox < dest_frame_w + 2 && oy < dest_frame_h + 2)
                        continue;

                    RGBQUAD quad;
                    if(FreeImage_GetPixelColor(source_image, source_x + ox, source_frame_h * source_rows - (source_y + oy), &quad) && quad.rgbReserved >= 0x40)
                        invalid++;
                }
            }
        }
    }

    bool okay = (invalid <= 20);

    if(okay)
    {
        FreeImage_Unload(source_image);
        *image = dest_image;
    }
    else
        FreeImage_Unload(dest_image);

    return okay;
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

    m_palette_used = palette->count;

    for(size_t i = 0; i < palette->count; i++)
        m_palette[i] = palette->entries[i];

    return true;
}

} // namespace XTConvert
