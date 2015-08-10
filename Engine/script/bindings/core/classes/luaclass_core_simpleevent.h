#ifndef BINDING_LEVEL_CLASS_SIMPLEEVENT_H
#define BINDING_LEVEL_CLASS_SIMPLEEVENT_H

#include <luabind/luabind.hpp>
#include <lua_inclues/lua.hpp>

class Binding_Core_Class_SimpleEvent
{
public:
    Binding_Core_Class_SimpleEvent();
    Binding_Core_Class_SimpleEvent(luabind::object dataObj, bool cancellable = false);

    void setDataObject(luabind::object dataObj);
    luabind::object getDataObject();
    bool isCancellable();
    void setCancelled(bool cancelled, lua_State* L);
    bool getCancelled();
    static luabind::scope bindToLua();

private:
    luabind::object eventData;
    bool eventCancellable;
    bool eventCancelled;
};

using SimpleLuaEvent = Binding_Core_Class_SimpleEvent;

#endif // BINDING_LEVEL_CLASS_SIMPLEEVENT_H
