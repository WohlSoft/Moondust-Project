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
    virtual void lua_onKill(int damageReason);
    virtual void lua_onHarm(int damage, int damageReason);
    virtual void lua_onTransform(long id);

    static void def_lua_onActivated(LVL_Npc* base) { base->LVL_Npc::lua_onActivated(); }
    static void def_lua_onLoop(LVL_Npc* base, float tickTime) { base->LVL_Npc::lua_onLoop(tickTime); }
    static void def_lua_onInit(LVL_Npc* base) { base->LVL_Npc::lua_onInit(); }
    static void def_lua_onKill(LVL_Npc* base, int damageReason) { base->LVL_Npc::lua_onKill(damageReason); }
    static void def_lua_onHarm(LVL_Npc* base, int damage, int damageReason) { base->LVL_Npc::lua_onHarm(damage, damageReason); }
    static void def_lua_onTransform(LVL_Npc* base, long id) { base->LVL_Npc::lua_onTransform(id); }


    static luabind::scope bindToLua();
private:
    luabind::detail::wrap_access mself;
};

#endif // BINDING_LEVEL_CLASSWRAPPER_LVL_NPC_H
