#include "lua_event.h"
#include "lua_engine.h"

lua_State *LuaEvent::getNativeState()
{
    return m_engine->getNativeState();
}


LuaEvent::LuaEvent(LuaEngine *engine)
{
    m_engine = engine;
}

std::string LuaEvent::eventName() const
{
    return m_eventName;
}

void LuaEvent::setEventName(const std::string &eventName)
{
    m_eventName = eventName;
}

luabind::scope LuaEvent::bindToLua()
{
    using namespace luabind;
    return
        class_<LuaEvent>("LuaEvent")
            .property("eventName", &LuaEvent::eventName, &LuaEvent::setEventName);

}
