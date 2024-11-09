#pragma once

#include <QString>

struct FIBITMAP;
class GraphicsLoad
{
public:
    // ensures that FreeImage is inited while alive
    struct FreeImage_Sentinel
    {
        FreeImage_Sentinel();
        ~FreeImage_Sentinel();
    };

    static FIBITMAP* loadImage(const QString& path);

    static void RGBAToMask(FIBITMAP* mask);

    static void mergeWithMask(FIBITMAP *image, FIBITMAP *mask);

    static FIBITMAP *fast2xScaleDown(FIBITMAP *image);

    static bool validateBitmaskRequired(FIBITMAP *image, FIBITMAP *mask);
};
