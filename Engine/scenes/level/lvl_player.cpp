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

#include "lvl_player.h"
#include "lvl_physenv.h"

LVL_Player::LVL_Player(LevelScene *_parent) : PGE_Phys_Object(_parent)
{
    camera = nullptr;
    /* ****** PHYSICS ******* */
    m_filterID = 1;
    m_blocked[1] = Block_NONE;
    m_blocked[2] = Block_NONE;
    m_danger[1] = Block_NONE;
    m_danger[2] = Block_NONE;
    m_allowHoleRuning = true;
    m_onSlopeFloorTopAlign = true;
    m_contactPadding = -1.0;
    /* ****** PHYSICS ******* */
    type = LVLPlayer;
    m_bodytype = Body_DYNAMIC;
    playerID = 0;
    data = FileFormats::CreateLvlPlayerPoint(0);
    m_isLocked = false;
    m_noRender = false;
    m_isInited = false;
    m_global_state = nullptr;
    m_keysPrev = ResetControlKeys();
    m_isLuaPlayer = false;
    m_isExiting = false;
    m_direction = 1;
    m_frameW = 100;
    m_frameH = 100;
    stateID     = 1;
    characterID = 1;
    _heightDelta = 0;
    _doSafeSwitchCharacter = false;
    _stucked = false;
    m_jumpPressed = false;
    forceCollideCenter = false;
    m_climbing = false;
    m_climbableHeight = 0.0;
    environment = LVL_PhysEnv::Env_Air;
    last_environment = LVL_PhysEnv::Env_Air;
    m_bumpDown = false;
    m_bumpUp = false;
    m_bumpVelocity = 0.0;
    m_bumpJumpVelocity = 0.0;
    m_bumpJumpTime = 0;
    m_exiting_swimTimer = 700;
    m_health = 3;
    m_invincible = false;
    m_invincibleDelay = 0.0;
    m_blinkScreen = false;
    m_jumpTime = 0;
    m_jumpVelocity = 5.3;
    isAlive = true;
    m_killDo = false;
    m_killReason = DEAD_fall;
    m_isRunning = false;
    m_contactedWithWarp = false;
    m_contactedWarp = NULL;
    m_wasEnteredTimeout = 0;
    m_wasEntered = false;
    m_warpDirectO = 0;
    m_isWarping = false;
    m_warpPipeOffset = 0;
    m_warpFrameW = 0.0;
    m_warpFrameH = 0.0;
    collided_talkable_npc = NULL;
    m_gscale_Backup = 0.0; //!< BackUP of last gravity scale
    m_duckAllow = false;
    m_ducking = false;
    /********************Attack************************/
    m_attackEnabled = true;
    m_attackPressed = false;
    /********************Attack************************/
    /********************floating************************/
    m_floatingAllow = false;
    m_floatingMaxtime = 1000; //!< Max time to float
    m_floatingIsWorks = false;                       //!< Is character currently floating in air
    m_floatingTimer = 0;                          //!< Milliseconds to float
    m_floatingStartType = false;
    /********************floating************************/
}

LVL_Player::~LVL_Player()
{}
