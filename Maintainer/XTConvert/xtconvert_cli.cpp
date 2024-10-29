#include "libxtconvert.h"

int main(int argc, char** argv)
{
    XTConvert::Spec s;
    s.target_platform = XTConvert::TargetPlatform::T3X;

    s.input_dir = argv[1];
    s.destination = argv[2];

    return !XTConvert::Convert(s);
}
