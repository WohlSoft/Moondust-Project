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

    int m_palette_used = 0;

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

    int palette_used() const
    {
        return m_palette_used;
    }
};

} // namespace XTConvert
