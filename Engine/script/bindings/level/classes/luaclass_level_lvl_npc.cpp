#include "luaclass_level_lvl_npc.h"

#include <script/lua_global.h>

Binding_Level_ClassWrapper_LVL_NPC::Binding_Level_ClassWrapper_LVL_NPC() : LVL_Npc()
{
    isLuaNPC = true;
}

Binding_Level_ClassWrapper_LVL_NPC::~Binding_Level_ClassWrapper_LVL_NPC()
{}

void Binding_Level_ClassWrapper_LVL_NPC::lua_onLoop()
{
    if(!LuaGlobal::getEngine(m_self.state())->shouldShutdown())
        call<void>("onLoop");
}

void Binding_Level_ClassWrapper_LVL_NPC::lua_onInit()
{
    if(!LuaGlobal::getEngine(m_self.state())->shouldShutdown())
        call<void>("onInit");
}

luabind::scope Binding_Level_ClassWrapper_LVL_NPC::bindToLua()
{
    using namespace luabind;
    return
        class_<LVL_Npc, PGE_Phys_Object, detail::null_type, Binding_Level_ClassWrapper_LVL_NPC>("BaseNPC")
            .def(constructor<>())
            .def("onLoop", &LVL_Npc::lua_onLoop, &Binding_Level_ClassWrapper_LVL_NPC::def_lua_onLoop)
            .def("onInit", &LVL_Npc::lua_onInit, &Binding_Level_ClassWrapper_LVL_NPC::def_lua_onInit)
            .property("id", &LVL_Npc::getID);

}

