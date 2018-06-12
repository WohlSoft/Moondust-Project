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

#ifndef CONFIG_BGO_H
#define CONFIG_BGO_H

#include <PGEString.h>
#include <stdint.h>

class IniProcessing;

struct BgoSetup
{
    bool parse(IniProcessing* setup,
               PGEString bgoImgPath,
               uint32_t defaultGrid,
               BgoSetup* merge_with = nullptr,
               PGEString *error = nullptr);

    uint64_t    id = 0;
    PGEString   name;
    PGEString   group = "_NoGroup";
    PGEString   category = "_Other";
    PGEString   description = "";
    uint32_t    grid = 32;

    enum ZLayerEnum
    {
        z_background_2 =-1,
        z_background_1 = 0,
        z_foreground_1 = 1,//Default
        z_foreground_2 = 2
    };
    int32_t     zLayer = 0; //-1, 0, 1, 2
    long double zOffset = 0.0l;
    bool        zValueOverride = false;
    long double zValue = 0.0l;
    int32_t     offsetX = 0;
    int32_t     offsetY = 0;

    PGEString   image_n;
    PGEString   mask_n;

    //! (Optional) In-editor icon for item boxes
    PGEString   icon_n;

    uint32_t    frame_h = 0;

    bool        climbing = false;
    bool        animated = false;
    uint32_t    frames = 1;
    uint32_t    framespeed = 128;

    uint32_t    display_frame = 0;

    //! Custom animation sequence
    PGEList<int32_t> frame_sequence;
};

#endif // CONFIG_BGO_H
