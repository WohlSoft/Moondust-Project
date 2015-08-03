#ifndef BINDING_LEVEL_CLASSWRAPPER_LVL_NPC_H
#define BINDING_LEVEL_CLASSWRAPPER_LVL_NPC_H

#include <scenes/level/lvl_npc.h>

#include <luabind/luabind.hpp>
#include <lua_inclues/lua.hpp>

class Binding_Level_ClassWrapper_LVL_NPC : public LVL_Npc, public luabind::wrap_base
{
public:
    Binding_Level_ClassWrapper_LVL_NPC();
    virtual ~Binding_Level_ClassWrapper_LVL_NPC();

    virtual void lua_onActivated();
    virtual void lua_onLoop(float tickTime);
    virtual void lua_onInit();
    virtual void lua_onKill();
    virtual void lua_onHarm(int damage);

    static void def_lua_onActivated(LVL_Npc* base) { base->LVL_Npc::lua_onActivated(); }
    static void def_lua_onLoop(LVL_Npc* base, float tickTime) { base->LVL_Npc::lua_onLoop(tickTime); }
    static void def_lua_onInit(LVL_Npc* base) { base->LVL_Npc::lua_onInit(); }
    static void def_lua_onKill(LVL_Npc* base) { base->LVL_Npc::lua_onKill(); }
    static void def_lua_onHarm(LVL_Npc* base, int damange) { base->LVL_Npc::lua_onHarm(damange); }


    static luabind::scope bindToLua();
};

#endif // BINDING_LEVEL_CLASSWRAPPER_LVL_NPC_H
