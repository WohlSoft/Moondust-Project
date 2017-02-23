/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef CONFIG_WLD_GENERIC_H
#define CONFIG_WLD_GENERIC_H

#include <PGEString.h>
#include <stdint.h>

class IniProcessing;

struct WldGenericSetup
{
    bool parse(IniProcessing* setup,
               PGEString imgPath,
               uint32_t defaultGrid,
               WldGenericSetup* merge_with = nullptr,
               PGEString *error = nullptr);

    uint64_t    id = 0;
    PGEString   image_n;
    PGEString   mask_n;

    uint32_t    grid = 0;
    PGEString   group    = "_NoGroup";
    PGEString   category = "_Other";
    bool        animated = false;
    uint32_t    frames = 0;
    uint32_t    framespeed = 0; // Default 128 ms
    uint32_t    display_frame = 0;
    uint32_t    frame_h = 0; //Hegth of the frame. Calculating automatically

    bool                map3d_vertical = false;
    PGEList<uint32_t>   map3d_stackables;

    /**Used only by terrain and path tiles **/
    uint32_t row = 0;
    uint32_t col = 0;
    /****************************************/
};

#endif // CONFIG_WLD_GENERIC_H
