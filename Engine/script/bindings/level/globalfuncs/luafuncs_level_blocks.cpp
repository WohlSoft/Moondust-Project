#include "luafuncs_level_blocks.h"
#include <scenes/scene_level.h>

unsigned long Binding_Level_GlobalFuncs_BLOCKS::count(lua_State *L)
{
    LevelScene* scene = LuaGlobal::getLevelEngine(L)->getScene();
    LevelScene::LVL_BlocksArray &allBlocks = scene->getBlocks();
    return (unsigned long)allBlocks.size();
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

/***
Level specific functions and classes
@module LevelCommon
*/

luabind::scope Binding_Level_GlobalFuncs_BLOCKS::bindToLua()
{
    using namespace luabind;
    return
        /***
        Blocks static functions
        @section LevelBlocksFuncs
        */
        namespace_("Block")
        [
            /***
            Count total Blocks on the level
            @function Block.count
            @return ulong Total count of blocks
            */
            def("count", &Binding_Level_GlobalFuncs_BLOCKS::count),

            /***
            Get array of all Blocks are on the level
            @function Block.get
            @return table Array of @{LevelBlockClass.BaseBlock} references
            */
            def("get", &Binding_Level_GlobalFuncs_BLOCKS::get)
        ];
}
