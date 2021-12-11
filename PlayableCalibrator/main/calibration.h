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
    bool isMountRiding = false;

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
            showGrabItem == o.showGrabItem &&
            isMountRiding == o.isMountRiding
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
};

inline bool operator==(const AniFrameSet &o1, const AniFrameSet &o2)
{
    return o1.name == o2.name &&
           o1.L == o2.L &&
           o1.R == o2.R;
}


#define FRAME_WIDTH 100
#define FRAME_HEIGHT 100

struct Calibration
{
    int matrixWidth = 10;
    int matrixHeight = 10;
    int spriteWidth = 1000;
    int spriteHeight = 1000;
    int cellWidth = 100;
    int cellHeight = 100;
    int frameWidth = 0;
    int frameHeight = 0;
    int frameHeightDuck = -1;
    int grabOffsetX = 0;
    int grabOffsetY = 0;
    bool grabOverTop = false;

    enum CalibrationCompat
    {
        COMPAT_UNSPECIFIED = 0,
        // Shorter config for Moondust
        COMPAT_MOONDUST,
        // Shorter config for TheXTech
        COMPAT_THEXTECH,
        // Shorter config for SMBX2
        COMPAT_SMBX2,
        // SMBX-38A compatibility
        COMPAT_SMBX38A,
        // Export the full config with all data
        COMPAT_CALIBRATOR_FULL
    };
    int compatProfile = COMPAT_UNSPECIFIED;

    typedef QPair<int /*x*/, int /*y*/> FramePos;
    typedef QMap<FramePos, CalibrationFrame> FramesSet;
    typedef QMap<QString, AniFrameSet> AnimationSet;

    FramesSet frames;
    AnimationSet animations;
};

#endif // CALIBRATION_H
