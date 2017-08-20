#include "luaclass_core_simpleevent.h"

Binding_Core_Class_SimpleEvent::Binding_Core_Class_SimpleEvent() : eventData(), eventCancellable(false), eventCancelled(false)
{}

Binding_Core_Class_SimpleEvent::Binding_Core_Class_SimpleEvent(luabind::adl::object dataObj, bool cancellable) : eventData(dataObj), eventCancellable(cancellable), eventCancelled(false)
{}

void Binding_Core_Class_SimpleEvent::setDataObject(luabind::adl::object dataObj)
{
    dataObj.swap(dataObj);
}

luabind::adl::object Binding_Core_Class_SimpleEvent::getDataObject()
{
    return eventData;
}

bool Binding_Core_Class_SimpleEvent::isCancellable()
{
    return eventCancellable;
}

void Binding_Core_Class_SimpleEvent::setCancelled(bool cancelled, lua_State *L)
{
    if(!eventCancellable)
        luaL_error(L, "Trying to set the cancelled property on a non-cancellable event!");
    else
        eventCancelled = cancelled;
}

bool Binding_Core_Class_SimpleEvent::getCancelled()
{
    return eventCancelled;
}

/***
Simple utility class for a custom events
@module SimpleEventClass
*/

luabind::scope Binding_Core_Class_SimpleEvent::bindToLua()
{
    using namespace luabind;
    /***
    Simple utility class for a custom events
    @type SimpleLuaEvent
    */
    return class_<Binding_Core_Class_SimpleEvent>("SimpleLuaEvent")
            /***
            User data
            @tfield object data
            */
            .property("data", &Binding_Core_Class_SimpleEvent::getDataObject)
            /***
            Is this event cancellable
            @tfield bool cancellable true or false
            */
            .property("cancellable", &Binding_Core_Class_SimpleEvent::isCancellable)
            /***
            Was this event cancelled
            @tfield bool cancelled true or false
            */
            .property("cancelled", &Binding_Core_Class_SimpleEvent::getCancelled, &Binding_Core_Class_SimpleEvent::setCancelled);
}

