#pragma once

#include <vector>
#include <cstdint>

#include "libimagequant.h"

struct FIBITMAP;

namespace XTConvert
{

static inline int next_power_of_2(int val)
{
    int power = 8;
    while(power < val)
        power *= 2;
    return power;
}

void write_uint16_le(uint8_t* dest, uint16_t src);
uint16_t color_to_rgb5a1(liq_color color);

bool shrink_player_texture(FIBITMAP** image, bool char5);

struct PaletteTex
{
private:
    liq_attr* m_attr = nullptr;
    liq_image* m_image = nullptr;
    liq_result* m_result = nullptr;

    std::vector<liq_color> m_palette;
    std::vector<uint8_t> m_indexes;

public:
    enum PaletteSize
    {
        HALF,
        BYTE,
    };

    PaletteTex(const uint8_t* pixels_rgba, int width, int height);
    ~PaletteTex();

    bool convert(PaletteSize size, int min_quality);

    const liq_color* palette() const
    {
        if(m_palette.empty())
            return nullptr;

        return m_palette.data();
    }

    const uint8_t* indexes() const
    {
        if(m_indexes.empty())
            return nullptr;

        return m_indexes.data();
    }
};

} // namespace XTConvert
