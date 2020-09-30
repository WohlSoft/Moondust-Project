#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <QPair>
#include <QMap>
#include <QHash>
#include <QString>

#include "animator/aniFrames.h"

struct CalibrationFrame
{
    unsigned int H = 100;
    unsigned int W = 100;
    int offsetX = 0;
    int offsetY = 0;
    bool used = false;
    bool isDuck;
    bool isRightDir;
    bool showGrabItem;
};

#define FRAME_WIDTH 100
#define FRAME_HEIGHT 100

struct Calibration
{
    int frameWidth = -1;
    int frameHeight = -1;
    int frameHeightDuck = -1;
    int frameGrabOffsetX = 0;
    int frameGrabOffsetY = 0;
    bool frameOverTopGrab = false;

    typedef QPair<int /*x*/, int /*y*/> FramePos;
    typedef QMap<FramePos, CalibrationFrame> FramesSet;
    typedef QMap<QString, AniFrameSet> AnimationSet;

    FramesSet frames;
    AnimationSet animations;
};

#endif // CALIBRATION_H
