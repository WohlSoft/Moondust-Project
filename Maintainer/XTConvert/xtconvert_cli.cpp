#include "libxtconvert.h"

int main(int argc, char** argv)
{
    XTConvert::Spec s;
    s.target_platform = XTConvert::TargetPlatform::DSG;

    s.package_type = XTConvert::PackageType::Episode;

    s.input_dir = argv[1];
    s.use_assets_dir = argv[2];
    s.destination = argv[3];

    return !XTConvert::Convert(s);
}
