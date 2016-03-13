#ifndef LUACLASS_CORE_LVL_PLAYER_H
#define LUACLASS_CORE_LVL_PLAYER_H

#include <scenes/level/lvl_player.h>

#include <luabind/luabind.hpp>
#include <lua_inclues/lua.hpp>

class Binding_Level_ClassWrapper_LVL_Player : public LVL_Player, public luabind::wrap_base
{
public:
    Binding_Level_ClassWrapper_LVL_Player();
    virtual ~Binding_Level_ClassWrapper_LVL_Player();

    virtual void lua_onLoop();
    virtual void lua_onHarm(LVL_Player_harm_event*harmevent);
    static void def_lua_onLoop(LVL_Player* base) { base->LVL_Player::lua_onLoop(); }
    static void def_lua_onHarm(LVL_Player* base, LVL_Player_harm_event*harmevent) { base->LVL_Player::lua_onHarm(harmevent); }

    static luabind::scope bindToLua();
    static luabind::scope HarmEvent_bindToLua();
private:
    luabind::detail::wrap_access mself;
};

#endif // LUACLASS_CORE_LVL_PLAYER_H
