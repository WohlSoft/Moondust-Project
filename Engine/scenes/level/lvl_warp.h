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

#ifndef LVL_WARP_H
#define LVL_WARP_H

#include "../../physics/base_object.h"
#include "../../data_configs/obj_bgo.h"

#include <PGE_File_Formats/file_formats.h>

class LVL_Warp : public PGE_Phys_Object
{
public:
    LVL_Warp();
    ~LVL_Warp();
    void init();

    LevelDoors data; //Local settings
};

#endif // LVL_WARP_H
