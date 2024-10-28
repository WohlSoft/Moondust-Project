#include <cstring>

#include "export_tpl.h"

namespace XTConvert
{

namespace ExportTPL
{

void write_uint16_be(uint8_t* dest, uint16_t src)
{
    dest[0] = uint8_t(src >> 8);
    dest[1] = uint8_t(src >> 0);
}

uint16_t color_to_rgb5a3(liq_color color)
{
    if(color.a < 0xf8)
    {
        return (uint16_t(color.a >> 5) << 12)
            | (uint16_t(color.b >> 4) << 8)
            | (uint16_t(color.g >> 4) << 4)
            | (uint16_t(color.r >> 4) << 0);
    }
    else
    {
        return (uint16_t(1) << 15)
            | (uint16_t(color.b >> 3) << 10)
            | (uint16_t(color.g >> 3) << 5)
            | (uint16_t(color.r >> 3) << 0);
    }
}

size_t data_size(PaletteTex::PaletteSize index_size, int input_w, int input_h)
{
    size_t indexes_per_byte = (index_size == PaletteTex::HALF) ? 2 : 1;

    size_t img_height = next_power_of_2(input_h);
    size_t img_width  = next_power_of_2(input_w);

    if(img_width > 1024)
        img_width = 1024;

    size_t file_hdr   = 0x60;
    size_t img_data   = img_height * img_width / indexes_per_byte;
    size_t pal_data   = (index_size == PaletteTex::HALF) ? 0x20 : 0x200;

    return file_hdr + img_data + pal_data;
}

void fill_data(uint8_t* out, PaletteTex::PaletteSize index_size, int input_w, int input_h, const liq_color* palette, const uint8_t* indexes)
{
    size_t indexes_per_byte = (index_size == PaletteTex::HALF) ? 2 : 1;

    size_t img_height = (input_h + 7) / 8 * 8;
    size_t img_width  = (input_w + 7) / 8 * 8;

    if(img_width > 1024)
        img_width = 1024;

    size_t file_hdr   = 0x60;
    size_t img_data   = img_height * img_width / indexes_per_byte;
    size_t pal_data   = (index_size == PaletteTex::HALF) ? 0x20 : 0x200;

    // calculations for headers
    size_t pal_dat_off = file_hdr + img_data;
    uint8_t pdo1 = uint8_t(pal_dat_off >> 24);
    uint8_t pdo2 = uint8_t(pal_dat_off >> 16);
    uint8_t pdo3 = uint8_t(pal_dat_off >>  8);
    uint8_t pdo4 = uint8_t(pal_dat_off >>  0);

    size_t palette_entry_count = (index_size == PaletteTex::HALF) ? 16 : 256;
    uint8_t pec1 = palette_entry_count / 0x100;
    uint8_t pec2 = palette_entry_count &  0xff;

    uint8_t ihe1 = img_height / 0x100;
    uint8_t ihe2 = img_height &  0xff;

    uint8_t iwi1 = img_width / 0x100;
    uint8_t iwi2 = img_width &  0xff;

    uint8_t ifmt = (index_size == PaletteTex::HALF) ? 0x08 : 0x09;

    // file header template
    const uint8_t file_header[0x60] = {
        0x00, 0x20, 0xaf, 0x30, // magic number
        0x00, 0x00, 0x00, 0x01, // image count
        0x00, 0x00, 0x00, 0x0c, // image offset table offset
        0x00, 0x00, 0x00, 0x14, // image header offset

        0x00, 0x00, 0x00, 0x40, // palette header offset
        ihe1, ihe2, iwi1, iwi2, // [[image header]] image dimensions
        0x00, 0x00, 0x00, ifmt, // image format
        0x00, 0x00, 0x00, 0x60, // image data offset

        0x00, 0x00, 0x00, 0x00, // wrap s
        0x00, 0x00, 0x00, 0x00, // wrap t
        0x00, 0x00, 0x00, 0x01, // min filter
        0x00, 0x00, 0x00, 0x01, // mag filter

        0x00, 0x00, 0x00, 0x00, // LOD bias
        0x00, 0x00, 0x00, 0x00, // flags
        0x00, 0x00, 0x00, 0x00, // padding
        0x00, 0x00, 0x00, 0x00, // padding

        pec1, pec2, 0x00, 0x00, // palette header (pt 1)
        0x00, 0x00, 0x00, 0x02, // palette format
        pdo1, pdo2, pdo3, pdo4, // palette data offset

        // PADDING
    };

    memcpy(out, file_header, 0x60);

    // palette data
    {
        uint8_t* pal_out = out + pal_dat_off;

        for(unsigned pal_i = 0; pal_i < palette_entry_count; pal_i++)
            write_uint16_be(pal_out + pal_i * 2, color_to_rgb5a3(palette[pal_i]));
    }

    // image data
    {
        uint8_t* img_out = out + 0x60;

        uint8_t* dest = img_out;

        int blk_width = 8;
        int blk_stride = (index_size == PaletteTex::HALF) ? 4 : 8;
        int blk_height = (index_size == PaletteTex::HALF) ? 8 : 4;

        for(int start_y = 0; start_y < (int)img_height; start_y += blk_height)
        {
            for(int start_x = 0; start_x < (int)img_width; start_x += blk_width)
            {
                for(int dy = 0; dy < blk_height; dy++)
                {
                    if(start_y + dy >= input_h)
                    {
                        dest += blk_stride;
                        continue;
                    }

                    const uint8_t* src = indexes + (start_y + dy) * input_w + start_x;

                    if(index_size == PaletteTex::HALF)
                    {
                        for(int dx = 0; dx < blk_width; dx += 2)
                        {
                            if(start_x + dx + 1 > input_w)
                                dest++;
                            else if(start_x + dx + 1 == input_w)
                                *(dest++) = (src[dx] << 4);
                            else
                                *(dest++) = (src[dx + 0] << 4) | (src[dx + 1] << 0);
                        }
                    }
                    else
                    {
                        for(int dx = 0; dx < blk_width; dx++)
                        {
                            if(start_x + dx >= input_w)
                                dest++;
                            else
                                *(dest++) = src[dx];
                        }
                    }
                }
            }
        }
    }
}

} // namespace ExportTPL

} // namespace XTConvert
