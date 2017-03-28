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

#include "../lvl_player.h"
#include "../../scene_level.h"

#include <audio/pge_audio.h>
#include <audio/play_music.h>

#include <settings/debugger.h>



LVL_Player_harm_event::LVL_Player_harm_event() :
    doHarm(false),
    doHarm_damage(0)
{}


void LVL_Player::attack(LVL_Player::AttackDirection _dir)
{
    PGE_RectF attackZone;

    switch(_dir)
    {
    case Attack_Up:
        attackZone.setRect(posCenterX() - 5, top() - 17, 10, 5);
        break;
    case Attack_Down:
        attackZone.setRect(posCenterX() - 5, bottom(), 10, 5);
        break;
    case Attack_Forward:
        if(m_direction >= 0)
            attackZone.setRect(right(), bottom() - 32, 10, 10);
        else
            attackZone.setRect(left() - 10, bottom() - 32, 10, 10);
        break;
    }


    std::vector<PGE_Phys_Object *> bodies;
    m_scene->queryItems(attackZone, &bodies);
    int contacts = 0;

    std::vector<LVL_Block *>    target_blocks;
    std::vector<LVL_Npc *>      target_npcs;
    for(PGE_RenderList::iterator it = bodies.begin(); it != bodies.end(); it++)
    {
        PGE_Phys_Object *visibleBody = *it;
        contacts++;
        if(visibleBody == this) continue;
        if(visibleBody == NULL)
            continue;
        if(!visibleBody->isVisible()) //Don't whip elements from hidden layers!
            continue;
        switch(visibleBody->type)
        {
        case PGE_Phys_Object::LVLBlock:
            target_blocks.push_back(static_cast<LVL_Block *>(visibleBody));
            break;
        case PGE_Phys_Object::LVLNPC:
            target_npcs.push_back(static_cast<LVL_Npc *>(visibleBody));
            break;
        case PGE_Phys_Object::LVLPlayer:
        default:
            break;
        }
    }

    for(LVL_Block *x : target_blocks)
    {
        if(!x) continue;
        if(x->m_destroyed) continue;
        if(x->sizable && _dir == Attack_Forward)
            continue;
        x->hit();
        if(!x->m_destroyed)
        {
            m_scene->launchStaticEffectC(69, x->posCenterX(), x->posCenterY(), 1, 0, 0, 0, 0);
            PGE_Audio::playSoundByRole(obj_sound_role::WeaponExplosion);
        }
        x->setDestroyed(true);
    }
    for(LVL_Npc *x : target_npcs)
    {
        if(!x) continue;
        if(x->isPaused()) continue; //Don't attack NPC with paused physics!
        if(x->isKilled()) continue;
        if(x->m_isGenerator) continue;
        x->doHarm(LVL_Npc::DAMAGE_BY_PLAYER_ATTACK);
        m_scene->launchStaticEffectC(75, attackZone.center().x(), attackZone.center().y(), 1, 0, 0, 0, 0);
        kill_npc(x, NPC_Kicked);
    }
}


