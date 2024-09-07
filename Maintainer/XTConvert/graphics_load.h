#pragma once

#include <QString>

struct FIBITMAP;
class GraphicsLoad
{
public:
    /*!
     * \brief Initializes FreeImage
     */
    static void  initFreeImage();

    /*!
     * \brief DeInitializes FreeImage
     */
    static void  closeFreeImage();

    static FIBITMAP* loadImage(const QString& path);

    static FIBITMAP* loadPNGAsMask(const QString& path);

    static void mergeWithMask(FIBITMAP *image, FIBITMAP *mask);

    static FIBITMAP *fast2xScaleDown(FIBITMAP *image);

    static bool validateBitmaskRequired(FIBITMAP *image, FIBITMAP *mask);
};
