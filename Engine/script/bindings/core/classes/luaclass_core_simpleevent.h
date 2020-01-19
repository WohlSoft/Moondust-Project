/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#ifndef BINDING_LEVEL_CLASS_SIMPLEEVENT_H
#define BINDING_LEVEL_CLASS_SIMPLEEVENT_H

#include <luabind/luabind.hpp>
#include <lua_includes/lua.hpp>

class Binding_Core_Class_SimpleEvent
{
public:
    Binding_Core_Class_SimpleEvent();
    Binding_Core_Class_SimpleEvent(luabind::object dataObj, bool cancellable = false);

    void setDataObject(luabind::object dataObj);
    luabind::object getDataObject();
    std::string getEventName();
    bool isCancellable();
    void setCancelled(bool cancelled, lua_State* L);
    bool getCancelled();
    static luabind::scope bindToLua();

private:
    luabind::object eventData;
    std::string eventName;
    bool eventCancellable;
    bool eventCancelled;
};

using SimpleLuaEvent = Binding_Core_Class_SimpleEvent;

#endif // BINDING_LEVEL_CLASS_SIMPLEEVENT_H
