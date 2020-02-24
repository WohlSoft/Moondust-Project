/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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
    static void apply(T &value, T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max())
    {
        static_assert(std::is_arithmetic<T>::value, "The value for \"apply\" must be arithemtic");
        if(value < min)
            value = min;
        else if(value > max)
            value = max;
    }

    template<typename T>
    static void applyD(T &value, T defvalue, T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max())
    {
        static_assert(std::is_arithmetic<T>::value, "The value for \"applyD\" must be arithemtic");
        if((value < min) || (value > max))
        {
            value = defvalue;
            apply(value, min, max);
        }
    }


};

#endif // NUMBER_LIMITER_H
