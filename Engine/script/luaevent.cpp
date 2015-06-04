#include "luaevent.h"
#include "luaengine.h"

lua_State *LuaEvent::getNativeState()
{
    m_engine->getNativeState();
}


LuaEvent::LuaEvent(LuaEngine *engine)
{
    m_engine = engine;
}

QString LuaEvent::eventName() const
{
    return m_eventName;
}

void LuaEvent::setEventName(const QString &eventName)
{
    m_eventName = eventName;
}
