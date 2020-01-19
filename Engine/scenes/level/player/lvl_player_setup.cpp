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

#include "../lvl_player.h"
#include "../../scene_level.h"

#include <audio/pge_audio.h>
#include <data_configs/config_manager.h>
#include <common_features/logger.h>

#include <script/bindings/level/classes/luaclass_level_playerstate.h>

bool LVL_Player::isInited()
{
    return m_isInited;
}

bool LVL_Player::init()
{
    if(!setCharacter(characterID, stateID))
        return false;
    m_direction = data.direction;
    long posX = data.x + (data.w / 2) - (state_cur.width / 2);
    long posY = data.y = data.y + data.h - state_cur.height;
    setSize(state_cur.width, state_cur.height);
    setPos(posX, posY);

    phys_setup.max_vel_y = 12;
    m_animator.tickAnimation(0.0);
    m_isLocked = false;

    if(m_isLuaPlayer)
    {
        try
        {
            lua_onInit();
        }
        catch(luabind::error &e)
        {
            m_scene->getLuaEngine()->postLateShutdownError(e);
            return false;
        }
    }

    m_isInited = true;
    _syncSection();
    m_momentum.saveOld();
    return true;
}

bool LVL_Player::setCharacter(unsigned long CharacterID, unsigned long _stateID)
{
    if(!_doSafeSwitchCharacter)
    {
        if(!ConfigManager::playable_characters.contains(CharacterID))
        {
            PGE_Audio::playSoundByRole(obj_sound_role::PlayerSpring);
            return false;
        }
        else
            setup = ConfigManager::playable_characters[CharacterID];

        states =   ConfigManager::playable_characters[CharacterID].states;

        if(!states.contains(_stateID))
        {
            PGE_Audio::playSoundByRole(obj_sound_role::CameraSwitch);
            return false;
        }
        else
            state_cur = states[_stateID];
    }
    else
        _doSafeSwitchCharacter = false;

    physics = setup.phys_default;
    physics_cur = physics[static_cast<unsigned long>(abs(environment))];
    int tID = ConfigManager::getLvlPlayerTexture(CharacterID, _stateID);

    if(tID >= 0)
    {
        texId = ConfigManager::level_textures[tID].texture;
        texture = ConfigManager::level_textures[tID];
        m_frameW = ConfigManager::level_textures[tID].w / setup.matrix_width;
        m_frameH = ConfigManager::level_textures[tID].h / setup.matrix_height;
    }

    m_animator.setSize(setup.matrix_width, setup.matrix_height);
    if(!m_animator.installAnimationSet(state_cur.sprite_setup))
    {
        pLogCritical("Fail to initialize animator for the playable character %lu with state %1u.",
                     characterID, stateID);
        return false;
    }
    phys_setup.max_vel_x =  fabs(m_isRunning ?
                                 physics_cur.MaxSpeed_run :
                                 physics_cur.MaxSpeed_walk);
    phys_setup.min_vel_x = -fabs(m_isRunning ?
                                 physics_cur.MaxSpeed_run :
                                 physics_cur.MaxSpeed_walk);
    phys_setup.max_vel_y =  fabs(physics_cur.MaxSpeed_down);
    phys_setup.min_vel_y = -fabs(physics_cur.MaxSpeed_up);
    phys_setup.decelerate_x = physics_cur.decelerate_air;
    phys_setup.gravityScale = physics_cur.gravity_scale;
    phys_setup.gravityAccel = physics_cur.gravity_accel;
    phys_setup.grd_dec_x    = physics_cur.walk_force;
    /********************floating************************/
    m_floatingAllow   = state_cur.allow_floating;
    m_floatingMaxtime = state_cur.floating_max_time; // Max time to float

    if(m_isInited)
    {
        if(!m_floatingAllow && m_floatingIsWorks)
            m_floatingTimer = 0;
    }

    /********************floating************************/
    /********************duck**************************/
    m_duckAllow = state_cur.duck_allow;
    /********************duck**************************/
    characterID = CharacterID;
    stateID = _stateID;

    //Refresh global state
    if(m_global_state)
    {
        m_global_state->setCharacterID(CharacterID);
        m_global_state->setStateID(_stateID);
    }

    if(m_isInited)
    {
        m_ducking = (m_ducking & state_cur.duck_allow);
        double cx = m_momentum.centerX();
        double b = m_momentum.bottom();
        setSize(state_cur.width, m_ducking ? state_cur.duck_height : state_cur.height);
        setPos(cx - m_momentum.w / 2.0, b - m_momentum.h);
        PlayerState x = m_scene->getGameState()->getPlayerState(playerID);
        x.characterID    = characterID;
        x.stateID        = stateID;
        x._chsetup.state = stateID;
        m_scene->getGameState()->setPlayerState(playerID, x);
        //Apply changed animation on character switchers and configure switches and filters
        m_scene->m_characterSwitchers.refreshState();

        try
        {
            lua_onTransform(characterID, stateID);
        }
        catch(luabind::error &e)
        {
            m_scene->getLuaEngine()->postLateShutdownError(e);
            return false;
        }
    }
    return true;
}

