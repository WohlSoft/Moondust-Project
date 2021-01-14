/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "luafuncs_level_lvl_npc.h"

#include <script/bindings/level/classes/luaclass_level_lvl_npc.h>
#include <scenes/scene_level.h>

unsigned long Binding_Level_GlobalFuncs_NPC::count(lua_State *L)
{
    LevelScene* scene = LuaGlobal::getLevelEngine(L)->getScene();
    LevelScene::LVL_NpcsArray &allNPCs = scene->getNpcs();
    return (unsigned long)allNPCs.size();
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
    LevelScene::LVL_NpcActiveSet &allActiveNPCs = scene->getActiveNpcs();

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

/***
Level specific functions and classes
@module LevelCommon
*/

luabind::scope Binding_Level_GlobalFuncs_NPC::bindToLua()
{
    using namespace luabind;
    return
        /***
        NPC static functions
        @section LevelNPCFuncs
        */
        namespace_("NPC")[
            /***
            Count total NPCs on the level
            @function NPC.count
            @return ulong Total count of NPCs
            */
            def("count", &Binding_Level_GlobalFuncs_NPC::count),
            /***
            Get array of all NPCs are on the level
            @function NPC.get
            @return table Array of @{LevelNpcClass.BaseNPC} references
            */
            def("get", &Binding_Level_GlobalFuncs_NPC::get),
            /***
            Get array of all activated NPCs (aka all awake NPCs)
            @function NPC.getActive
            @return table Array of @{LevelNpcClass.BaseNPC} references
            */
            def("getActive", &Binding_Level_GlobalFuncs_NPC::getActive)
        ];
}
