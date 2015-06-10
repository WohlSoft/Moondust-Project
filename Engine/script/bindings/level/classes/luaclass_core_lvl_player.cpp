#include "luaclass_core_lvl_player.h"



Binding_Level_ClassWrapper_LVL_Player::Binding_Level_ClassWrapper_LVL_Player() : LVL_Player()
{}

void Binding_Level_ClassWrapper_LVL_Player::lua_onLoop()
{
    call<void>("onLoop");
}
