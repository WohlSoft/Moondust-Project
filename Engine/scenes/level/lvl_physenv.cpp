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

#include "lvl_physenv.h"
#include "../scene_level.h"

const int LVL_PhysEnv::numOfEnvironments=3;

LVL_PhysEnv::LVL_PhysEnv(LevelScene *_parent) : PGE_Phys_Object(_parent)
{
    type = LVLPhysEnv;
    env_type = Env_Water;
}

LVL_PhysEnv::~LVL_PhysEnv()
{}

void LVL_PhysEnv::init()
{
    setSize(data.w, data.h);
    setPos(data.x, data.y);
    switch(data.env_type)
    {
        case LevelPhysEnv::ENV_WATER:       env_type = Env_Water; break;
        case LevelPhysEnv::ENV_QUICKSAND:   env_type = Env_Quicksand; break;
        case LevelPhysEnv::ENV_AIR:         env_type = Env_Air; break;
        default:                            env_type = Env_SameAsAround;
    }
    LEGACY_collide_player = COLLISION_NONE;
    LEGACY_collide_npc = COLLISION_NONE;
    _scene->layers.registerItem(data.layer, this);
}

