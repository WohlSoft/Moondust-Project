/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2025 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef LUAEVENT_H
#define LUAEVENT_H

#include <luabind/luabind.hpp>
#include <lua_includes/lua.hpp>

class LuaEngine;

class LuaEvent{

    friend class LuaEngine;
private:

    std::string m_eventName;
    LuaEngine* m_engine;

    lua_State* getNativeState();

protected:
    std::vector<luabind::object> objList;
public:
    LuaEvent(LuaEngine* engine);
    std::string eventName() const;
    void setEventName(const std::string &eventName);

    template<typename T>
    void addParameter(T par)
    {
        objList.emplace_back(getNativeState(), par);
    }
    void cleanupAllParams();

    static luabind::scope bindToLua();

};

#endif // LUAEVENT_H


