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

#include "../lvl_npc.h"
#include "../../scene_level.h"

#include <audio/pge_audio.h>

bool LVL_Npc::isKilled()
{
    return killed;
}

void LVL_Npc::doHarm(int damageReason)
{
    int damageLevel=1;
    //call Lua
    //onHarm(killReason, out_DamageLevel)

    harm(damageLevel, damageReason);
}

void LVL_Npc::harm(int damage, int damageReason)
{
    try {
        HarmEvent event;
        event.damage=damage;
        event.reason_code=damageReason;
        lua_onHarm(&event);
        if(event.cancel) return;
        damage = event.damage;
    } catch (luabind::error& e) {
        m_scene->getLuaEngine()->postLateShutdownError(e);
        return;
    }

    health -= damage;
    if(health <= 0)
    {
        if( setup->setup.contents_id == 0)
        {
            kill(damageReason);
        }
        else
        {
            //Spawn the contents of the dead NPC's container
            if (damageReason == LVL_Npc::DAMAGE_STOMPED)
                transformTo_x(setup->setup.contents_id);
            else
                kill(damageReason);
        }
    }
    else
    {
        if(setup->setup.hit_sound_id==0)
            PGE_Audio::playSoundByRole(obj_sound_role::NpcHit);
        else
            PGE_Audio::playSound(setup->setup.hit_sound_id);
    }
}

void LVL_Npc::talkWith()
{
    if(data.msg.empty())
        return;
    m_scene->m_messages.showMsg( data.msg );
    if(!data.event_talk.empty())
    {
        m_scene->m_events.triggerEvent(data.event_talk);
    }
}


void LVL_Npc::kill(int damageReason)
{
    if((damageReason == DAMAGE_LAVABURN) && (setup->setup.lava_protect)) return;

    try{
        KillEvent event;
        event.reason_code=damageReason;
        lua_onKill(&event);
        if(event.cancel) return;
    } catch (luabind::error& e) {
        m_scene->getLuaEngine()->postLateShutdownError(e);
        return;
    }

    //Pre-unregistring event
    if(!data.event_die.empty())
        m_scene->m_events.triggerEvent(data.event_die);

    unregister();

    switch(damageReason)
    {
        case DAMAGE_NOREASON:
            if(setup->setup.effect_1 > 0)
            {
                m_scene->launchStaticEffectC(setup->setup.effect_1,
                                            posCenterX(),
                                            posCenterY(),
                                            1, 0, 0, 0, 0,
                                            _direction);
            }
        break;
        case DAMAGE_STOMPED:
            if(setup->setup.effect_1 > 0)
            {
                m_scene->launchStaticEffectC(setup->setup.effect_1,
                                            posCenterX(),
                                            posCenterY(),
                                            0, 500, 0, 0, 0,
                                            _direction);
            }
        break;
        case DAMAGE_TAKEN:
            if(setup->setup.effect_1 > 0)
            {
                m_scene->launchStaticEffectC(setup->setup.effect_1,
                                            posCenterX(),
                                            posCenterY(),
                                            1, 0, 0, 0, 0,
                                            _direction);
            }
        break;
        case DAMAGE_LAVABURN:
            if(ConfigManager::g_setup_npc.eff_lava_burn>0)
            {
                m_scene->launchStaticEffectC(ConfigManager::g_setup_npc.eff_lava_burn,
                                            posCenterX(),
                                            posCenterY(),
                                            1, 0, 0, 0, 0,
                                            _direction);
            }
        break;
        case DAMAGE_PITFALL:
            //Don't spawn effects
        break;
        case DAMAGE_BY_KICK:
            if( setup->setup.effect_2 > 0 )
            {
                m_scene->launchStaticEffectC(setup->setup.effect_2,
                                            posCenterX(),
                                            posCenterY(),
                                            0, 5000, -3.0*_direction, -6.0, 18.0,
                                            _direction);
            }
        break;
        //case DAMAGE_BY_PLAYER_ATTACK:
        default:
            {
                SpawnEffectDef eff = ConfigManager::g_setup_effects.m_smoke;
                eff.startX = posCenterX();
                eff.startY = posCenterY();
                eff.direction = _direction;
                m_scene->launchEffect(eff, true);
            }
        break;
    }

    if(damageReason != DAMAGE_LAVABURN)
    {
        if(setup->setup.death_sound_id==0)
            PGE_Audio::playSoundByRole(obj_sound_role::NpcDeath);
        else
        {
            if(setup->setup.death_sound_id>0)
                PGE_Audio::playSound(setup->setup.death_sound_id);
        }
    } else {
        PGE_Audio::playSoundByRole(obj_sound_role::NpcLavaBurn);
    }

    //Post-unregistring event
    if(!data.event_emptylayer.empty())
    {
        if(m_scene->m_layers.isEmpty(data.layer))
            m_scene->m_events.triggerEvent(data.event_emptylayer);
    }

}

void LVL_Npc::unregister()
{
    killed=true;
    m_is_visible=false;
    unregisterFromTree();
    m_scene->m_npcDead.push_back(this);
    m_scene->m_layers.removeRegItem(data.layer, this);
    if(transformedFromBlock)
    {
        //Untegister the source block from all trees
        m_scene->m_layers.removeRegItem(transformedFromBlock->data.layer, transformedFromBlock);
        m_scene->m_blocksDestroyed.erase(transformedFromBlock);
        m_scene->unregisterElement(transformedFromBlock);
    }
}

