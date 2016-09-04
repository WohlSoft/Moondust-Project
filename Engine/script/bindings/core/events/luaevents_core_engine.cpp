#include "luaevents_core_engine.h"



LuaEvent BindingCore_Events_Engine::createInitEngineEvent(LuaEngine *engine)
{
    LuaEvent event = LuaEvent(engine);
    event.setEventName("initEngine");
    return event;
}

LuaEvent BindingCore_Events_Engine::createLoopEvent(LuaEngine *engine, double ticks)
{
    LuaEvent event = LuaEvent(engine);
    event.setEventName("loop");
    event.addParameter(ticks);
    return event;
}

LuaEvent BindingCore_Events_Engine::createDrawLevelHUDEvent(LuaEngine *engine,
                                                            PGE_LevelCamera *camera,
                                                            lua_LevelPlayerState *playerState)
{
    LuaEvent event = LuaEvent(engine);
    event.setEventName("drawHUD");
    event.addParameter( camera );
    event.addParameter( playerState );
    return event;
}
