#pragma once

#include <cstdint>
#include <cstddef>

namespace XTConvert
{

bool extract_archive_file(const char* dest_dir, const char* filename);
bool extract_archive_data(const char* dest_dir, const uint8_t* data, size_t len);

} // namespace XTConvert
