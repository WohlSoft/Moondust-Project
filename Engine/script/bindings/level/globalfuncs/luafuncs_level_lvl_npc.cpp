#include "luafuncs_level_lvl_npc.h"

#include <script/bindings/level/classes/luaclass_level_lvl_npc.h>
#include <scenes/scene_level.h>

int Binding_Level_GlobalFuncs_NPC::count(lua_State *L)
{
    LevelScene* scene = LuaGlobal::getLevelEngine(L)->getScene();
    LevelScene::LVL_NpcsArray &allNPCs = scene->getNpcs();
    return allNPCs.count();
}

luabind::adl::object Binding_Level_GlobalFuncs_NPC::get(lua_State *L)
{
    LevelScene* scene = LuaGlobal::getLevelEngine(L)->getScene();
    LevelScene::LVL_NpcsArray &allNPCs = scene->getNpcs();

    luabind::object tableOfNPCs = luabind::newtable(L);

    int i = 1;
    for(auto it = allNPCs.begin(); it != allNPCs.end(); it++){
        LVL_Npc* npc = static_cast<LVL_Npc*>(*it);
        Binding_Level_ClassWrapper_LVL_NPC* possibleLuaNPC = dynamic_cast<Binding_Level_ClassWrapper_LVL_NPC*>(npc);
        if(possibleLuaNPC ){
            tableOfNPCs[i++] = possibleLuaNPC;
        }else{
            tableOfNPCs[i++] = npc;
        }
    }
    return tableOfNPCs;
}

luabind::adl::object Binding_Level_GlobalFuncs_NPC::getActive(lua_State *L)
{
    LevelScene* scene = LuaGlobal::getLevelEngine(L)->getScene();
    LevelScene::LVL_NpcsArray &allActiveNPCs = scene->getActiveNpcs();

    luabind::object tableOfNPCs = luabind::newtable(L);

    int i = 1;
    for(auto it = allActiveNPCs.begin(); it != allActiveNPCs.end(); it++){
        LVL_Npc* npc = static_cast<LVL_Npc*>(*it);
        Binding_Level_ClassWrapper_LVL_NPC* possibleLuaNPC = dynamic_cast<Binding_Level_ClassWrapper_LVL_NPC*>(npc);
        if(possibleLuaNPC ){
            tableOfNPCs[i++] = possibleLuaNPC;
        }else{
            tableOfNPCs[i++] = npc;
        }
    }
    return tableOfNPCs;
}

luabind::scope Binding_Level_GlobalFuncs_NPC::bindToLua()
{
    using namespace luabind;
    return
        namespace_("NPC")[
            def("count", &Binding_Level_GlobalFuncs_NPC::count),
            def("get", &Binding_Level_GlobalFuncs_NPC::get),
            def("getActive", &Binding_Level_GlobalFuncs_NPC::getActive)
        ];
}
