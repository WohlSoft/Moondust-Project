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
    bool force_iso_3ds = false;

    std::string input_path;
    std::string base_assets_path;
    std::string destination;
};

bool Convert(const Spec& spec);

} // namespace XTConvert