void LVL_Player::kill_npc(LVL_Npc *target, LVL_Player::kill_npc_reasons reason)
{
    if(!target) return;
    if(!target->isKilled()) return;

    switch(reason)
    {
    case NPC_Unknown:
        break;
    case NPC_Stomped:
        PGE_Audio::playSoundByRole(obj_sound_role::PlayerStomp);
        break;
    case NPC_Kicked:
        PGE_Audio::playSoundByRole(obj_sound_role::PlayerKick);
        break;
    case NPC_Taked_Coin:
        PGE_Audio::playSoundByRole(obj_sound_role::BonusCoin);
        break;
    case NPC_Taked_Powerup:
        break;
    }

    if(reason == NPC_Taked_Coin)
    {
        try
        {
            lua_onTakeNpc(target);
        }
        catch(luabind::error &e)
        {
            m_scene->getLuaEngine()->postLateShutdownError(e);
            return;
        }
    }
    else
    {
        try
        {
            lua_onKillNpc(target);
        }
        catch(luabind::error &e)
        {
            m_scene->getLuaEngine()->postLateShutdownError(e);
            return;
        }
    }

    if(target->setup->setup.exit_is)
    {
        long snd = target->setup->setup.exit_snd;
        if(snd <= 0)
        {
            switch(target->setup->setup.exit_code)
            {
            case  1:
                snd = ConfigManager::getSoundByRole(obj_sound_role::LevelExit01);
                break;
            case  2:
                snd = ConfigManager::getSoundByRole(obj_sound_role::LevelExit02);
                break;
            case  3:
                snd = ConfigManager::getSoundByRole(obj_sound_role::LevelExit03);
                break;
            case  4:
                snd = ConfigManager::getSoundByRole(obj_sound_role::LevelExit04);
                break;
            case  5:
                snd = ConfigManager::getSoundByRole(obj_sound_role::LevelExit05);
                break;
            case  6:
                snd = ConfigManager::getSoundByRole(obj_sound_role::LevelExit06);
                break;
            case  7:
                snd = ConfigManager::getSoundByRole(obj_sound_role::LevelExit07);
                break;
            case  8:
                snd = ConfigManager::getSoundByRole(obj_sound_role::LevelExit08);
                break;
            case  9:
                snd = ConfigManager::getSoundByRole(obj_sound_role::LevelExit09);
                break;
            case 10:
                snd = ConfigManager::getSoundByRole(obj_sound_role::LevelExit10);
                break;
            default:
                break;
            }
        }
        if(snd > 0)
        {
            PGE_MusPlayer::stop();
            PGE_Audio::playSound(snd);
        }
        /***********************Reset and unplug controllers************************/
        m_scene->m_player1Controller->resetControls();
        m_scene->m_player1Controller->sendControls();
        m_scene->m_player1Controller->removeFromControl(this);
        m_scene->m_player2Controller->resetControls();
        m_scene->m_player2Controller->sendControls();
        m_scene->m_player2Controller->removeFromControl(this);
        /***********************Reset and unplug controllers*end********************/
        if(target->setup->setup.exit_walk_direction < 0)
            keys.left = true;
        else if(target->setup->setup.exit_walk_direction > 0)
            keys.right = true;
        m_isExiting = true;
        m_scene->setExiting(target->setup->setup.exit_delay, target->setup->setup.exit_code);
    }
}

void LVL_Player::harm(int _damage)
{
    if(m_invincible || PGE_Debugger::cheat_pagangod) return;

    LVL_Player_harm_event dmg;
    dmg.doHarm = true;
    dmg.doHarm_damage = _damage;

    try
    {
        lua_onHarm(&dmg);
    }
    catch(luabind::error &e)
    {
        m_scene->getLuaEngine()->postLateShutdownError(e);
    }

    if(!dmg.doHarm) return;
    _damage = dmg.doHarm_damage;

    //doHarm=true;
    m_health -= _damage;
    if(m_health <= 0)
        kill(DEAD_killed);
    else
    {
        //PGE_Audio::playSoundByRole(obj_sound_role::PlayerShrink);
        setInvincible(true, 3000, true);
    }
}

void LVL_Player::setInvincible(bool state, double delay, bool enableScreenBlink)
{
    m_invincible = state;
    m_invincibleDelay = delay;
    m_blinkScreen = enableScreenBlink;
}


void LVL_Player::kill(deathReason reason)
{
    if((reason != DEAD_fall) && PGE_Debugger::cheat_pagangod) return;

    m_killDo = true;
    isAlive = false;
    m_killReason = reason;

    if(m_global_state)
        m_global_state->setHealth(0);

    setPaused(true);
    if(m_killReason == DEAD_burn)
    {
        m_scene->launchStaticEffectC(ConfigManager::g_setup_npc.eff_lava_burn,
                                    posCenterX(),
                                    posCenterY(), 1, 0, 0, 0, 0, m_direction);
    }
    else if(m_killReason == DEAD_killed)
    {
        SpawnEffectDef effect = setup.fail_effect;
        if(effect.id > 0)
        {
            effect.startX = m_momentum.centerX();
            effect.startY = m_momentum.centerY();
            effect.velocityX *= m_direction;
            effect.direction *= m_direction;
            m_scene->launchEffect(effect, true);
        }
    }
    unregister();
}

void LVL_Player::unregister()
{
    isAlive = false;
    m_is_visible = false;
    //Unregister controllers
    if(m_scene->m_player1Controller) m_scene->m_player1Controller->removeFromControl(this);
    if(m_scene->m_player2Controller) m_scene->m_player2Controller->removeFromControl(this);

    unregisterFromTree();

    //Store into death list
    m_scene->m_playersDead.push_back(this);
}


