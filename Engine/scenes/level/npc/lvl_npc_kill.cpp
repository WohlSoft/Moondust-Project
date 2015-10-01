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

#include "../lvl_npc.h"
#include "../lvl_scene_ptr.h"

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
        lua_onHarm(damage, damageReason);
    } catch (luabind::error& e) {
        LvlSceneP::s->getLuaEngine()->postLateShutdownError(e);
    }

    health -= damage;
    if(health <= 0)
    {
        kill(damageReason);
        if(!setup->container || setup->contents_id == 0)
            LvlSceneP::s->launchStaticEffectC(setup->effect_1, posCenterX(), posCenterY(), 1, 250, 0, 0, 0, _direction);
        else
        {
            //Spawn the contents of the dead NPC's container
            if (damageReason == LVL_Npc::DAMAGE_STOMPED)
            {
                LevelNPC def = data;
                def.id=setup->contents_id;
                def.x=posX();
                def.y=posY();
                LvlSceneP::s->placeNPC(def);
            }
        }
    }
    else
        PGE_Audio::playSound(39);
}

void LVL_Npc::talkWith()
{
    if(data.msg.isEmpty()) return;
    PGE_MsgBox msgX(LvlSceneP::s, data.msg, PGE_MsgBox::msg_info, PGE_Point(-1,-1),
               ConfigManager::setup_message_box.box_padding,
               ConfigManager::setup_message_box.sprite);
    msgX.exec();
    if(!data.event_talk.isEmpty())
    {
        LvlSceneP::s->events.triggerEvent(data.event_talk);
    }
}


void LVL_Npc::kill(int damageReason)
{
    try{
        lua_onKill(damageReason);
    } catch (luabind::error& e) {
        LvlSceneP::s->getLuaEngine()->postLateShutdownError(e);
    }


    //Pre-unregistring event
    if(!data.event_die.isEmpty())
        LvlSceneP::s->events.triggerEvent(data.event_die);

    unregister();

    //Post-unregistring event
    if(!data.event_emptylayer.isEmpty())
    {
        if(LvlSceneP::s->layers.isEmpty(data.layer))
            LvlSceneP::s->events.triggerEvent(data.event_emptylayer);
    }

}

void LVL_Npc::unregister()
{
    killed=true;
    LvlSceneP::s->unregisterElement(this);
    LvlSceneP::s->dead_npcs.push_back(this);
    LvlSceneP::s->layers.removeRegItem(data.layer, this);
}

