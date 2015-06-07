#include "luaevent.h"
#include "luaengine.h"

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

void LuaEvent::bindToLua(lua_State *L)
{
    using namespace luabind;
    module(L)[
        class_<LuaEvent>("LuaEvent")
            .property("eventName", &LuaEvent::eventName, &LuaEvent::setEventName)
        ];
}
