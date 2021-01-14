/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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
