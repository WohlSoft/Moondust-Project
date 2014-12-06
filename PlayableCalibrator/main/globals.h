/*
 * SMBX64 Playble Character Sprite Calibrator, a free tool for playable srite design
 * This is a part of the Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef GLOBALS_H
#define GLOBALS_H

#include <QList>
#include <QPair>
#include <QPixmap>
#include "animator/aniFrames.h"
#include "animator/AnimationScene.h"

class Globals
{
public:
    Globals();
};

extern QList<QVector<frameOpts > > imgOffsets; //!< Image offsets
extern QList<QVector<frameOpts > > framesX;    //!< Collision boxes settings
extern int frameWidth;
extern int frameHeight;
extern int frameHeightDuck;
extern int frameGrabOffsetX;
extern int frameGrabOffsetY;
extern FrameSets AniFrames;                    //!< Animation settings

extern frameOpts buffer;

extern QString LastOpenDir;
extern QString currentFile;
extern int CurFrame_X, CurFrame_Y;

#endif // GLOBALS_H
