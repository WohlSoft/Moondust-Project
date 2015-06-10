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

#ifndef NUMBER_LIMITER_H
#define NUMBER_LIMITER_H

#include <climits>
#include <cfloat>

class NumberLimiter
{
public:
    static void apply(short &value, short min=SHRT_MIN, short max=SHRT_MAX);
    static void applyD(short &value, short defvalue, short min=SHRT_MIN, short max=SHRT_MAX);
    static void apply(unsigned short &value, unsigned short min=0, unsigned short max=USHRT_MAX);
    static void applyD(unsigned short &value, unsigned short defvalue, unsigned short min=0, unsigned short max=USHRT_MAX);
    static void apply(int &value, int min=INT_MIN, int max=INT_MAX);
    static void applyD(int &value, int defvalue, int min=INT_MIN, int max=INT_MAX);
    static void apply(unsigned int &value, unsigned int min=0, unsigned int max=UINT_MAX);
    static void applyD(unsigned int &value, unsigned int defvalue, unsigned int min=0, unsigned int max=UINT_MAX);
    static void apply(long &value, long min=LONG_MIN, long max=LONG_MAX);
    static void applyD(long &value, long defvalue, long min=LONG_MIN, long max=LONG_MAX);
    static void apply(unsigned long &value, unsigned long min=0, unsigned long max=ULONG_MAX);
    static void applyD(unsigned long &value, unsigned long defvalue, unsigned long min=0, unsigned long max=ULONG_MAX);
    static void apply(float &value, float min=FLT_MIN, float max=FLT_MAX);
    static void applyD(float &value, float defvalue, float min=FLT_MIN, float max=FLT_MAX);
    static void apply(double &value, double min=FLT_MIN, double max=FLT_MAX);
    static void applyD(double &value, double defvalue, double min=FLT_MIN, double max=FLT_MAX);
};

#endif // NUMBER_LIMITER_H
