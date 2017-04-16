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
    if(data.idirect == LevelDoor::ENTRANCE_LEFT || data.idirect == LevelDoor::ENTRANCE_RIGHT)
    {
        m_momentum.w = data.length_i;
        m_momentum.h = 32;
    }
    else
    {
        m_momentum.w = 32;
        m_momentum.h = data.length_i;
    }

    m_momentum.x = data.ix;
    m_momentum.y = data.iy;

    if(!m_treemap.m_is_registered)
        m_treemap.addToScene();
    else
        m_treemap.updatePosAndSize();

    m_blocked[1] = Block_NONE;
    m_blocked[2] = Block_NONE;
    m_scene->m_layers.registerItem(data.layer, this);
    m_momentum_relative.saveOld();
    m_momentum.saveOld();
}
