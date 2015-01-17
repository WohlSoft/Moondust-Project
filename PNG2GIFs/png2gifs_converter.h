#ifndef P2G_CONVERTER_H
#define P2G_CONVERTER_H

#include <QImage>

extern bool removeSource;

QImage  setAlphaMask(QImage image, QImage mask);
bool    toGif(QImage& img, QString& path);
QImage  loadQImage(QString file);
void    doMagicIn(QString path, QString q, QString OPath);

class P2G_Converter
{
public:
    P2G_Converter();
    ~P2G_Converter();
};

#endif // P2G_CONVERTER_H
