#include "luaclass_level_lvl_npc.h"

#include <script/lua_global.h>

Binding_Level_ClassWrapper_LVL_NPC::Binding_Level_ClassWrapper_LVL_NPC() : LVL_Npc()
{

}

Binding_Level_ClassWrapper_LVL_NPC::~Binding_Level_ClassWrapper_LVL_NPC()
{}

void Binding_Level_ClassWrapper_LVL_NPC::lua_onLoop()
{
    if(!LuaGlobal::getEngine(m_self.state())->shouldShutdown())
        call<void>("onLoop");
}

luabind::scope Binding_Level_ClassWrapper_LVL_NPC::bindToLua()
{
    using namespace luabind;
    return
        class_<LVL_Npc, PGE_Phys_Object, detail::null_type, Binding_Level_ClassWrapper_LVL_NPC>("BaseNPC")
            .def(constructor<>())
            .def("onLoop", &LVL_Npc::lua_onLoop, &Binding_Level_ClassWrapper_LVL_NPC::def_lua_onLoop);


}

