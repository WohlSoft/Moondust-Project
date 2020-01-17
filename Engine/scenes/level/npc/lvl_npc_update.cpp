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

#include "../lvl_npc.h"
#include "../lvl_section.h"
#include "../../scene_level.h"

void LVL_Npc::update(double tickTime)
{
    if(killed) return;

    if(wasDeactivated)
        return;

    if(m_isGenerator)
    {
        activationTimeout -= tickTime;
        updateGenerator(tickTime);
        return;
    }

    event_queue.processEvents(tickTime);
    animator.manualTick(tickTime);

    if(warpSpawing)
    {
        setSpeed(0.0, 0.0);
        return;
    }

    PGE_Phys_Object::update(tickTime);

    if(deActivatable)
        activationTimeout -= tickTime;

    if(motionSpeed != 0.0)
    {
        if(m_blockedAtLeft)
            setDirection(1);
        else if(m_blockedAtRight)
            setDirection(-1);
        else if(setup->setup.turn_on_cliff_detect && m_cliff)
            setDirection(_direction * -1);

        setSpeedX(motionSpeed * double(_direction));

        if(motionSpeed < 0.0)
            m_moveLeft = true;
        else if(motionSpeed > 0.0)
            m_moveRight = true;
    }

    if(not_movable())
    {
        detector_player_pos.processDetector();

        if(detector_player_pos.directedTo() != 0)
            setDirection(detector_player_pos.directedTo());
    }

    LVL_Section *section = sct();
    PGE_RectF sBox = section->sectionRect();

    if(offSectionDeactivate)
    {
        if(!sBox.collideRect(m_momentum.x, m_momentum.y, m_momentum.w, m_momentum.h))
        {
            if(activationTimeout > 100)
                activationTimeout = 100;

            //Iterate activation timeout if deactivation disabled by default
            if(!deActivatable)
                activationTimeout -= tickTime;
        }
    }

    if(section->isWrapH())
    {
        if(posX() < sBox.left() - m_momentum.w - 1.0)
            setPosX(sBox.right() - 1.0);
        else if(posX() > sBox.right() + 1.0)
            setPosX(sBox.left() - m_momentum.w + 1.0);
    }

    if(section->isWrapV())
    {
        if(posY() < sBox.top() - m_momentum.h - 1.0)
            setPosY(sBox.bottom() - 1.0);
        else if(posY() > sBox.bottom() + 1.0)
            setPosY(sBox.top() - m_momentum.h + 1.0);
    }
    else if((setup->setup.kill_on_pit_fall) && (posY() > sBox.bottom() + m_momentum.h))
        kill(DAMAGE_PITFALL);

    for(auto i = detectors.begin(); i != detectors.end(); i++)
        (*i)->processDetector();

    try
    {
        lua_onLoop(tickTime);
    }
    catch(luabind::error &e)
    {
        m_scene->getLuaEngine()->postLateShutdownError(e);
    }
}
