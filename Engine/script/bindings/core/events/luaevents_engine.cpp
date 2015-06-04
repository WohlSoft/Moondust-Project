#include "luaevents_engine.h"
//#include "../../../luaengine.h"



LuaEvent BindingCore_Events_Engine::createInitEngineEvent(LuaEngine *engine)
{
    LuaEvent event = LuaEvent(engine);
    event.setEventName("initEngine");
    return event;
}
