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
    static void def_lua_onLoop(LVL_Player* base) { base->LVL_Player::lua_onLoop(); }

    static luabind::scope bindToLua();

};

#endif // LUACLASS_CORE_LVL_PLAYER_H
