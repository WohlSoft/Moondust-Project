/*
 * Playble Character Calibrator, a free tool for playable character sprite setup tune
 * This is a part of the Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#pragma once
#ifndef ANIMATIONSCENE_H
#define ANIMATIONSCENE_H

#include <QGraphicsScene>
#include <QtWidgets>
#include "calibration_main.h"
#include "main/calibration.h"

class AnimationScene : public QGraphicsScene
{
    Q_OBJECT

    Calibration *m_conf = nullptr;

public:
    AnimationScene(Calibration &conf, QObject *parent = nullptr);

    void draw();
    QPoint pos() const;
    void setAnimation(QVector<AniFrame> frameS);
    void setFrame(int frame);

    void setFrameInterval(int interval);

private slots:
    void nextFrame();

private:
    //! Full set of animations
    Calibration::AnimationSet m_allAnimations;
    //! Currently installed animation
    QVector<AniFrame > m_currentAnimation;
    //! Table of frames
    Calibration::FramesSet m_framesTable;
    //! Animation timer
    QTimer m_timer;
    //! Full-sprite image
    QPixmap m_spriteImage;
    //! "No animation" message
    QPixmap m_noAnimate;
    //! Current frame image object on scene
    QGraphicsPixmapItem m_imageFrame;
    //! An abstract ground under feet of playable character on scene
    QGraphicsRectItem m_ground;
    //! Image that contains a current frame
    QPixmap m_currentImage;
    //! Current frame's X cell on a sprite
    int m_currentFrameX = 0;
    //! Current frame's Y cell on a sprite
    int m_currentFrameY = 0;
    //! Current frame cell X/Y position
    QPoint m_pos = QPoint(0, 0);
    //! Current frame index
    int m_curFrameIdx = 0;
};

#endif // ANIMATIONSCENE_H
