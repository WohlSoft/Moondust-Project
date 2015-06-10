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

#include "number_limiter.h"

void NumberLimiter::apply(short &value, short min, short max)
{
    if(value<min)
        value=min;
    else if(value>max)
        value=max;
}

void NumberLimiter::applyD(short &value, short defvalue, short min, short max)
{
    if((value<min)||(value>max))
    {
        value=defvalue;
        apply(value, min, max);
    }
}

void NumberLimiter::apply(unsigned short &value, unsigned short min, unsigned short max)
{
    if(value<min)
        value=min;
    else if(value>max)
        value=max;
}

void NumberLimiter::applyD(unsigned short &value, unsigned short defvalue, unsigned short min, unsigned short max)
{
    if((value<min)||(value>max))
    {
        value=defvalue;
        apply(value, min, max);
    }
}

void NumberLimiter::apply(int &value, int min, int max)
{
    if(value<min)
        value=min;
    else if(value>max)
        value=max;
}

void NumberLimiter::applyD(int &value, int defvalue, int min, int max)
{
    if((value<min)||(value>max))
    {
        value=defvalue;
        apply(value, min, max);
    }
}

void NumberLimiter::apply(unsigned int &value, unsigned int min, unsigned int max)
{
    if(value<min)
        value=min;
    else if(value>max)
        value=max;
}

void NumberLimiter::applyD(unsigned int &value, unsigned int defvalue, unsigned int min, unsigned int max)
{
    if((value<min)||(value>max))
    {
        value=defvalue;
        apply(value, min, max);
    }
}

void NumberLimiter::apply(long &value, long min, long max)
{
    if(value<min)
        value=min;
    else if(value>max)
        value=max;
}

void NumberLimiter::applyD(long &value, long defvalue, long min, long max)
{
    if((value<min)||(value>max))
    {
        value=defvalue;
        apply(value, min, max);
    }
}

void NumberLimiter::apply(unsigned long &value, unsigned long min, unsigned long max)
{
    if(value<min)
        value=min;
    else if(value>max)
        value=max;
}

void NumberLimiter::applyD(unsigned long &value, unsigned long defvalue, unsigned long min, unsigned long max)
{
    if((value<min)||(value>max))
    {
        value=defvalue;
        apply(value, min, max);
    }
}

void NumberLimiter::apply(float &value, float min, float max)
{
    if(value<min)
        value=min;
    else if(value>max)
        value=max;
}

void NumberLimiter::applyD(float &value, float defvalue, float min, float max)
{
    if((value<min)||(value>max))
    {
        value=defvalue;
        apply(value, min, max);
    }
}

void NumberLimiter::apply(double &value, double min, double max)
{
    if(value<min)
        value=min;
    else if(value>max)
        value=max;
}

void NumberLimiter::applyD(double &value, double defvalue, double min, double max)
{
    if((value<min)||(value>max))
    {
        value=defvalue;
        apply(value, min, max);
    }
}
