#ifndef BINDINGCORE_EVENTS_ENGINE_H
#define BINDINGCORE_EVENTS_ENGINE_H

#include "../../../luaevent.h"

class BindingCore_Events_Engine
{
public:
    static LuaEvent createInitEngineEvent(LuaEngine* engine);
};

#endif // BINDINGCORE_EVENTS_ENGINE_H
