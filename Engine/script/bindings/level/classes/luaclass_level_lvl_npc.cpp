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

void Binding_Level_ClassWrapper_LVL_NPC::lua_onKill(int damageReason)
{
    if(!LuaGlobal::getEngine(m_self.state())->shouldShutdown())
        call<void>("onKill", damageReason);
}

void Binding_Level_ClassWrapper_LVL_NPC::lua_onHarm(int damage, int damageReason)
{
    if(!LuaGlobal::getEngine(m_self.state())->shouldShutdown())
        call<void>("onHarm", damage, damageReason);
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

            .def("onKill", &LVL_Npc::lua_onKill, &Binding_Level_ClassWrapper_LVL_NPC::def_lua_onKill)
            .def("onHarm", &LVL_Npc::lua_onHarm, &Binding_Level_ClassWrapper_LVL_NPC::def_lua_onHarm)

            //Functions
            .def("setSequenceLeft", &LVL_Npc::lua_setSequenceLeft)
            .def("setSequenceRight", &LVL_Npc::lua_setSequenceRight)
            .def("setSequence", &LVL_Npc::lua_setSequence)
            .def("setOnceAnimationMode", &LVL_Npc::lua_setOnceAnimation)

            .def("installInAreaDetector", &LVL_Npc::lua_installInAreaDetector)
            .def("installPlayerPosDetector", &LVL_Npc::lua_installPlayerPosDetector)

            .def("setSpriteWarp", &LVL_Npc::setSpriteWarp)
            .def("resetSpriteWarp", &LVL_Npc::resetSpriteWarp)

            .def("activateNeighbours", &LVL_Npc::lua_activate_neighbours)

            .def("unregister", &LVL_Npc::unregister)         // Destroy NPC with no effects and no events

            //Properties
            .property("id", &LVL_Npc::getID)
            .property("direction", &LVL_Npc::direction, &LVL_Npc::setDirection)
            .property("not_movable", &LVL_Npc::not_movable)
            .property("special1", &LVL_Npc::special1)
            .property("special2", &LVL_Npc::special2)
            .property("isBoss", &LVL_Npc::isBoss)

            .property("frameDelay", &LVL_Npc::lua_frameDelay, &LVL_Npc::lua_setFrameDelay)

            //States
            .def_readonly("onGround", &LVL_Npc::onGround)
            .def_readonly("animationIsFinished", &LVL_Npc::lua_animationIsFinished);

}

