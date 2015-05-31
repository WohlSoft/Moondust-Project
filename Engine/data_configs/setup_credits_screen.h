#ifndef SETUP_CREDITS_SCREEN_H
#define SETUP_CREDITS_SCREEN_H

#include <QString>
#include <QColor>
class QSettings;
#include <QList>

struct CreditsScreenAdditionalImage
{
    QString imgFile;
    bool animated;
    int frames;
    int x;
    int y;
};

struct CreditsScreenSetup
{
    void init(QSettings &engine_ini);
    QString backgroundImg;
    QColor backgroundColor;
    int updateDelay;
    QList<CreditsScreenAdditionalImage > AdditionalImages;
    QString luaFile;
};


#endif // SETUP_CREDITS_SCREEN_H

