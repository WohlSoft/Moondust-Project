/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2024 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "lvl_npc.h"
#include "../scene_level.h"

LVL_Npc::LVL_Npc(LevelScene *_parent) :
    PGE_Phys_Object(_parent),
    detector_player_pos(this)
{
    /* ****** PHYSICS ******* */
    m_filterID = 2;
    m_blocked[1] = Block_NONE;
    m_blocked[2] = Block_NONE;
    m_danger[1] = Block_ALL;
    m_danger[2] = Block_NONE;
    m_allowHoleRuning = false;
    m_onSlopeFloorTopAlign = true;
    /* ****** PHYSICS ******* */
    type = LVLNPC;
    m_bodytype = Body_DYNAMIC;
    _npc_id = 0;
    data = FileFormats::CreateLvlNpc();
    animated = false;
    animator_ID = 0;
    killed = false;
    transformedFromBlockID = -1ul;
    transformedFromNpcID = -1ul;
    isActivated = false;
    deActivatable = true;
    reSpawnable = true;
    wasDeactivated = false;
    offSectionDeactivate = false;
    activationTimeout = 4000;
    m_spawnedGeneratorType = LevelScene::GENERATOR_APPEAR;
    m_spawnedGeneratorDirection = LevelScene::SPAWN_UP;
    warpSpawing = false;
    m_isInited = false;
    isLuaNPC = false;
    isWarping = false;
    is_static = false;
    is_activity = true;
    is_shared_animation = false;
    keep_position_on_despawn = false;
    warpDirectO = 0;
    warpSpriteOffset = 1.0f;
    warpFrameW = 0.0f;
    warpFrameH = 0.0f;
    m_disableBlockCollision = false;
    disableNpcCollision = false;
    enablePlayerCollision = true;
    _stucked = false;
    bumpDown = false;
    bumpUp = false;
    health = 1;
    m_isGenerator = false;
    generatorTimeLeft = 0;
    generatorType = 0;
    generatorDirection = 0;
    resetThrowned();
    buddies_list = NULL;
    buddies_updated = false;
    buddies_leader = NULL;
    //collision_speed_add.clear();
}

LVL_Npc::~LVL_Npc()
{}


void LVL_Npc::show()
{
    wasDeactivated = false;
    PGE_Phys_Object::show();
}

void LVL_Npc::hide()
{
    activationTimeout = -1;
    PGE_Phys_Object::hide();
}

void LVL_Npc::setVisible(bool visible)
{
    if(visible)
        wasDeactivated = false;
    else
        activationTimeout = -1;
    PGE_Phys_Object::setVisible(visible);
}


/****************Additional event callbacks******************/
LVL_Npc::KillEvent::KillEvent()
{
    cancel = false;
    reason_code = DAMAGE_NOREASON;
    killed_by = self;
    killer_p = NULL;
    killer_n = NULL;
}

LVL_Npc::KillEvent::KillEvent(const LVL_Npc::KillEvent &ke)
{
    cancel = ke.cancel;
    reason_code = ke.reason_code;
    killed_by = ke.killed_by;
    killer_p = ke.killer_p;
    killer_n = ke.killer_n;
}

LVL_Npc::HarmEvent::HarmEvent()
{
    cancel = false;
    damage = 1;
    reason_code = DAMAGE_NOREASON;
    killed_by = self;
    killer_p = NULL;
    killer_n = NULL;
}

LVL_Npc::HarmEvent::HarmEvent(const LVL_Npc::HarmEvent &he)
{
    cancel = he.cancel;
    damage = he.damage;
    reason_code = he.reason_code;
    killed_by = he.killed_by;
    killer_p = he.killer_p;
    killer_n = he.killer_n;
}
