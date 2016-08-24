/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "lvl_warp.h"
#include "../scene_level.h"

LVL_Warp::LVL_Warp(LevelScene *_parent) : PGE_Phys_Object(_parent)
{
     type = LVLWarp;
     m_bodytype = Body_STATIC;
}

LVL_Warp::~LVL_Warp()
{}

void LVL_Warp::init()
{
    if(data.idirect==LevelDoor::ENTRANCE_LEFT||data.idirect==LevelDoor::ENTRANCE_RIGHT)
        setSize(data.length_i, 32);
    else
        setSize(32, data.length_i);

    setPos(data.ix, data.iy);
    m_blocked[1] = Block_NONE;
    m_blocked[2] = Block_NONE;
    _scene->layers.registerItem(data.layer, this);
    m_momentum.saveOld();
}
