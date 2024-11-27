#pragma once

#include <QString>

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

    QString input_dir;
    QString use_assets_dir;
    QString destination;
};

bool Convert(const Spec& spec);

} // namespace XTConvert
