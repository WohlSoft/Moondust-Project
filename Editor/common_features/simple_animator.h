/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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
#ifndef SIMPLE_ANIMATOR_H
#define SIMPLE_ANIMATOR_H

#include <QObject>
#include <QTimer>
#include <QPixmap>
#include <QMutex>
#include <QVector>
#include <QMutexLocker>

#include "animation_timer.h"

class SimpleAnimator : public QObject, public TimedAnimation
{
    Q_OBJECT
public:
    SimpleAnimator(QObject *parent=0);
    SimpleAnimator(const SimpleAnimator &a, QObject *parent=0);
    SimpleAnimator &operator=(const SimpleAnimator& a);
    explicit SimpleAnimator(QPixmap &sprite, bool enables=false, int framesq=1, int fspeed=64, int First=0, int Last=-1,
                    bool rev=false, bool bid=false, QObject *parent=0);
    ~SimpleAnimator();
    QPixmap image(int frame=-1);
    QRect &frameRect();
    QRectF frameRectF();
    QPixmap &wholeImage();
    void setSettings(QPixmap &sprite, bool enables=false, int framesq=1, int fspeed=64, int First=0, int Last=-1,
                   bool rev=false, bool bid=false);

    void setFrameSequance(QList<int> sequance);

    /**
     * @brief Set current frame number
     * @param y frame number to set
     */
    void setFrame(int fnum);

    /**
     * @brief Returns current frame number
     * @return Current frame number
     */
    int  frame();

    void start();
    void stop();

    void resetFrame();
    void nextFrame();

private:
    QMutex  m_mutex;

    //! Animation frame delay
    int m_frameDelay;

    QPixmap *m_texture; //Whole image
    //QVector<QPixmap> frames; //Whole image

    bool       m_frame_sequance_enabled;
    QList<int> m_frame_sequance;
    int        m_frame_sequance_cur;

    QRect      m_frame_rect;

    //! Current frame index
    int m_currentFrame;

    //! Is animation enabled
    bool m_animated;

    //! Is anination is bidirectional
    bool m_bidirectional;
    //! Is reversive animation
    bool m_reverce;

    //! Frame top and bottom points position on the texture
    QPoint m_framePos;
    //! Number of pre-defined frames
    int m_framesCount;
    //! Height of one frame
    int m_frameHeight;
    //! Width of one frame
    int m_frameWidth; // sprite width
    //! Height of entire sprite
    int m_spriteHeight; //sprite height

    //Animation alhorithm
    int m_frameFirst;
    int m_frameLast;

};

#endif // SIMPLE_ANIMATOR_H
