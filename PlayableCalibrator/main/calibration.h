#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <QPair>
#include <QMap>
#include <QHash>
#include <QVector>
#include <QString>

struct CalibrationFrame
{
    unsigned int h = 100;
    unsigned int w = 100;
    int offsetX = 0;
    int offsetY = 0;
    bool used = false;
    bool isDuck = false;
    bool isRightDir = false;
    bool showGrabItem = false;

    bool operator==(const CalibrationFrame &o)
    {
        return
            h == o.h &&
            w == o.w &&
            offsetX == o.offsetX &&
            offsetY == o.offsetY &&
            used == o.used &&
            isDuck == o.isDuck &&
            isRightDir == o.isRightDir &&
            showGrabItem == o.showGrabItem
        ;
    }
};

struct AniFrame
{
    int x = 0;
    int y = 0;
};

inline bool operator==(const AniFrame &o1, const AniFrame &o2)
{
    return o1.x == o2.x &&
           o1.y == o2.y;
}

struct AniFrameSet
{
    QVector<AniFrame > L;
    QVector<AniFrame > R;
    QString name;

    bool operator==(const AniFrameSet &o)
    {
        return name == o.name &&
               L == o.L &&
               R == o.R;
    }
};

#define FRAME_WIDTH 100
#define FRAME_HEIGHT 100

struct Calibration
{
    int frameWidth = 0;
    int frameHeight = 0;
    int frameHeightDuck = -1;
    int grabOffsetX = 0;
    int grabOffsetY = 0;
    bool grabOverTop = false;

    typedef QPair<int /*x*/, int /*y*/> FramePos;
    typedef QMap<FramePos, CalibrationFrame> FramesSet;
    typedef QMap<QString, AniFrameSet> AnimationSet;

    FramesSet frames;
    AnimationSet animations;
};

#endif // CALIBRATION_H
