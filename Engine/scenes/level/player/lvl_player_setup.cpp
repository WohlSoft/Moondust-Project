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

#include "../lvl_player.h"
#include "../../scene_level.h"

#include <audio/pge_audio.h>
#include <data_configs/config_manager.h>

#include <script/bindings/level/classes/luaclass_level_playerstate.h>

bool LVL_Player::isInited()
{
    return _isInited;
}

void LVL_Player::init()
{
    setCharacter(characterID, stateID);

    _direction = data.direction;
    long posX = data.x+(data.w/2)-(state_cur.width/2);
    long posY = data.y = data.y+data.h-state_cur.height;
    setSize(state_cur.width, state_cur.height);
    setPos(posX, posY);
    phys_setup.max_vel_y=12;
    animator.tickAnimation(0.f);
    isLocked=false;

    if(isLuaPlayer)
    {
        try{
            lua_onInit();
        } catch (luabind::error& e) {
            _scene->getLuaEngine()->postLateShutdownError(e);
            return;
        }
    }

    _isInited=true;
    _syncSection();

    m_momentum.saveOld();
}

void LVL_Player::setCharacter(int CharacterID, int _stateID)
{
    if(!_doSafeSwitchCharacter)
    {
        if(!ConfigManager::playable_characters.contains(CharacterID))
        {
            PGE_Audio::playSoundByRole(obj_sound_role::PlayerSpring);
            return;
        }
        else
            setup = ConfigManager::playable_characters[CharacterID];
        states =   ConfigManager::playable_characters[CharacterID].states;
        if(!states.contains(_stateID))
        {
            PGE_Audio::playSoundByRole(obj_sound_role::CameraSwitch);
            return;
        }
        else
            state_cur = states[_stateID];
    }
    else _doSafeSwitchCharacter=false;

    physics = setup.phys_default;
    physics_cur = physics[environment];

    long tID = ConfigManager::getLvlPlayerTexture(CharacterID, _stateID);
    if( tID >= 0 )
    {
        texId = ConfigManager::level_textures[tID].texture;
        texture = ConfigManager::level_textures[tID];
        frameW = ConfigManager::level_textures[tID].w / setup.matrix_width;
        frameH = ConfigManager::level_textures[tID].h / setup.matrix_height;
    }

    animator.setSize(setup.matrix_width, setup.matrix_height);
    animator.installAnimationSet(state_cur.sprite_setup);

    phys_setup.max_vel_x = fabs(_isRunning ?
                physics_cur.MaxSpeed_run :
                physics_cur.MaxSpeed_walk);
    phys_setup.min_vel_x = -fabs(_isRunning ?
                physics_cur.MaxSpeed_run :
                physics_cur.MaxSpeed_walk);
    phys_setup.max_vel_y = fabs(physics_cur.MaxSpeed_down);
    phys_setup.min_vel_y = -fabs(physics_cur.MaxSpeed_up);
    phys_setup.decelerate_x = physics_cur.decelerate_air;
    phys_setup.gravityScale = physics_cur.gravity_scale;
    phys_setup.gravityAccel = physics_cur.gravity_accel;
    phys_setup.grd_dec_x    = physics_cur.walk_force;

    /********************floating************************/
    floating_allow=state_cur.allow_floating;
    floating_maxtime=state_cur.floating_max_time; //!< Max time to float
    if(_isInited)
    {
        if(!floating_allow && floating_isworks)
            floating_timer=0;
    }
    /********************floating************************/

    /********************duck**************************/
    duck_allow= state_cur.duck_allow;
    /********************duck**************************/

    characterID = CharacterID;
    stateID = _stateID;
    //Refresh global state
    if(global_state)
    {
        global_state->setCharacterID( CharacterID );
        global_state->setStateID( _stateID );
    }

    if(_isInited)
    {
        ducking = (ducking & state_cur.duck_allow);
        float cx = m_momentum.centerX();
        float b = m_momentum.bottom();
        setSize(state_cur.width, ducking?state_cur.duck_height:state_cur.height);
        setPos(cx-m_width_registered/2, b-m_height_registered);
        PlayerState x = _scene->getGameState()->getPlayerState(playerID);
        x.characterID    = characterID;
        x.stateID        = stateID;
        x._chsetup.state = stateID;
        _scene->getGameState()->setPlayerState(playerID, x);

        //Apply changed animation on character switchers and configure switches and filters
        _scene->character_switchers.refreshState();

        try{
            lua_onTransform(characterID, stateID);
        } catch (luabind::error& e) {
            _scene->getLuaEngine()->postLateShutdownError(e);
        }
    }
}

void LVL_Player::setCharacterSafe(int CharacterID, int _stateID)
{
    if( (CharacterID<1) || !ConfigManager::playable_characters.contains(CharacterID) )
    {
        PGE_Audio::playSoundByRole(obj_sound_role::PlayerSpring);
        return;
    }
    else
        setup = ConfigManager::playable_characters[CharacterID];
    states =   ConfigManager::playable_characters[CharacterID].states;

    float oldHeight = m_momentum.h;

    if((_stateID<1) || !states.contains(_stateID))
    {
        PGE_Audio::playSoundByRole(obj_sound_role::CameraSwitch);
        return;
    }
    else
        state_cur = states[_stateID];
    _doSafeSwitchCharacter=true;

    _heightDelta=oldHeight-( (ducking&&state_cur.duck_allow) ? state_cur.duck_height : state_cur.height);
    if(_heightDelta>0.0f) _heightDelta=0.0f;

    characterID = CharacterID;
    stateID = _stateID;
}

void LVL_Player::setPlayerPointInfo(PlayerPoint pt)
{
    data = pt;
    playerID=pt.id;
    PlayerState x = _scene->getGameState()->getPlayerState(playerID);
    characterID = x.characterID;
    stateID     = x._chsetup.state;
    health      = x._chsetup.health;
    if(global_state)
        global_state->setHealth(health);
    if( _isInited )
        setCharacter(characterID, stateID);
}




bool LVL_Player::locked()
{
    return isLocked;
}

void LVL_Player::setLocked(bool lock)
{
    isLocked=lock;
    setPaused(lock);
}


LVL_Npc *LVL_Player::lua_spawnNPC(int npcID, int sp_type, int sp_dir, bool reSpawnable)
{
    LevelNPC def = FileFormats::CreateLvlNpc();
    def.id=npcID;
    def.x=round(posX());
    def.y=round(posY());
    def.direct=_direction;
    def.generator=false;
    def.layer="Spawned NPCs";
    def.attach_layer = "";
    def.event_activate = "";
    def.event_die = "";
    def.event_talk = "";
    def.event_emptylayer = "";
    return _scene->spawnNPC(def,
                           (LevelScene::NpcSpawnType)sp_type,
                           (LevelScene::NpcSpawnDirection)sp_dir, reSpawnable);
}

void LVL_Player::setHealth(int _health)
{
    health=_health;
    if(global_state)
        global_state->setHealth(_health);
}
