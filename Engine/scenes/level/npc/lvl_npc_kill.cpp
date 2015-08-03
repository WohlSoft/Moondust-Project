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
    health-=damage;
    if(health<=0)
    {
        kill(damageReason);
        LvlSceneP::s->launchStaticEffectC(setup->effect_1, posCenterX(), posCenterY(), 1, 0, 0, 0, 0, _direction);
    }
    else
        PGE_Audio::playSound(39);
}

void LVL_Npc::kill(int damageReason, bool nolua)
{
    if(!nolua)
    {
        try{
            lua_onKill(damageReason);
        } catch (luabind::error& e) {
            LvlSceneP::s->getLuaEngine()->postLateShutdownError(e);
        }
    }

    killed=true;
    //sct()->unregisterElement(this);
    LvlSceneP::s->unregisterElement(this);
    LvlSceneP::s->dead_npcs.push_back(this);

    if(!data.event_die.isEmpty())
        LvlSceneP::s->events.triggerEvent(data.event_die);
    LvlSceneP::s->layers.removeRegItem(data.layer, this);
    if(!data.event_emptylayer.isEmpty())
    {
        if(LvlSceneP::s->layers.isEmpty(data.layer))
            LvlSceneP::s->events.triggerEvent(data.event_emptylayer);
    }
}

