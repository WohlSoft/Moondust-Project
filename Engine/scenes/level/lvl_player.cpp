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

#include "lvl_player.h"
#include "lvl_physenv.h"

LVL_Player::LVL_Player() : PGE_Phys_Object()
{
    camera = NULL;

    playerID = 0;
    isLocked = false;
    _no_render = false;
    _isInited = false;
    isLuaPlayer = false;

    isExiting = false;

    _direction = 1;

    type = LVLPlayer;

    frameW=100;
    frameH=100;

    stateID     =1;
    characterID =1;

    _heightDelta=0;

    _doSafeSwitchCharacter=false;

    _stucked=false;

    collided_slope=false;
    collided_slope_direct=0;
    collided_slope_angle_ratio=0.0f;

    JumpPressed=false;
    _onGround=false;
    on_slippery_surface=false;
    forceCollideCenter=false;

    climbing=false;

    collide_player = COLLISION_ANY;
    collide_npc    = COLLISION_NONE;

    environment = LVL_PhysEnv::Env_Air;
    last_environment = LVL_PhysEnv::Env_Air;

    bumpDown=false;
    bumpUp=false;
    bumpVelocity=0.0f;

    bumpJumpVelocity=0.0f;
    bumpJumpTime=0.0f;

    _exiting_swimTimer=700;

    health=3;
    invincible=false;
    invincible_delay=0.0f;
    blink_screen=false;
    blink_screen_state=true;
    doHarm=false;
    doHarm_damage=0;

    jumpTime=0;
    jumpVelocity=5.3f;

    isAlive = true;
    doKill = false;
    kill_reason=DEAD_fall;

    _isRunning = false;

    contactedWithWarp = false;
    contactedWarp = NULL;
    wasEnteredTimeout = 0;
    wasEntered = false;
    warpDirectO = 0;
    isWarping=false;
    warpPipeOffset=0;

    collided_talkable_npc = NULL;

    gscale_Backup=0.f;//!< BackUP of last gravity scale

    duck_allow=false;
    ducking=false;

    /********************Attack************************/
    attack_enabled=true;
    attack_pressed=false;
    /********************Attack************************/

    /********************floating************************/
    floating_allow=false;
    floating_maxtime=1000; //!< Max time to float
    floating_isworks=false;                         //!< Is character currently floating in air
    floating_timer=0;                            //!< Milliseconds to float
    floating_start_type=false;
    /********************floating************************/
}

LVL_Player::~LVL_Player()
{}






