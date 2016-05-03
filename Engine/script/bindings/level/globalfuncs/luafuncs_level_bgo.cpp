#include "luafuncs_level_bgo.h"
#include <scenes/scene_level.h>

int Binding_Level_GlobalFuncs_BGO::count(lua_State *L)
{
    LevelScene* scene = LuaGlobal::getLevelEngine(L)->getScene();
    LevelScene::LVL_BgosArray &allBGOs = scene->getBGOs();
    return allBGOs .count();
}

luabind::adl::object Binding_Level_GlobalFuncs_BGO::get(lua_State *L)
{
    LevelScene* scene = LuaGlobal::getLevelEngine(L)->getScene();
    LevelScene::LVL_BgosArray &allBGOs = scene->getBGOs();

    luabind::object tableOfBGOs = luabind::newtable(L);
    int i = 1;
    for(auto it = allBGOs.begin(); it != allBGOs.end(); it++)
    {
        LVL_Bgo* bgo = static_cast<LVL_Bgo*>(*it);
        if(bgo)
        {
            tableOfBGOs[i++] = bgo;
        }
    }
    return tableOfBGOs;
}

luabind::scope Binding_Level_GlobalFuncs_BGO::bindToLua()
{
    using namespace luabind;
    return
        namespace_("BGO")
        [
            def("count", &Binding_Level_GlobalFuncs_BGO::count),
            def("get", &Binding_Level_GlobalFuncs_BGO::get)
        ];
}
