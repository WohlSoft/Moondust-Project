/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "luafuncs_level_bgo.h"
#include <scenes/scene_level.h>

unsigned long Binding_Level_GlobalFuncs_BGO::count(lua_State *L)
{
    LevelScene* scene = LuaGlobal::getLevelEngine(L)->getScene();
    LevelScene::LVL_BgosArray &allBGOs = scene->getBGOs();
    return (unsigned long)allBGOs.size();
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

/***
Level specific functions and classes
@module LevelCommon
*/

luabind::scope Binding_Level_GlobalFuncs_BGO::bindToLua()
{
    using namespace luabind;
    return
        /***
        BGO static functions
        @section LevelBGOFuncs
        */
        namespace_("BGO")
        [
            /***
            Count total BGOs on the level
            @function BGO.count
            @return ulong Total count of BGO
            */
            def("count", &Binding_Level_GlobalFuncs_BGO::count),

            /***
            Get array of all BGOs are on the level
            @function BGO.get
            @return table Array of @{LevelBGOClass.BaseBGO} references
            */
            def("get", &Binding_Level_GlobalFuncs_BGO::get)
        ];
}
