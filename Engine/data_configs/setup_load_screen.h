#ifndef SETUP_LOAD_SCREEN_H
#define SETUP_LOAD_SCREEN_H

#include <QString>
#include <QColor>
class QSettings;
#include <QList>

struct LoadingScreenAdditionalImage
{
    QString imgFile;
    bool animated;
    int frames;
    int x;
    int y;
};

struct LoadingScreenSetup
{
    void init(QSettings &engine_ini);
    QString backgroundImg;
    QColor backgroundColor;
    int updateDelay;
    QList<LoadingScreenAdditionalImage > AdditionalImages;
};

#endif // SETUP_LOAD_SCREEN_H

