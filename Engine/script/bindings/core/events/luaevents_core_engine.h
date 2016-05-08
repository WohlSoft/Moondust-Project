#ifndef BINDINGCORE_EVENTS_ENGINE_H
#define BINDINGCORE_EVENTS_ENGINE_H

#include "../../../lua_event.h"
#include <scenes/level/lvl_camera.h>
#include "../../level/classes/luaclass_level_playerstate.h"

class BindingCore_Events_Engine
{
public:
    static LuaEvent createInitEngineEvent(LuaEngine* engine);
    static LuaEvent createLoopEvent(LuaEngine* engine, float ticks);

    static LuaEvent createDrawLevelHUDEvent(LuaEngine* engine,
                                            PGE_LevelCamera* camera,
                                            lua_LevelPlayerState *playerState);
};

#endif // BINDINGCORE_EVENTS_ENGINE_H
