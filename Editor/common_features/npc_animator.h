/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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
#ifndef NPC_ANIMATOR_H
#define NPC_ANIMATOR_H

#include <QObject>
#include <QTimer>
#include <QPixmap>
#include <QVector>
#include <QList>
#include <QString>

#include <data_configs/obj_npc.h>

class AdvNpcAnimator : public QObject
{
    Q_OBJECT
public:
    AdvNpcAnimator();
    AdvNpcAnimator(QPixmap &sprite, obj_npc &config);
    ~AdvNpcAnimator();

    void buildAnimator(QPixmap &sprite, obj_npc &config);

    QPixmap image(int dir, int frame=-1);
    QPixmap &wholeImage();

    QRect  &frameRect(int dir);

    void setFrameL(int y);
    void setFrameR(int y);

    void start();
    void stop();

signals:
    void onFrame();

private slots:
    void nextFrame();

private:
    obj_npc m_setup;

    QPixmap *m_mainImage;

    //QVector<QPixmap> frames; //Whole image
    //void createAnimationFrames();

    bool m_animated;

    int m_frameSpeed;
    int m_frameStyle;

    bool m_aniBiDirect;

    int m_curDirect;
    int m_frameStep;

    bool m_customAnimate;

    bool m_frameSequance;

    int getFrameNumL(int num);
    int getFrameNumR(int num);

    QList<int> m_framesListL;     //Current frame srquence
    QList<int> m_framesListR;     //Current frame srquence

    int m_frameCurrent;
    QTimer *m_timer;

    int m_framesCountOneSide;
    int m_framesCountTotal;
    int m_frameHeight; // size of one frame
    int m_frameWidth; // sprite width
    int m_spriteHeight; //sprite height

    bool m_aniDirectL;
    bool m_aniDirectR;

    int m_frameCurrentRealL;
    int m_frameCurrentRealR;

    int m_frameCurrentTimerL;
    int m_frameCurrentTimerR;

    QRect m_frameRectL;
    QRect m_frameRectR;

    int m_frameFirstL;
    int m_frameLastL;

    int m_frameFirstR;
    int m_frameLastR;

};

#endif // NPC_ANIMATOR_H
