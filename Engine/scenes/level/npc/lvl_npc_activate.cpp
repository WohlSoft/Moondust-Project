/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "../lvl_npc.h"
#include "../../scene_level.h"

void LVL_Npc::Activate()
{
    if(is_activity)
        activationTimeout = setup->setup.deactivationDelay;
    else
        activationTimeout = 150;

    if(isActivated)
        return;

    setPaused(false);
    deActivatable = ((setup->setup.deactivation) || (!setup->setup.activity)); //!< Allow deactivation of this NPC when it going offscreen
    wasDeactivated = false;

    if(m_spawnedGeneratorType != LevelScene::GENERATOR_PROJECTILE)
        setSpeed(0.0, 0.0);

    animator.start();
    isActivated = true;

    if(is_layer_unstickable)
        m_scene->m_layers.setItemMovable(m_scene->m_layers.getLayer(data.layer), this, false, true);

    if(!data.event_activate.empty())
        m_scene->m_events.triggerEvent(data.event_activate);

    if(isLuaNPC)
    {
        try
        {
            lua_onActivated();
        }
        catch(luabind::error &e)
        {
            m_scene->getLuaEngine()->postLateShutdownError(e);
        }
    }

    lua_activate_neighbours();//Also activate neighours
}

void LVL_Npc::deActivate()
{
    if(!wasDeactivated)
        wasDeactivated = true;

    if(!isActivated) return;

    isActivated = false;

    if(!is_shared_animation)
        animator.stop();

    if(!keep_position_on_despawn)
    {
        if(!reSpawnable)
            unregister();
        else
        {
            if(data.id != _npc_id)
            {
                transformTo_x(data.id); //Transform NPC back into initial form
            }
            setDefaults();
            setRelativePos(data.x, data.y);
            setDirection(data.direct);
            setPaused(true);
            m_momentum.saveOld();
        }
    }
}

