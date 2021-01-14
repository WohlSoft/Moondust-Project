/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <common_features/fmt_format_ne.h>

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

LuaEvent BindingCore_Events_Engine::createLoadEvent(LuaEngine *engine)
{
    LuaEvent event = LuaEvent(engine);
    event.setEventName("load");
    return event;
}

LuaEvent BindingCore_Events_Engine::createStartEvent(LuaEngine *engine)
{
    LuaEvent event = LuaEvent(engine);
    event.setEventName("start");
    return event;
}

LuaEvent BindingCore_Events_Engine::createTickEvent(LuaEngine *engine, double ticks)
{
    LuaEvent event = LuaEvent(engine);
    event.setEventName("tick");
    event.addParameter(ticks);
    return event;
}

LuaEvent BindingCore_Events_Engine::createTickEndEvent(LuaEngine *engine, double ticks)
{
    LuaEvent event = LuaEvent(engine);
    event.setEventName("tickEnd");
    event.addParameter(ticks);
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

LuaEvent BindingCore_Events_Engine::createSaveEvent(LuaEngine *engine)
{
    LuaEvent event = LuaEvent(engine);
    event.setEventName("save");
    return event;
}

LuaEvent BindingCore_Events_Engine::createQuitEvent(LuaEngine *engine, int exitCode)
{
    LuaEvent event = LuaEvent(engine);
    event.setEventName("quit");
    event.addParameter(exitCode);
    return event;
}

LuaEvent BindingCore_Events_Engine::createExitLevelEvent(LuaEngine *engine, int exitCode)
{
    LuaEvent event = LuaEvent(engine);
    event.setEventName("exitLevel");
    event.addParameter(exitCode);
    return event;
}

LuaEvent BindingCore_Events_Engine::createJumpEvent(LuaEngine *engine, int playerIndex)
{
    LuaEvent event = LuaEvent(engine);
    event.setEventName("jump");
    event.addParameter(playerIndex);
    return event;
}

LuaEvent BindingCore_Events_Engine::createJumpEndEvent(LuaEngine *engine, int playerIndex)
{
    LuaEvent event = LuaEvent(engine);
    event.setEventName("jumpEnd");
    event.addParameter(playerIndex);
    return event;
}

LuaEvent BindingCore_Events_Engine::createKeyDownEvent(LuaEngine *engine, int keyCode, int playerIndex)
{
    LuaEvent event = LuaEvent(engine);
    event.setEventName("keyDown");
    event.addParameter(keyCode);
    event.addParameter(playerIndex);
    return event;
}

LuaEvent BindingCore_Events_Engine::createKeyUpEvent(LuaEngine *engine, int keyCode, int playerIndex)
{
    LuaEvent event = LuaEvent(engine);
    event.setEventName("keyUp");
    event.addParameter(keyCode);
    event.addParameter(playerIndex);
    return event;
}

LuaEvent BindingCore_Events_Engine::createInputUpdateEvent(LuaEngine *engine)
{
    LuaEvent event = LuaEvent(engine);
    event.setEventName("inputUpdate");
    return event;
}

LuaEvent BindingCore_Events_Engine::createMessageBoxEvent(LuaEngine *engine,
                                                          Binding_Core_Class_SimpleEvent *eventContext,
                                                          const std::string &messageText)
{
    LuaEvent event = LuaEvent(engine);
    event.setEventName("messageBox");
    event.addParameter(eventContext);
    event.addParameter(messageText);
    return event;
}

LuaEvent BindingCore_Events_Engine::createEventEvent(LuaEngine *engine, const std::string &eventName)
{
    LuaEvent event = LuaEvent(engine);
    event.setEventName("event");
    event.addParameter(eventName);
    return event;
}

LuaEvent BindingCore_Events_Engine::createEventDirectEvent(LuaEngine *engine,
                                                           Binding_Core_Class_SimpleEvent *eventContext,
                                                           const std::string &eventName)
{
    LuaEvent event = LuaEvent(engine);
    event.setEventName("eventDirect");
    event.addParameter(eventContext);
    event.addParameter(eventName);
    return event;
}

LuaEvent BindingCore_Events_Engine::createCameraUpdateEvent(LuaEngine *engine,
                                                            Binding_Core_Class_SimpleEvent *eventContext,
                                                            int cameraIndex)
{
    LuaEvent event = LuaEvent(engine);
    event.setEventName("cameraUpdate");
    event.addParameter(eventContext);
    event.addParameter(cameraIndex);
    return event;
}

LuaEvent BindingCore_Events_Engine::createCameraUpdateDirectEvent(LuaEngine *engine,
                                                                  Binding_Core_Class_SimpleEvent *eventContext,
                                                                  PGE_LevelCamera *camera)
{
    LuaEvent event = LuaEvent(engine);
    event.setEventName("cameraUpdateDirect");
    event.addParameter(eventContext);
    event.addParameter(camera);
    return event;
}

LuaEvent BindingCore_Events_Engine::createLoopSectionEvent(LuaEngine *engine, int sectionId, int playerIndex)
{
    LuaEvent event = LuaEvent(engine);
    event.setEventName("loopSection");
    event.addParameter(sectionId);
    event.addParameter(playerIndex);
    return event;
}

LuaEvent BindingCore_Events_Engine::createLoopSectionXEvent(LuaEngine *engine, int sectionId, int playerIndex)
{
    LuaEvent event = LuaEvent(engine);
    event.setEventName(fmt::format("loopSection{0}", sectionId));
    event.addParameter(playerIndex);
    return event;
}

LuaEvent BindingCore_Events_Engine::createLoadSectionEvent(LuaEngine *engine, int sectionId, int playerIndex)
{
    LuaEvent event = LuaEvent(engine);
    event.setEventName("loadSection");
    event.addParameter(sectionId);
    event.addParameter(playerIndex);
    return event;
}

LuaEvent BindingCore_Events_Engine::createLoadSectionXEvent(LuaEngine *engine, int sectionId, int playerIndex)
{
    LuaEvent event = LuaEvent(engine);
    event.setEventName(fmt::format("loadSection{0}", sectionId));
    event.addParameter(playerIndex);
    return event;
}

LuaEvent BindingCore_Events_Engine::createKeyboardPressEvent(LuaEngine *engine, int vkCode)
{
    LuaEvent event = LuaEvent(engine);
    event.setEventName("keyboardPress");
    event.addParameter(vkCode);
    return event;
}

LuaEvent BindingCore_Events_Engine::createKeyboardPressDirectEvent(LuaEngine *engine,
                                                                   Binding_Core_Class_SimpleEvent *eventContext,
                                                                   int vkCode)
{
    LuaEvent event = LuaEvent(engine);
    event.setEventName("keyboardPressDirect");
    event.addParameter(eventContext);
    event.addParameter(vkCode);
    return event;
}

LuaEvent BindingCore_Events_Engine::createDrawEvent(LuaEngine *engine)
{
    LuaEvent event = LuaEvent(engine);
    event.setEventName("draw");
    return event;
}

LuaEvent BindingCore_Events_Engine::createDrawEndEvent(LuaEngine *engine)
{
    LuaEvent event = LuaEvent(engine);
    event.setEventName("drawEnd");
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
