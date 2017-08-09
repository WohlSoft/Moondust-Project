/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <vector>
#include <string>

#include <SDL2/SDL_timer.h>
#include <utility>

#include <common_features/sizef.h>
#include <data_configs/obj_npc.h>

typedef std::pair<double, double > AniPos;

class AdvNpcAnimator
{
    public:
        AdvNpcAnimator();
        AdvNpcAnimator(PGE_Texture &sprite, obj_npc &config);
        void construct(PGE_Texture &sprite, obj_npc &config);
        ~AdvNpcAnimator();

        AniPos image(int dir, int frame = -1);
        AniPos wholeImage();

        enum CustomAnimationAlgorithms
        {
            ANI_DefaultSequence = 0,
            ANI_FrameJump = 1,
            ANI_CustomSequence = 2
        };

        void setSequenceL(std::vector<int> _frames);
        void setSequenceR(std::vector<int> _frames);
        void setSequence(std::vector<int> _frames);

        void setFrameL(int y);
        void setFrameR(int y);

        int  frameSpeed();
        void setFrameSpeed(int ms);
        void setBidirectional(bool bid);
        void setDirectedSequence(bool dd);
        void setOnceMode(bool en);
        bool animationFinished();
        PGE_SizeF sizeOfFrame();

        void start();
        void stop();

        void manualTick(double ticks);

    private:
        bool m_isValid;
        void nextFrame();
        obj_npc m_setup;

        PGE_Texture m_mainImage;

        std::vector<AniPos> m_frames;

        double m_ticks;
        bool m_onceMode;
        bool m_animationFinished;

        bool m_isEnabled;

        void createAnimationFrames();

        bool m_animated;

        double m_frameDelay;
        int m_frameStyle;

        bool m_aniBiDirect;
        int m_frameStep;

        bool m_customAnimate;
        int m_customAniAlg; //custom animation algorythm 0 - forward, 1 - frameJump
        int m_custom_frameFL;//first left
        int m_custom_frameEL;//end left / jump step
        int m_custom_frameFR;//first right
        int m_custom_frameER;//enf right / jump step

        bool m_frameSequance;

        int getFrameNumL(int num);
        int getFrameNumR(int num);

        std::vector<int> s_framesL;     //Current frame srquence
        std::vector<int> s_framesR;     //Current frame srquence

        int m_framesCount;
        double m_frameHeight; // size of one frame
        double m_frameWidth; // sprite width
        double m_spriteHeight; //sprite height

        //Animation alhorithm
        bool m_aniDirectL;
        bool m_aniDirectR;

        int m_curFrameL_real;
        int m_curFrameR_real;

        int m_curFrameL_timer;
        int m_curFrameR_timer;

        int m_frameFirstL;
        int m_frameLastL;

        int m_frameFirstR;
        int m_frameLastR;
};

#endif // NPC_ANIMATOR_H
