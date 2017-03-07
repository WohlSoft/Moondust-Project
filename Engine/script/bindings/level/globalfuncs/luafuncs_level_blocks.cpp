#include "luafuncs_level_blocks.h"
#include <scenes/scene_level.h>

int Binding_Level_GlobalFuncs_BLOCKS::count(lua_State *L)
{
    LevelScene* scene = LuaGlobal::getLevelEngine(L)->getScene();
    LevelScene::LVL_BlocksArray &allBlocks = scene->getBlocks();
    return allBlocks.size();
}

luabind::adl::object Binding_Level_GlobalFuncs_BLOCKS::get(lua_State *L)
{
    LevelScene* scene = LuaGlobal::getLevelEngine(L)->getScene();
    LevelScene::LVL_BlocksArray &allBlocks = scene->getBlocks();

    luabind::object tableOfBlocks = luabind::newtable(L);
    int i = 1;
    for(auto it = allBlocks.begin(); it != allBlocks.end(); it++)
    {
        LVL_Block* block = static_cast<LVL_Block*>(*it);
        if(block)
        {
            tableOfBlocks[i++] = block;
        }
    }
    return tableOfBlocks;
}

luabind::scope Binding_Level_GlobalFuncs_BLOCKS::bindToLua()
{
    using namespace luabind;
    return
        namespace_("Block")
        [
            def("count", &Binding_Level_GlobalFuncs_BLOCKS::count),
            def("get", &Binding_Level_GlobalFuncs_BLOCKS::get)
        ];
}
