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
#include <limits>

class NumberLimiter
{
public:
    template<typename T>
    static void apply(T &value, T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max()){
        static_assert(std::is_arithmetic<T>::value, "The value for \"apply\" must be arithemtic");
        if(value<min)
            value=min;
        else if(value>max)
            value=max;
    }

    template<typename T>
    static void applyD(T &value, T defvalue, T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max()){
        static_assert(std::is_arithmetic<T>::value, "The value for \"applyD\" must be arithemtic");
        if((value<min)||(value>max))
        {
            value=defvalue;
            apply(value, min, max);
        }
    }


};

#endif // NUMBER_LIMITER_H
