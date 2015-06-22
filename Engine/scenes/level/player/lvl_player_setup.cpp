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

#include "../lvl_player.h"
#include "../lvl_scene_ptr.h"

#include <audio/pge_audio.h>
#include <data_configs/config_manager.h>


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
    _isInited=true;

    _syncSection();
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

    if(_isInited)
    {
        ducking = (ducking & state_cur.duck_allow);
        float cx = posRect.center().x();
        float b = posRect.bottom();
        setSize(state_cur.width, ducking?state_cur.duck_height:state_cur.height);
        setPos(cx-_width/2, b-_height);
        PlayerState x = LvlSceneP::s->getGameState()->getPlayerState(playerID);
        x.characterID    = characterID;
        x.stateID        = stateID;
        x._chsetup.state = stateID;
        LvlSceneP::s->getGameState()->setPlayerState(playerID, x);
        _collideUnduck();

        #ifdef COLLIDE_DEBUG
        //Freeze time to check out what happening at moment
        //GlRenderer::makeShot();
        //LvlSceneP::s->isTimeStopped=true;
        #endif
    }
}

void LVL_Player::setCharacterSafe(int CharacterID, int _stateID)
{
    if(!ConfigManager::playable_characters.contains(CharacterID))
    {
        PGE_Audio::playSoundByRole(obj_sound_role::PlayerSpring);
        return;
    }
    else
        setup = ConfigManager::playable_characters[CharacterID];
    states =   ConfigManager::playable_characters[CharacterID].states;

    float oldHeight = posRect.height();

    if(!states.contains(_stateID))
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
    PlayerState x = LvlSceneP::s->getGameState()->getPlayerState(playerID);
    characterID = x.characterID;
    stateID     = x._chsetup.state;
    if(_isInited) setCharacter(characterID, stateID);
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

