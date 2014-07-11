#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "../libs/EasyBMP/EasyBMP.h"
extern "C"
{
    #include "../libs/giflib/gif_lib.h"
}

#include <QFile>
#include <QImage>

class Graphics
{
public:
    static QImage setAlphaMask(QImage image, QImage mask);
    static bool toGif(QImage& img, QString& path);
    static QImage fromBMP(QString &file);
    static QImage loadQImage(QString file);
};

#endif // GRAPHICS_H
