#pragma once

#include <array>

namespace XTConvert
{

void init_mask_arrays();
bool check_BlockHasNoMask(unsigned int index);
bool check_BackgroundHasNoMask(unsigned int index);

} // namespace XTConvert
