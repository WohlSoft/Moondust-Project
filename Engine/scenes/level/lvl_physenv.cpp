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

#include "lvl_physenv.h"
#include "lvl_scene_ptr.h"

LVL_PhysEnv::LVL_PhysEnv() : PGE_Phys_Object()
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
    env_type = data.quicksand ? Env_Quicksand : Env_Water;
    collide_player = COLLISION_NONE;
    collide_npc = COLLISION_NONE;
    LvlSceneP::s->layers.registerItem(data.layer, this);
}

