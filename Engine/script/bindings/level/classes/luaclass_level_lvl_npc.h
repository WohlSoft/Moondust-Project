#ifndef BINDING_LEVEL_CLASSWRAPPER_LVL_NPC_H
#define BINDING_LEVEL_CLASSWRAPPER_LVL_NPC_H

#include <scenes/level/lvl_npc.h>

#include <luabind/luabind.hpp>
#include <lua_includes/lua.hpp>

class Binding_Level_ClassWrapper_LVL_NPC : public LVL_Npc, public luabind::wrap_base
{
    public:
        Binding_Level_ClassWrapper_LVL_NPC(LevelScene *_parent = NULL);
        virtual ~Binding_Level_ClassWrapper_LVL_NPC();

        virtual void lua_onActivated();
        virtual void lua_onLoop(double tickTime);
        virtual void lua_onInit();
        virtual void lua_onKill(KillEvent *killEvent);
        virtual void lua_onHarm(HarmEvent *harmEvent);
        virtual void lua_onTransform(unsigned long id);

        static void def_lua_onActivated(LVL_Npc *base)
        {
            base->LVL_Npc::lua_onActivated();
        }
        static void def_lua_onLoop(LVL_Npc *base, double tickTime)
        {
            base->LVL_Npc::lua_onLoop(tickTime);
        }
        static void def_lua_onInit(LVL_Npc *base)
        {
            base->LVL_Npc::lua_onInit();
        }
        static void def_lua_onKill(LVL_Npc *base, KillEvent *killEvent)
        {
            base->LVL_Npc::lua_onKill(killEvent);
        }
        static void def_lua_onHarm(LVL_Npc *base, HarmEvent *harmEvent)
        {
            base->LVL_Npc::lua_onHarm(harmEvent);
        }
        static void def_lua_onTransform(LVL_Npc *base, unsigned long id)
        {
            base->LVL_Npc::lua_onTransform(id);
        }

        static luabind::scope bindToLua();
        static luabind::scope HarmEvent_bindToLua();
        static luabind::scope KillEvent_bindToLua();
    private:
        luabind::detail::wrap_access mself;
};

#endif // BINDING_LEVEL_CLASSWRAPPER_LVL_NPC_H
