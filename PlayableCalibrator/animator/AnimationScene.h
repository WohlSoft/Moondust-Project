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

#ifndef ANIMATIONSCENE_H
#define ANIMATIONSCENE_H

#include <QGraphicsScene>
#include <QtWidgets>
#include "aniFrames.h"
#include "calibrationmain.h"

class AnimationScene : public QGraphicsScene
{
    Q_OBJECT
public:
    AnimationScene(QObject *parent = 0);
    QPainter test;
    int mCurrentFrameX;
    int mCurrentFrameY;
    int mCurFrN;

    FrameSets FullFrames;

    QVector<AniFrame > currentFrameSet;

    QList<QVector<frameOpts > >  framesTable;

    void draw( );
    QPoint pos() const;
    void setAnimation(QVector<AniFrame> frameS);
    void setFrame(int frame);
    void setSquare(int x, int y, int h, int w);

    QPixmap mSpriteImage;
    QPixmap NoAnimate;

    QTimer timer;

public slots:
    void nextFrame();

private:
    QGraphicsPixmapItem ImageFrame;
    QGraphicsRectItem GRound;
    QPoint mPos;
    int mXDir;
    int x, y, dir;
    QPixmap currentImage;
};

#endif // ANIMATIONSCENE_H
