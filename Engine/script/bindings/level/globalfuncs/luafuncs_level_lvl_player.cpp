#include "luafuncs_level_lvl_player.h"

#include <script/bindings/level/classes/luaclass_level_lvl_player.h>

#include <scenes/scene_level.h>

luabind::adl::object Binding_Level_GlobalFuncs_Player::get(lua_State* L)
{
    LevelScene* scene = LuaGlobal::getLevelEngine(L)->getScene();
    LevelScene::LVL_PlayersArray allPlayers = scene->getPlayers();

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

luabind::scope Binding_Level_GlobalFuncs_Player::bindToLua()
{
    using namespace luabind;
    return
        namespace_("Player")[
            def("get", &Binding_Level_GlobalFuncs_Player::get)
        ];
}
