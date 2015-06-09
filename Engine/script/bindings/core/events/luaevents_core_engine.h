#ifndef BINDINGCORE_EVENTS_ENGINE_H
#define BINDINGCORE_EVENTS_ENGINE_H

#include "../../../lua_event.h"

class BindingCore_Events_Engine
{
public:
    static LuaEvent createInitEngineEvent(LuaEngine* engine);
    static LuaEvent createLoopEvent(LuaEngine* engine, float ticks);
};

#endif // BINDINGCORE_EVENTS_ENGINE_H
