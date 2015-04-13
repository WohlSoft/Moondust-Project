#ifndef SETUP_TITLE_SCREEN_H
#define SETUP_TITLE_SCREEN_H

#include <QString>
#include <QColor>
#include <QList>
class QSettings;

struct TitleScreenAdditionalImage
{
    enum align{
        NO_ALIGN=0,
        LEFT_ALIGN,
        TOP_ALIGN,
        RIGHT_ALIGN,
        BOTTOM_ALIGN,
        CENTER_ALIGN
    };

    QString imgFile;
    bool animated;
    int frames;
    unsigned int framespeed;
    align align_to;
    int x;
    int y;
    bool center_x;
    bool center_y;
};

struct TitleScreenSetup
{
    void init(QSettings &engine_ini);
    QString backgroundImg;
    QColor backgroundColor;
    QList<TitleScreenAdditionalImage > AdditionalImages;
};

#endif // SETUP_TITLE_SCREEN_H

