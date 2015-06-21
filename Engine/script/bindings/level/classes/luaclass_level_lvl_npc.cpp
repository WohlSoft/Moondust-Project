#include "luaclass_level_lvl_npc.h"

#include <script/lua_global.h>

Binding_Level_ClassWrapper_LVL_NPC::Binding_Level_ClassWrapper_LVL_NPC() : LVL_Npc()
{
    isLuaNPC = true;
}

Binding_Level_ClassWrapper_LVL_NPC::~Binding_Level_ClassWrapper_LVL_NPC()
{}

void Binding_Level_ClassWrapper_LVL_NPC::lua_onActivated()
{
    if(!LuaGlobal::getEngine(m_self.state())->shouldShutdown())
        call<void>("onActivated");
}

void Binding_Level_ClassWrapper_LVL_NPC::lua_onLoop(float tickTime)
{
    if(!LuaGlobal::getEngine(m_self.state())->shouldShutdown())
        call<void>("onLoop", tickTime);
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
            //Events
            .def("onActivated", &LVL_Npc::lua_onActivated, &Binding_Level_ClassWrapper_LVL_NPC::def_lua_onActivated)
            .def("onLoop", &LVL_Npc::lua_onLoop, &Binding_Level_ClassWrapper_LVL_NPC::def_lua_onLoop)
            .def("onInit", &LVL_Npc::lua_onInit, &Binding_Level_ClassWrapper_LVL_NPC::def_lua_onInit)

            //Functions
            .def("setSequenceLeft", &LVL_Npc::lua_setSequenceLeft)
            .def("setSequenceRight", &LVL_Npc::lua_setSequenceRight)
            .def("setSequence", &LVL_Npc::lua_setSequence)

            .def("setSpriteWarp", &LVL_Npc::setSpriteWarp)
            .def("resetSpriteWarp", &LVL_Npc::resetSpriteWarp)

            //Properties
            .property("id", &LVL_Npc::getID)

            //States
            .def_readonly("onGround", &LVL_Npc::onGround);

}

