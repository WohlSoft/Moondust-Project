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

#include "luaevents_core_engine.h"

/***
Callback events called by engine. There are must be defined in your script files where you want to handle those events.
@module EventCallbacks
*/

/***
Global events
@section Events
*/

/***
Event callback once called on Lua engine initialization
@function onInitEngine

@usage
function onInitEngine()
    -- Initialize your stuff if needed here
end
*/
LuaEvent BindingCore_Events_Engine::createInitEngineEvent(LuaEngine *engine)
{
    LuaEvent event = LuaEvent(engine);
    event.setEventName("initEngine");
    return event;
}

/***
Event callback calling per every frame
@function onLoop
@tparam double frameDelay Frame delay in milliseconds. Use it for various timing processors.

@usage
-- A timer value
local timer = 0

-- Play sound every one second
function ticker(frameDelay)
    -- Iterate a timer
    timer = timer + frameDelay
    -- Check if timer reaches one whole second
    if(timer >= 1000)then
        -- Play a tick sound
        Audio.playSoundByRole(SoundRoles.MenuScroll)
        -- Reset timer to zero without lost of timing accuracy
        timer = timer - 1000
    end
end

function onLoop(frameDelay)
    ticker(frameDelay)
end
*/
LuaEvent BindingCore_Events_Engine::createLoopEvent(LuaEngine *engine, double ticks)
{
    LuaEvent event = LuaEvent(engine);
    event.setEventName("loop");
    event.addParameter(ticks);
    return event;
}

/***
Event callback calling per every frame after all in-game objects are drawn (Levels Only)
@function onDrawHUD
@tparam LevelCommon.LevelCamera camera Camera where need to show HUD
@tparam LevelCommon.LevelPlayerState playerState State of playable character

@usage
function onDrawHUD(camera, state)
    -- Write here your code to draw HUD based on given or any other information
end
*/
LuaEvent BindingCore_Events_Engine::createDrawLevelHUDEvent(LuaEngine *engine,
        PGE_LevelCamera *camera,
        lua_LevelPlayerState *playerState)
{
    LuaEvent event = LuaEvent(engine);
    event.setEventName("drawHUD");
    event.addParameter(camera);
    event.addParameter(playerState);
    return event;
}
