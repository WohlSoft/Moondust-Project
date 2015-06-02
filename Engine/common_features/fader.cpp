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

#include "fader.h"
#include <stdlib.h>
#include <math.h>

PGE_Fader::PGE_Fader()
{
    cur_ratio=0.0;
    target_ratio=0.0;
    fade_step=0.0;
    fadeSpeed=0;
    manual_ticks=0;
}

PGE_Fader::PGE_Fader(const PGE_Fader &f)
{
    cur_ratio      =f.cur_ratio;
    target_ratio   =f.target_ratio;
    fade_step      =f.fade_step;
    fadeSpeed      =f.fadeSpeed;
    manual_ticks   =f.manual_ticks;
}

PGE_Fader::~PGE_Fader()
{}

void PGE_Fader::setFade(int speed, float target, float step)
{
    fade_step = fabs(step);
    target_ratio = target;
    fadeSpeed = speed;
    manual_ticks = speed;
}

void PGE_Fader::setRatio(float ratio)
{
    cur_ratio=ratio;
    target_ratio=ratio;
}

bool PGE_Fader::tickFader(float ticks)
{
    if(fadeSpeed<1) return true; //Idling animation
    if(cur_ratio==target_ratio)
        return true;

    manual_ticks-=fabs(ticks);
        while(manual_ticks<=0.0f)
        {
            fadeStep();
            manual_ticks+=fadeSpeed;
        }
        return (cur_ratio==target_ratio);
}

const float &PGE_Fader::fadeRatio()
{
    return cur_ratio;
}

bool PGE_Fader::isFading()
{
    return (cur_ratio!=target_ratio);
}

bool PGE_Fader::isFull()
{
    return ((cur_ratio>=1.0f) && (!isFading()));
}

void PGE_Fader::setFull()
{
    cur_ratio=1.0f;
    target_ratio=1.0f;
}

bool PGE_Fader::isNull()
{
    return ((cur_ratio<=0.0f) && (!isFading()));
}

void PGE_Fader::setNull()
{
    cur_ratio    =0.0f;
    target_ratio =0.0f;
}

int PGE_Fader::ticksLeft()
{
    return manual_ticks;
}

void PGE_Fader::fadeStep()
{
    if(cur_ratio==target_ratio)
        return;
    if(cur_ratio < target_ratio)
        cur_ratio+=fade_step;
    else
        cur_ratio-=fade_step;

    if(cur_ratio>1.0f) cur_ratio = 1.0f;
    else
        if(cur_ratio<0.0f) cur_ratio = 0.0f;
}
