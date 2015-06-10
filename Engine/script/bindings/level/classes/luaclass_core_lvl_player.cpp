#include "luaclass_core_lvl_player.h"



Binding_Level_ClassWrapper_LVL_Player::Binding_Level_ClassWrapper_LVL_Player() : LVL_Player()
{}

void Binding_Level_ClassWrapper_LVL_Player::lua_onLoop()
{
    call<void>("onLoop");
}

luabind::scope Binding_Level_ClassWrapper_LVL_Player::bindToLuaBase()
{
    using namespace luabind;
    return
        class_<Binding_Level_ClassWrapper_LVL_Player>("BasePlayer");
}

luabind::scope Binding_Level_ClassWrapper_LVL_Player::bindToLua()
{
    using namespace luabind;
    return
        class_<LVL_Player, Binding_Level_ClassWrapper_LVL_Player>("LVL_Player")
            .def(constructor<>())
            .def("onLoop", &LVL_Player::lua_onLoop, &Binding_Level_ClassWrapper_LVL_Player::lua_onLoop);
}
