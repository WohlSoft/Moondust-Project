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
    None, // ban this for now
    All,
};

struct Spec
{
    TargetPlatform target_platform = TargetPlatform::Desktop;
    PackageType package_type = PackageType::AssetPack;
    ConvertGIFs convert_gifs = ConvertGIFs::Safe;
    bool use_romfs_3ds = false;

    std::string input_path;
    std::string base_assets_path;
    std::string destination;
};

bool Convert(const Spec& spec);

} // namespace XTConvert
