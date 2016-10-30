#ifndef LUACLASS_CORE_LVL_PLAYER_H
#define LUACLASS_CORE_LVL_PLAYER_H

#include <lua_inclues/lua.hpp>
#include <luabind/luabind.hpp>
#include <luabind/function.hpp>
#include <luabind/class.hpp>
#include <luabind/detail/call_function.hpp>
#include <luabind/operator.hpp>

#include <scenes/level/lvl_player.h>

class Binding_Level_ClassWrapper_LVL_Player : public LVL_Player, public luabind::wrap_base
{
    public:
        Binding_Level_ClassWrapper_LVL_Player();
        virtual ~Binding_Level_ClassWrapper_LVL_Player();

        virtual void lua_onInit();
        virtual void lua_onLoop(float tickTime);
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
        static void def_lua_onLoop(LVL_Player *base, float tickTime)
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
