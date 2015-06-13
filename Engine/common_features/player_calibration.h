/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2015 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PLAYER_CALIBRATION_H
#define PLAYER_CALIBRATION_H

class QSettings;
#include <QString>
#include <QList>

struct AniFrame
{
    int x;
    int y;
};

struct AniFrameSet
{
    QList<AniFrame > L;
    QList<AniFrame > R;
    QString name;
};

struct FrameSets
{
    QList<AniFrameSet > set;
};

struct frameOpts
{
    unsigned int H;
    unsigned int W;
    int offsetX;
    int offsetY;
    bool used;
    bool isDuck;
    bool isRightDir;
    bool showGrabItem;
};

struct obj_player_calibration
{
    int frameWidth;
    int frameHeight;
    int frameHeightDuck;
    int frameGrabOffsetX;
    int frameGrabOffsetY;
    int frameOverTopGrab;
    QList<QList<frameOpts > > framesX; //!< Collision boxes settings
    FrameSets AniFrames;               //!< Animation settings
    void init(int x, int y);
    bool load(QString fileName);
private:
    void getSpriteAniData(QSettings &set, QString name);
};

#endif // PLAYER_CALIBRATION_H

