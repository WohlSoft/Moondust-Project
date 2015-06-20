#include "luafuncs_level_lvl_npc.h"

#include <script/bindings/level/classes/luaclass_level_lvl_npc.h>

#include <scenes/scene_level.h>


luabind::adl::object Binding_Level_GlobalFuncs_NPC::get(lua_State *L)
{
    LevelScene* scene = LuaGlobal::getLevelEngine(L)->getScene();
    LevelScene::LVL_NpcsArray allNPCs = scene->getNpcs();

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

luabind::scope Binding_Level_GlobalFuncs_NPC::bindToLua()
{
    using namespace luabind;
    return
        namespace_("NPC")[
            def("get", &Binding_Level_GlobalFuncs_NPC::get)
        ];
}
