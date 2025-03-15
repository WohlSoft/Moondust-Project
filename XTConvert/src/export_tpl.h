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

#include "texconv.h"

namespace XTConvert
{

namespace ExportTPL
{

size_t data_size(PaletteTex::PaletteSize index_size, int input_w, int input_h);

void fill_data(uint8_t* out, PaletteTex::PaletteSize index_size, int input_w, int input_h, const liq_color* palette, const uint8_t* indexes, int input_w_full);

} // namespace ExportTPL

} // namespace XTConvert
