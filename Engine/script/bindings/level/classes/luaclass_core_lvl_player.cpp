#include "luaclass_core_lvl_player.h"



Binding_Level_ClassWrapper_LVL_Player::Binding_Level_ClassWrapper_LVL_Player() : LVL_Player()
{
    isLuaPlayer = true;
}

Binding_Level_ClassWrapper_LVL_Player::~Binding_Level_ClassWrapper_LVL_Player()
{}

void Binding_Level_ClassWrapper_LVL_Player::lua_onLoop()
{
    call<void>("onLoop");
}

luabind::scope Binding_Level_ClassWrapper_LVL_Player::bindToLua()
{
    using namespace luabind;
    return
        class_<LVL_Player, no_bases, detail::null_type, Binding_Level_ClassWrapper_LVL_Player>("BasePlayer")
            .def(constructor<>())
            .def("onLoop", &LVL_Player::lua_onLoop, &Binding_Level_ClassWrapper_LVL_Player::def_lua_onLoop)
            .def_readwrite("x", &LVL_Player::_posX)
            .def_readwrite("y", &LVL_Player::_posY);


}
