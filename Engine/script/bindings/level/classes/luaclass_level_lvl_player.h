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

#ifndef LUACLASS_CORE_LVL_PLAYER_H
#define LUACLASS_CORE_LVL_PLAYER_H

#include <lua_includes/lua.hpp>
#include <luabind/luabind.hpp>

#include <scenes/level/lvl_player.h>

class Binding_Level_ClassWrapper_LVL_Player : public LVL_Player, public luabind::wrap_base
{
    public:
        Binding_Level_ClassWrapper_LVL_Player();
        virtual ~Binding_Level_ClassWrapper_LVL_Player();

        virtual void lua_onInit();
        virtual void lua_onLoop(double tickTime);
        virtual void lua_onHarm(LVL_Player_harm_event *harmevent);
        virtual void lua_onTransform(unsigned long character, unsigned long state);
        virtual void lua_onTakeNpc(LVL_Npc *npc);
        virtual void lua_onKillNpc(LVL_Npc *npc);
        virtual void lua_onKeyPressed(ControllableObject::KeyType ktype);
        virtual void lua_onKeyReleased(ControllableObject::KeyType ktype);
        static void def_lua_onInit(LVL_Player *base)
        {
            base->LVL_Player::lua_onInit();
        }
        static void def_lua_onLoop(LVL_Player *base, double tickTime)
        {
            base->LVL_Player::lua_onLoop(tickTime);
        }
        static void def_lua_onHarm(LVL_Player *base, LVL_Player_harm_event *harmevent)
        {
            base->LVL_Player::lua_onHarm(harmevent);
        }
        static void def_lua_onTransform(LVL_Player *base, unsigned long character, unsigned long state)
        {
            base->LVL_Player::lua_onTransform(character, state);
        }
        static void def_lua_onTakeNpc(LVL_Player *base, LVL_Npc *npc)
        {
            base->LVL_Player::lua_onTakeNpc(npc);
        }
        static void def_lua_onKillNpc(LVL_Player *base, LVL_Npc *npc)
        {
            base->LVL_Player::lua_onKillNpc(npc);
        }
        static void def_lua_onKeyPressed(LVL_Player *base, ControllableObject::KeyType ktype)
        {
            base->LVL_Player::lua_onKeyPressed(ktype);
        }
        static void def_lua_onKeyReleased(LVL_Player *base, ControllableObject::KeyType ktype)
        {
            base->LVL_Player::lua_onKeyReleased(ktype);
        }

        static luabind::scope bindToLua();
        static luabind::scope HarmEvent_bindToLua();
    private:
        luabind::detail::wrap_access mself;
};

#endif // LUACLASS_CORE_LVL_PLAYER_H
