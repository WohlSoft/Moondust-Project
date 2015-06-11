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
#include <type_traits>


class NumberLimiter
{
public:

    template<typename T>
    static T minVal()
    {
        if(std::is_same<T, char>::value)
            return CHAR_MIN;
        else if(std::is_same<T, unsigned char>::value)
            return 0;
        if(std::is_same<T, short>::value)
            return SHRT_MIN;
        else if(std::is_same<T, unsigned short>::value)
            return 0;
        if(std::is_same<T, int>::value)
            return INT_MIN;
        else if(std::is_same<T, unsigned int>::value)
            return 0;
        if(std::is_same<T, long>::value)
            return LONG_MIN;
        else if(std::is_same<T, unsigned long>::value)
            return 0;
        if(std::is_same<T, long long>::value)
            return LLONG_MIN;
        else if(std::is_same<T, unsigned long long>::value)
            return 0;
        else if(std::is_same<T, float>::value)
            return FLT_MIN;
        else if(std::is_same<T, double>::value)
            return DBL_MIN;
    }

    template<typename T>
    static T maxVal()
    {
        if(std::is_same<T, char>::value)
            return CHAR_MAX;
        else if(std::is_same<T, unsigned char>::value)
            return UCHAR_MAX;
        if(std::is_same<T, short>::value)
            return SHRT_MAX;
        else if(std::is_same<T, unsigned short>::value)
            return USHRT_MAX;
        if(std::is_same<T, int>::value)
            return INT_MAX;
        else if(std::is_same<T, unsigned int>::value)
            return UINT_MAX;
        if(std::is_same<T, long>::value)
            return LONG_MAX;
        else if(std::is_same<T, unsigned long>::value)
            return ULONG_MAX;
        if(std::is_same<T, long long>::value)
            return LLONG_MIN;
        else if(std::is_same<T, unsigned long long>::value)
            return ULLONG_MAX;
        else if(std::is_same<T, float>::value)
            return FLT_MAX;
        else if(std::is_same<T, double>::value)
            return DBL_MAX;
    }

    template<typename T>
    static void apply(T &value, T min = minVal<T>(), T max = maxVal<T>()){
        static_assert(std::is_arithmetic<T>::value, "The value for \"apply\" must be arithemtic");
        if(value<min)
            value=min;
        else if(value>max)
            value=max;
    }

    template<typename T>
    static void applyD(T &value, T min = minVal<T>(), T max = maxVal<T>()){
        static_assert(std::is_arithmetic<T>::value, "The value for \"applyD\" must be arithemtic");
        if((value<min)||(value>max))
        {
            value=defvalue;
            apply(value, min, max);
        }
    }


};

#endif // NUMBER_LIMITER_H
