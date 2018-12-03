/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

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
