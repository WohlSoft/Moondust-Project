#include <stdio.h>
#include "ConfigPackManager/image_size.h"

int main()
{
    uint32_t w, h;
    int err;
    if(!PGE_ImageInfo::getImageSize("../res/test.bmp", &w, &h, &err))
        fprintf(stderr, "BMP failed: %d\n", err);
    else
        fprintf(stderr, "BMP: %u x %u\n", w, h);

    if(!PGE_ImageInfo::getImageSize("../res/test.png", &w, &h, &err))
        fprintf(stderr, "PNG failed: %d\n", err);
    else
        fprintf(stderr, "PNG: %u x %u\n", w, h);

    if(!PGE_ImageInfo::getImageSize("../res/test.gif", &w, &h, &err))
        fprintf(stderr, "GIF failed: %d\n", err);
    else
        fprintf(stderr, "GIF: %u x %u\n", w, h);

    if(!PGE_ImageInfo::getImageSize("../res/test1.jpg", &w, &h, &err))
        fprintf(stderr, "JPEG (1) failed: %d\n", err);
    else
        fprintf(stderr, "JPEG (1): %u x %u\n", w, h);

    if(!PGE_ImageInfo::getImageSize("../res/test2.jpg", &w, &h, &err))
        fprintf(stderr, "JPEG (2) failed: %d\n", err);
    else
        fprintf(stderr, "JPEG (2): %u x %u\n", w, h);

    if(!PGE_ImageInfo::getImageSize("../res/test3.jpg", &w, &h, &err))
        fprintf(stderr, "JPEG (3) failed: %d\n", err);
    else
        fprintf(stderr, "JPEG (3): %u x %u\n", w, h);

    if(!PGE_ImageInfo::getImageSize("../res/test4.jpg", &w, &h, &err))
        fprintf(stderr, "JPEG (4) failed: %d\n", err);
    else
        fprintf(stderr, "JPEG (4): %u x %u\n", w, h);

    return 0;
}
