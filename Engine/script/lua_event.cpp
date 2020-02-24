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

void LuaEvent::cleanupAllParams()
{
    objList.clear();
}

luabind::scope LuaEvent::bindToLua()
{
    using namespace luabind;
    return
        class_<LuaEvent>("LuaEvent")
            .property("eventName", &LuaEvent::eventName, &LuaEvent::setEventName);
}
