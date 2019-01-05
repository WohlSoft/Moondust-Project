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

#include "../lvl_npc.h"
#include "../../scene_level.h"
#include <common_features/number_limiter.h>

void LVL_Npc::setSpriteWarp(float depth, LVL_Npc::WarpingSide direction, bool resizedBody)
{
    NumberLimiter::apply(depth, 0.0f, 1.0f);
    isWarping = (depth > 0.0f);
    warpSpriteOffset = depth;
    warpResizedBody = resizedBody;
    warpDirectO = direction;
}

void LVL_Npc::resetSpriteWarp()
{
    warpSpriteOffset = 0.0f;
    warpResizedBody = false;
    isWarping = false;
}

void LVL_Npc::setWarpSpawn(LVL_Npc::WarpingSide side)
{
    setSpriteWarp(1.0f, side);
    warpSpawing = true;
    double origGravityScale = gravityScale();
    setSpeed(0.0, 0.0);
    setGravityScale(0.0);
    double pStep = 1.5 / (1000.0 / m_scene->frameDelay());
    EventQueueEntry<LVL_Npc >warpOut;
    warpOut.makeWaiterCond([this, pStep]()->bool
    {
        setSpriteWarp(warpSpriteOffset - float(pStep), (WarpingSide)warpDirectO, false);
        return warpSpriteOffset <= 0.0f;
    }, false, 0);
    event_queue.events.push_back(warpOut);

    EventQueueEntry<LVL_Npc >endWarping;
    endWarping.makeCaller([this, side, origGravityScale]()->void
    {
        setSpriteWarp(0.0f, side);
        setGravityScale(origGravityScale);
        warpSpawing = false;
        last_environment = -1; //!< Forcing to refresh physical environment
    }, 0);
    event_queue.events.push_back(endWarping);
}