void LVL_Player::setCharacterSafe(unsigned long CharacterID, unsigned long _stateID)
{
    if((CharacterID < 1) || !ConfigManager::playable_characters.contains(CharacterID))
    {
        PGE_Audio::playSoundByRole(obj_sound_role::PlayerSpring);
        return;
    }
    else
        setup = ConfigManager::playable_characters[CharacterID];

    states =   ConfigManager::playable_characters[CharacterID].states;
    double oldHeight = m_momentum.h;

    if((_stateID < 1) || !states.contains(_stateID))
    {
        PGE_Audio::playSoundByRole(obj_sound_role::CameraSwitch);
        return;
    }
    else
        state_cur = states[_stateID];

    _doSafeSwitchCharacter = true;
    _heightDelta = oldHeight - ((m_ducking && state_cur.duck_allow) ? state_cur.duck_height : state_cur.height);

    if(_heightDelta > 0.0) _heightDelta = 0.0;

    characterID = CharacterID;
    stateID = _stateID;
}

void LVL_Player::setPlayerPointInfo(PlayerPoint pt)
{
    data = pt;
    playerID = static_cast<int>(pt.id);
    PlayerState x = m_scene->getGameState()->getPlayerState(playerID);
    characterID = x.characterID;
    stateID     = x._chsetup.state;
    m_health      = static_cast<int>(x._chsetup.health);

    if(m_global_state)
        m_global_state->setHealth(m_health);

    if(m_isInited)
        setCharacter(characterID, stateID);
}




bool LVL_Player::locked()
{
    return m_isLocked;
}

void LVL_Player::setLocked(bool lock)
{
    m_isLocked = lock;
    setPaused(lock);
}


LVL_Npc *LVL_Player::lua_spawnNPC(unsigned long npcID, int sp_type, int sp_dir, bool reSpawnable)
{
    LevelNPC def = FileFormats::CreateLvlNpc();
    def.id = npcID;
    def.x = static_cast<long>(round(posX()));
    def.y = static_cast<long>(round(posY()));
    def.direct = m_direction;
    def.generator = false;
    def.layer = "Spawned NPCs";
    def.attach_layer = "";
    def.event_activate = "";
    def.event_die = "";
    def.event_talk = "";
    def.event_emptylayer = "";
    return m_scene->spawnNPC(def,
                             static_cast<LevelScene::NpcSpawnType>(sp_type),
                             static_cast<LevelScene::NpcSpawnDirection>(sp_dir), reSpawnable);
}

void LVL_Player::setHealth(int _health)
{
    m_health = _health;

    if(m_global_state)
        m_global_state->setHealth(_health);
}
