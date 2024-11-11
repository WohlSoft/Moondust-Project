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

struct Spec
{
    TargetPlatform target_platform = TargetPlatform::Desktop;

    bool preserve_bitmask_appearance = true;

    QString input_dir;
    QString use_assets_dir;
    QString destination;
};

bool Convert(const Spec& spec);

} // namespace XTConvert
