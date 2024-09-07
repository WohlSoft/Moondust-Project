#include "libxtconvert.h"

int main(int argc, char** argv)
{
    XTConvert::Spec s;
    // c.m_spec.target_3ds = true;

    s.input_dir = argv[1];
    s.destination = argv[2];

    return !XTConvert::Convert(s);
}
