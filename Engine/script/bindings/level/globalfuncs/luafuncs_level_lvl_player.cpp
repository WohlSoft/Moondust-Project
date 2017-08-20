#include "luafuncs_level_lvl_player.h"

#include <script/bindings/level/classes/luaclass_level_lvl_player.h>

#include <scenes/scene_level.h>

unsigned long Binding_Level_GlobalFuncs_Player::count(lua_State *L)
{
    LevelScene* scene = LuaGlobal::getLevelEngine(L)->getScene();
    LevelScene::LVL_PlayersArray &allPlayers = scene->getPlayers();
    return (unsigned long)allPlayers.size();
}

luabind::adl::object Binding_Level_GlobalFuncs_Player::get(lua_State* L)
{
    LevelScene* scene = LuaGlobal::getLevelEngine(L)->getScene();
    LevelScene::LVL_PlayersArray &allPlayers = scene->getPlayers();

    luabind::object tableOfPlayers = luabind::newtable(L);

    int i = 1;
    for(auto it = allPlayers.begin(); it != allPlayers.end(); it++){
        LVL_Player* player = static_cast<LVL_Player*>(*it);
        Binding_Level_ClassWrapper_LVL_Player* possibleLuaPlayer = dynamic_cast<Binding_Level_ClassWrapper_LVL_Player*>(player);
        if(possibleLuaPlayer){
            tableOfPlayers[i++] = possibleLuaPlayer;
        }else{
            tableOfPlayers[i++] = player;
        }
    }
    return tableOfPlayers;
}

/***
Level specific functions and classes
@module LevelCommon
*/

luabind::scope Binding_Level_GlobalFuncs_Player::bindToLua()
{
    using namespace luabind;
    return
        /***
        Playable Characters static functions
        @section LevelPlayerFuncs
        */
        namespace_("Player")[
            /***
            Count total Playable Characters on the level
            @function Player.count
            @return ulong Total count of Playalbe Characters
            */
            def("count", &Binding_Level_GlobalFuncs_Player::count),

            /***
            Get array of all Playalbe Characters are on the level
            @function Player.get
            @return table Array of @{LevelPlayerClass.BasePlayer} references
            */
            def("get", &Binding_Level_GlobalFuncs_Player::get)
        ];
}
