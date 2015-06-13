/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef NPC_ANIMATOR_H
#define NPC_ANIMATOR_H

#include <QList>
#include <QString>

#include <SDL2/SDL_timer.h>
#include <utility>

#include <data_configs/obj_npc.h>

typedef std::pair<double, double > AniPos;

class AdvNpcAnimator
{
public:
    AdvNpcAnimator();
    AdvNpcAnimator(PGE_Texture &sprite, obj_npc &config);
    void construct(PGE_Texture &sprite, obj_npc &config);
    ~AdvNpcAnimator();

    AniPos image(int dir, int frame=-1);
    AniPos wholeImage();

    void setFrameL(int y);
    void setFrameR(int y);

    void start();
    void stop();

    void manualTick(int ticks);

private:
    bool isValid;
    void nextFrame();
    obj_npc setup;

    PGE_Texture mainImage;

    QList<AniPos> frames;

    int manual_ticks;
    bool onceMode;
    bool animationFinished;

    bool isEnabled;
    SDL_TimerID timer_id;

    void createAnimationFrames();

    bool animated;

    int frameSpeed;
    int frameStyle;

    bool aniBiDirect;
    int frameStep;

    bool customAnimate;
    int customAniAlg; //custom animation algorythm 0 - forward, 1 - frameJump
    int custom_frameFL;//first left
    int custom_frameEL;//end left / jump step
    int custom_frameFR;//first right
    int custom_frameER;//enf right / jump step

    bool frameSequance;

    QList<int> frames_listL;     //Current frame srquence
    QList<int> frames_listR;     //Current frame srquence

    int framesQ;
    double frameSize; // size of one frame
    double frameWidth; // sprite width
    double frameHeight; //sprite height

    //Animation alhorithm
    bool aniDirectL;
    bool aniDirectR;

    int CurrentFrameL;
    int CurrentFrameR;

    int frameCurrentL;
    int frameCurrentR;

    int frameFirstL;
    int frameLastL;

    int frameFirstR;
    int frameLastR;

};

#endif // NPC_ANIMATOR_H
