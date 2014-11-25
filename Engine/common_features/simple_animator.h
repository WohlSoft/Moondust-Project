/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
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

#ifndef SIMPLE_ANIMATOR_H
#define SIMPLE_ANIMATOR_H

#include <SDL2/SDL_timer.h>
#include <utility>

typedef std::pair<double, double > AniPos;

class SimpleAnimator
{
public:
    SimpleAnimator();
    SimpleAnimator(const SimpleAnimator &animator);
    SimpleAnimator(bool enables, int framesq=1, int fspeed=64, int First=0, int Last=-1,
                    bool rev=false, bool bid=false);
    ~SimpleAnimator();

    bool operator!=(const SimpleAnimator &animator) const;
    bool operator==(const SimpleAnimator &animator) const;
    SimpleAnimator &operator=(const SimpleAnimator &animator);

    void construct(bool enables=false, int framesq=1, int fspeed=64, int First=0, int Last=-1,
                   bool rev=false, bool bid=false);

    AniPos image(double frame=-1);

    void setFrame(int y);
    void setFrames(int first, int last);

    void start();
    void stop();

    int speed;

    static unsigned int TickAnimation(unsigned int x, void *p);

public:
    void nextFrame();

private:
    double pos1;
    double pos2;

    int CurrentFrame;

    bool animated;

    bool bidirectional;
    bool reverce;

    int frameCurrent;

    bool isEnabled;
    SDL_TimerID timer_id;

    double framesQ;
    int frameSize; // size of one frame
    int frameWidth; // sprite width
    int frameHeight; //sprite height

    //Animation alhorithm
    int frameFirst;
    int frameLast;


};

#endif // SIMPLE_ANIMATOR_H
