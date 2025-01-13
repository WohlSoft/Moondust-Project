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

#include <string>

namespace XTConvert
{

enum class LogCategory
{
    ImageBitmask = 0,
    ImageTransparent,
    ImageScaledDown,
    ImageCropped,
    ImageNot2x,
    ImagePlayerCompressFailed,
    AudioQoa,
    AudioSpc2It,
    Content38AToPGEX,
    CopiedUnknown,
    SkippedUnused,
    SkippedInvalid,
    SkippedPNG,
    SkippedGIF,
    SkippedNoMask,
    ErrorMessage,
    Category_Count,
};

extern const char* const log_category[(int)LogCategory::Category_Count];

typedef void log_file_callback_t(void* userdata, LogCategory log_category, const std::string& filename);
// returns a negative number to signal interrupt
typedef int progress_callback_t(void* userdata, int cur_stage, int stage_count, const std::string& stage_name, int cur_file, int file_count, const std::string& file_name);

enum class TargetPlatform
{
    Desktop,
    TPL,
    T3X,
    DSG,
};

enum class PackageType
{
    Episode,
    AssetPack,
};

enum class ConvertGIFs
{
    Safe,
    // None, // ban this for now
    All,
};

struct Spec
{
    TargetPlatform target_platform = TargetPlatform::Desktop;
    PackageType package_type = PackageType::AssetPack;
    ConvertGIFs convert_gifs = ConvertGIFs::Safe;
    bool legacy_archives = false;

    log_file_callback_t* log_file_callback = nullptr;
    progress_callback_t* progress_callback = nullptr;
    void* callback_userdata = nullptr;

    std::string input_path;
    std::string base_assets_path;
    std::string destination;
};

bool Convert(const Spec& spec);

} // namespace XTConvert
