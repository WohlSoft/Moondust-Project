#pragma once

#include <QString>

namespace XTConvert
{

struct Spec
{
    bool target_big_endian = false;
    bool target_3ds = false;

    QString input_dir;
    QString graphics_dir;
    QString destination;
};

bool Convert(const Spec& spec);

} // namespace XTConvert
