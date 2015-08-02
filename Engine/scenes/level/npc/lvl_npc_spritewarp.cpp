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
#include <common_features/number_limiter.h>

void LVL_Npc::setSpriteWarp(float depth, LVL_Npc::WarpingSide direction, bool resizedBody)
{
    NumberLimiter::apply(depth, 0.0f, 1.0f);
    isWarping=(depth>0.0f);
    warpSpriteOffset=depth;
    warpResizedBody=resizedBody;
    warpDirectO=direction;
}

void LVL_Npc::resetSpriteWarp()
{
    warpSpriteOffset=0.0f;
    warpResizedBody=false;
    isWarping=false;
}

void LVL_Npc::setWarpSpawn(LVL_Npc::WarpingSide side)
{
    setSpriteWarp(1.0f, side);
    warpSpawing=true;
    setSpeed(0.0, 0.0);
    float pStep = 1.5f/PGE_Window::TicksPerSecond;
    EventQueueEntry<LVL_Npc >warpOut;
    warpOut.makeWaiterCond([this, pStep]()->bool{
                            setSpriteWarp(warpSpriteOffset-pStep, (WarpingSide)warpDirectO, false);
                              return warpSpriteOffset<=0.0f;
                          }, false, 0);
    event_queue.events.push_back(warpOut);

    EventQueueEntry<LVL_Npc >endWarping;
    endWarping.makeCaller([this, side]()->void{
                          setSpriteWarp(0.0f, side);
                          warpSpawing=false;
                          last_environment=-1;//!< Forcing to refresh physical environment
                      }, 0);
    event_queue.events.push_back(endWarping);
}


