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

#include "lvl_physenv.h"
#include "../scene_level.h"

const unsigned long LVL_PhysEnv::numOfEnvironments = 3;

LVL_PhysEnv::LVL_PhysEnv(LevelScene *_parent) : PGE_Phys_Object(_parent)
{
    type = LVLPhysEnv;
    env_type = Env_Water;
    m_bodytype = Body_STATIC;
}

LVL_PhysEnv::~LVL_PhysEnv()
{}

void LVL_PhysEnv::init()
{
    m_momentum.x = data.x;
    m_momentum.y = data.y;
    m_momentum.w = data.w;
    m_momentum.h = data.h;
    if(!m_treemap.m_is_registered)
    {
        m_treemap.addToScene();
    } else {
        m_treemap.updatePosAndSize();
    }

    switch(data.env_type)
    {
    case LevelPhysEnv::ENV_WATER:
        env_type = Env_Water;
        break;

    case LevelPhysEnv::ENV_QUICKSAND:
        env_type = Env_Quicksand;
        break;

    case LevelPhysEnv::ENV_AIR:
        env_type = Env_Air;
        break;

    default:
        env_type = Env_SameAsAround;
    }

    m_blocked[1] = Block_NONE;
    m_blocked[2] = Block_NONE;
    m_scene->m_layers.registerItem(data.layer, this);

    m_momentum.saveOld();
    m_momentum_relative.saveOld();
}
