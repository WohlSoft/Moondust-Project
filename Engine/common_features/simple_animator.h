/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#ifndef SIMPLE_ANIMATOR_H
#define SIMPLE_ANIMATOR_H

#include <SDL2/SDL_timer.h>
#include <vector>
#include <utility>

typedef std::pair<double, double > AniPos;

class SimpleAnimator
{
    public:
        SimpleAnimator();
        SimpleAnimator(const SimpleAnimator &animator);
        SimpleAnimator(bool enables, int framesq = 1, int fspeed = 64, int First = 0, int Last = -1,
                       bool rev = false, bool bid = false);
        ~SimpleAnimator();

        bool operator!=(const SimpleAnimator &animator) const;
        bool operator==(const SimpleAnimator &animator) const;
        SimpleAnimator &operator=(const SimpleAnimator &animator) = default;

        void construct(bool enables = false, int framesq = 1, int fspeed = 64, int First = 0, int Last = -1,
                       bool rev = false, bool bid = false);

        void setFrameSequance(std::vector<int> sequance);

        AniPos image(double frame = -1);

        void setFrame(int y);
        void setFrames(int first, int last);

        void start();
        void stop();

        double m_frameDelay;

        static unsigned int TickAnimation(unsigned int x, void *p);

        void setOnceMode(bool once, int loops = 1);
        void manualTick(double ticks);
        bool isFinished();
    public:
        void nextFrame();

    private:
        double m_pos1;
        double m_pos2;
        double m_nextFrameTime;
        bool m_onceMode;
        int  m_onceMode_loops;
        bool m_animationFinished;

        bool                m_frameSequanceEnabled;
        std::vector<int>    m_frameSequance;
        int                 m_frameSequanceCur;

        int m_currentFrame;

        bool m_animated;

        bool m_bidirectional;
        bool m_reverce;

        bool m_isEnabled;
        SDL_TimerID m_timerId;

        double m_framesCount;

        //Animation alhorithm
        int m_frameFirst;
        int m_frameLast;
};

#endif // SIMPLE_ANIMATOR_H
