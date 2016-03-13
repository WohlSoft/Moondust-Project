#include "luaclass_level_lvl_player.h"

#include <script/lua_global.h>

Binding_Level_ClassWrapper_LVL_Player::Binding_Level_ClassWrapper_LVL_Player() : LVL_Player()
{
    isLuaPlayer = true;
}

Binding_Level_ClassWrapper_LVL_Player::~Binding_Level_ClassWrapper_LVL_Player()
{}

void Binding_Level_ClassWrapper_LVL_Player::lua_onLoop()
{
    if(!LuaGlobal::getEngine(mself.ref(*this).state())->shouldShutdown())
        call<void>("onLoop");
}

void Binding_Level_ClassWrapper_LVL_Player::lua_onHarm(LVL_Player_harm_event *harmevent)
{
    if(!LuaGlobal::getEngine(mself.ref(*this).state())->shouldShutdown())
        call<void>("onHarm", harmevent);
}



luabind::scope Binding_Level_ClassWrapper_LVL_Player::bindToLua()
{
    using namespace luabind;
    return
        class_<LVL_Player, PGE_Phys_Object, detail::null_type, Binding_Level_ClassWrapper_LVL_Player>("BasePlayer")
            .def(constructor<>())
            .def("onLoop", &LVL_Player::lua_onLoop, &Binding_Level_ClassWrapper_LVL_Player::def_lua_onLoop)
            .def("onHarm", &LVL_Player::lua_onHarm, &Binding_Level_ClassWrapper_LVL_Player::def_lua_onHarm)

            .property("health", &LVL_Player::getHealth, &LVL_Player::setHealth)

            .def("bump", &LVL_Player::bump)

            .def("setState", &LVL_Player::setState)
            .def("setCharacter", &LVL_Player::setCharacterID)

            .def_readonly("onGround", &LVL_Player::onGround);
}

luabind::scope Binding_Level_ClassWrapper_LVL_Player::HarmEvent_bindToLua()
{
    using namespace luabind;
    return
        class_<LVL_Player_harm_event>("BasePlayerHarmEvent")
            .def(constructor<>())
            .def_readwrite("do_harm", &LVL_Player_harm_event::doHarm)
            .def_readwrite("damage", &LVL_Player_harm_event::doHarm_damage);
}
