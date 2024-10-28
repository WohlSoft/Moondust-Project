#pragma once

#include <vector>
#include <cstdint>

#include "texconv.h"

namespace XTConvert
{

namespace ExportTPL
{

size_t data_size(PaletteTex::PaletteSize index_size, int input_w, int input_h);

void fill_data(uint8_t* out, PaletteTex::PaletteSize index_size, int input_w, int input_h, const liq_color* palette, const uint8_t* indexes);

} // namespace ExportTPL

} // namespace XTConvert
