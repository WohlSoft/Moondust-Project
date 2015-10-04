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

#include "lvl_npc.h"
#include "lvl_scene_ptr.h"

LVL_Npc::LVL_Npc() : PGE_Phys_Object(), detector_player_pos(this)
{
    type = LVLNPC;
    data = FileFormats::dummyLvlNpc();
    animated=false;
    animator_ID=0;
    killed=false;

    isActivated=false;
    deActivatable=true;
    reSpawnable=true;
    wasDeactivated=false;
    activationTimeout=4000;
    warpSpawing=false;

    _isInited=false;
    isLuaNPC=false;
    isWarping=false;
    is_scenery=false;
    is_activity=true;
    is_shared_animation=false;

    warpDirectO=0;
    warpSpriteOffset=1.0f;
    warpFrameW=0.0f;
    warpFrameH=0.0f;

    _onGround=false;

    disableBlockCollision=false;
    disableNpcCollision=false;
    _stucked=false;

    bumpDown=false;
    bumpUp=false;

    forceCollideCenter=false;
    _heightDelta=0.0f;
    cliffDetected=false;

    health = 1;

    isGenerator=false;
    generatorTimeLeft=0;
    generatorType=0;
    generatorDirection=0;

    resetThrowned();

    buddies_list=NULL;
    buddies_updated=false;
    buddies_leader=NULL;
}

LVL_Npc::~LVL_Npc()
{}


void LVL_Npc::hide()
{
    activationTimeout=-1;
    PGE_Phys_Object::hide();
}

void LVL_Npc::show()
{
    wasDeactivated=false;
    PGE_Phys_Object::show();
}
