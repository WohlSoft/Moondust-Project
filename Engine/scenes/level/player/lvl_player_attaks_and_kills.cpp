/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <audio/play_music.h>

#include <settings/debugger.h>



LVL_Player_harm_event::LVL_Player_harm_event() :
    doHarm(false),
    doHarm_damage(0)
{}


void LVL_Player::attackCN(LVL_Player::AttackDirection _dir)
{
    PGE_RectF attackZone;

    switch(_dir)
    {
    case Attack_Up:
        attackZone.setRect(25, -17, 10, 5);
        break;
    case Attack_Down:
        attackZone.setRect(25, 0, 10, 5);
        break;
    case Attack_Forward:
        attackZone.setRect(width() / 2, -32, 10, 10);
        break;
    }
    attackArea(attackZone, AttackType_ForceDestroy, LVL_Npc::DAMAGE_BY_PLAYER_ATTACK, F_LVLBlock|F_LVLNPC);
}

void LVL_Player::attackArea(PGE_RectF area, int action, int type, int filters)
{
    std::vector<PGE_Phys_Object *> bodies;
    if(m_direction >= 0)
    {
        area.setLeft(posCenterX() + area.left());
        area.setRight(posCenterX() + area.right());
        area.setTop(bottom() + area.top());
        area.setBottom(bottom() + area.bottom());
    }
    else
    {
        double oldLeft = area.left(), oldRight = area.right();
        area.setLeft(posCenterX() - oldRight);
        area.setRight(posCenterX() - oldLeft);
        area.setTop(bottom() + area.top());
        area.setBottom(bottom() + area.bottom());
    }

    m_scene->queryItems(area, &bodies);
    int contacts = 0;
    bool findBlocks = (filters & (F_LVLBlock)) != 0;
    //bool findBGOs = (filters & (LVLBGO << 1)) != 0;
    bool findNPCs = (filters & (F_LVLNPC)) != 0;

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
            if(findBlocks)
                target_blocks.push_back(static_cast<LVL_Block *>(visibleBody));
            break;
        case PGE_Phys_Object::LVLNPC:
            if(findNPCs)
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
        if( ((x->m_blocked[m_filterID] & Block_LEFT) == 0) &&
            ((x->m_blocked[m_filterID] & Block_RIGHT) == 0)/* &&
            (_dir == Attack_Forward)*/)
            continue; // Skip non-solid blocks
        switch(action)
        {
        case AttackType_Hit:
            x->hit();
            break;
        case AttackType_ForceDestroy:
            x->hit();
            if(!x->m_destroyed)
            {
                m_scene->launchStaticEffectC(69, x->posCenterX(), x->posCenterY(), 1, 0, 0, 0, 0);
                PGE_Audio::playSoundByRole(obj_sound_role::WeaponExplosion);
            }
            x->setDestroyed(true);
            break;
        default:
            break;
        }
    }

    for(LVL_Npc *x : target_npcs)
    {
        if(!x) continue;
        if(x->isPaused()) continue; //Don't attack NPC with paused physics!
        if(x->isKilled()) continue; //Also, don't attack dead NPCs, we are not hunting for zombies!
        if(x->m_isGenerator) continue;//Generator is a saint Pagan God, don't touch it!
        switch(action)
        {
        case AttackType_Hit:
            x->doHarm(type, this);
            //TODO: Play this effect via scripts!
            //m_scene->launchStaticEffectC(75, area.center().x(), area.center().y(), 1, 0, 0, 0, 0);
            break;
        case AttackType_ForceDestroy:
            kill_npc(x, NPC_Kicked);
            break;
        }
    }
}

void LVL_Player::lua_attackArea(double left, double top, double right, double bottom, int action, int type, luabind::adl::object filters)
{
    int ltype = luabind::type(filters);
    std::vector<int> x_filters;
    int tfilters = F_LVLBlock|F_LVLNPC;
    if(ltype != LUA_TNIL)
    {
        if(ltype != LUA_TTABLE)
        {
            luaL_error(filters.interpreter(), "installPlayerInAreaDetector exptected int-array, got %s", lua_typename(filters.interpreter(), ltype));
            return;
        }
        x_filters = luabind_utils::convArrayTo<int>(filters);
        for(int &filter : x_filters)
        {
            switch(filter)
            {
            case 1:
                tfilters |= F_LVLBlock;
                break;
            case 2:
                tfilters |= F_LVLBGO;
                break;
            case 3:
                tfilters |= F_LVLNPC;
                break;
            case 4:
                tfilters |= F_LVLPlayer;
                break;
            }
        }
    }
    PGE_RectF r;
    r.setLeft(left);
    r.setTop(top);
    r.setRight(right);
    r.setBottom(bottom);
    attackArea(r, action, type, tfilters);
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


