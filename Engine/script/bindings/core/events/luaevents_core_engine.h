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

#ifndef BINDINGCORE_EVENTS_ENGINE_H
#define BINDINGCORE_EVENTS_ENGINE_H

#include "../../../lua_event.h"
#include <scenes/level/lvl_camera.h>
#include "../../level/classes/luaclass_level_playerstate.h"
#include "../../level/classes/luaclass_level_lvl_player.h"
#include "../classes/luaclass_core_simpleevent.h"

class BindingCore_Events_Engine
{
public:
    static LuaEvent createInitEngineEvent(LuaEngine* engine);

    static LuaEvent createLoadEvent(LuaEngine* engine); /*TODO: Use it*/
    static LuaEvent createStartEvent(LuaEngine* engine); /*TODO: Use it*/

    static LuaEvent createTickEvent(LuaEngine* engine, double ticks); /*TODO: Use it*/
    static LuaEvent createTickEndEvent(LuaEngine* engine, double ticks); /*TODO: Use it*/
    static LuaEvent createLoopEvent(LuaEngine* engine, double ticks);

    static LuaEvent createSaveEvent(LuaEngine* engine); /*TODO: Use it*/

    static LuaEvent createQuitEvent(LuaEngine* engine, int exitCode); /*TODO: Use it*/
    static LuaEvent createExitLevelEvent(LuaEngine* engine, int exitCode); /*TODO: Use it*/

    static LuaEvent createJumpEvent(LuaEngine* engine, int playerIndex); /*TODO: Use it*/
    static LuaEvent createJumpEndEvent(LuaEngine* engine, int playerIndex); /*TODO: Use it*/

    static LuaEvent createKeyDownEvent(LuaEngine* engine, int keyCode, int playerIndex); /*TODO: Use it*/
    static LuaEvent createKeyUpEvent(LuaEngine* engine, int keyCode, int playerIndex); /*TODO: Use it*/

    static LuaEvent createInputUpdateEvent(LuaEngine* engine); /*TODO: Use it*/

    static LuaEvent createMessageBoxEvent(LuaEngine* engine,
                                          Binding_Core_Class_SimpleEvent *event,
                                          const std::string &messageText); /*TODO: Use it*/

    static LuaEvent createEventEvent(LuaEngine* engine, const std::string &eventName); /*TODO: Use it*/
    static LuaEvent createEventDirectEvent(LuaEngine* engine,
                                           Binding_Core_Class_SimpleEvent *event,
                                           const std::string &eventName); /*TODO: Use it*/

    static LuaEvent createCameraUpdateEvent(LuaEngine* engine,
                                            Binding_Core_Class_SimpleEvent *event,
                                            int cameraIndex); /*TODO: Use it*/

    static LuaEvent createCameraUpdateDirectEvent(LuaEngine* engine,
                                                  Binding_Core_Class_SimpleEvent *event,
                                                  PGE_LevelCamera *camera); /*TODO: Use it*/

    static LuaEvent createLoopSectionEvent(LuaEngine* engine, int sectionId, int playerIndex); /*TODO: Use it*/
    static LuaEvent createLoopSectionXEvent(LuaEngine* engine, int sectionId, int playerIndex); /*TODO: Use it*/
    static LuaEvent createLoadSectionEvent(LuaEngine* engine, int sectionId, int playerIndex); /*TODO: Use it*/
    static LuaEvent createLoadSectionXEvent(LuaEngine* engine, int sectionId, int playerIndex); /*TODO: Use it*/

    static LuaEvent createKeyboardPressEvent(LuaEngine* engine, int vkCode); /*TODO: Use it*/
    static LuaEvent createKeyboardPressDirectEvent(LuaEngine* engine,
                                                   Binding_Core_Class_SimpleEvent *event,
                                                   int vkCode); /*TODO: Use it*/

    static LuaEvent createDrawEvent(LuaEngine* engine); /*TODO: Use it*/
    static LuaEvent createDrawEndEvent(LuaEngine* engine); /*TODO: Use it*/
    static LuaEvent createDrawLevelHUDEvent(LuaEngine* engine,
                                            PGE_LevelCamera* camera,
                                            lua_LevelPlayerState *playerState); /*TODO: Use it*/
};

#endif // BINDINGCORE_EVENTS_ENGINE_H
