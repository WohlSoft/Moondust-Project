/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2025 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "lua_level_engine.h"

#include <scenes/scene_level.h>
#include <scenes/level/lvl_player.h>
#include <scenes/level/lvl_npc.h>
#include <scenes/level/lvl_block.h>
#include <scenes/level/lvl_bgo.h>

#include "bindings/core/classes/luaclass_core_data.h"

#include "bindings/level/classes/luaclass_level_lvl_player.h"
#include "bindings/level/classes/luaclass_level_lvl_npc.h"
#include "bindings/level/classes/luaclass_level_physobj.h"
#include "bindings/level/classes/luaclass_level_inareadetector.h"
#include "bindings/level/classes/luaclass_level_playerposdetector.h"
#include "bindings/level/classes/luaclass_level_contact_detector.h"
#include "bindings/level/classes/luaclass_level_playerstate.h"

#include "bindings/level/globalfuncs/luafuncs_level_blocks.h"
#include "bindings/level/globalfuncs/luafuncs_level_bgo.h"
#include "bindings/level/globalfuncs/luafuncs_level_lvl_npc.h"
#include "bindings/level/globalfuncs/luafuncs_level_lvl_player.h"
#include "bindings/level/globalfuncs/luafuncs_level.h"

#include "bindings/core/lua_global_constants.h"

#include <luabind/adopt_policy.hpp>

LuaLevelEngine::LuaLevelEngine(LevelScene *scene) : LuaEngine(scene)
{}

LuaLevelEngine::~LuaLevelEngine()
{}

LVL_Player *LuaLevelEngine::createLuaPlayer()
{
    try
    {
        return luabind::call_function<LVL_Player *>(getNativeState(), "__create_luaplayer");
    }
    catch(const luabind::cast_failed &error)
    {
        postLateShutdownError(error.what());
        return nullptr;
    }
    catch(luabind::error &error)
    {
        postLateShutdownError(error);
        return nullptr;
    }
}

LVL_Npc *LuaLevelEngine::createLuaNpc(unsigned long id)
{
    try
    {
        return luabind::call_function<LVL_Npc *>(getNativeState(), "__create_luanpc", id);
    }
    catch(const luabind::cast_failed &error)
    {
        postLateShutdownError(error.what());
        return nullptr;
    }
    catch(luabind::error &error)
    {
        postLateShutdownError(error);
        return nullptr;
    }
}

void LuaLevelEngine::destoryLuaNpc(LVL_Npc *npc)
{
    try
    {
        luabind::call_function<void>(getNativeState(), "__destroy_luanpc", npc);
    }
    catch(const luabind::cast_failed &error)
    {
        postLateShutdownError(error.what());
        return;
    }
    catch(luabind::error &error)
    {
        postLateShutdownError(error);
        return;
    }
}

void LuaLevelEngine::destoryLuaPlayer(LVL_Player *plr)
{
    try
    {
        luabind::call_function<void>(getNativeState(), "__destroy_luaplayer", plr);
    }
    catch(const luabind::cast_failed &error)
    {
        postLateShutdownError(error.what());
        return;
    }
    catch(luabind::error &error)
    {
        postLateShutdownError(error);
        return;
    }
}

void LuaLevelEngine::initNPCClassTable()
{
    if(shouldShutdown())
        return;

    luabind::object _G = luabind::globals(getNativeState());

    if(luabind::type(_G["npc_class_table"]) != LUA_TTABLE)
        _G["npc_class_table"] = luabind::newtable(getNativeState());
}

void LuaLevelEngine::loadNPCClass(unsigned long id, const std::string &path)
{
    if(shouldShutdown())
        return;

    luabind::object _G = luabind::globals(getNativeState());

    if(luabind::type(_G["npc_class_table"]) != LUA_TTABLE)
        _G["npc_class_table"] = luabind::newtable(getNativeState());

    if(luabind::type(_G["npc_class_table"][id]) != LUA_TNIL)
        return;

    _G["npc_class_table"][id] = loadClassAPI(path);
}

void LuaLevelEngine::loadPlayerClass(unsigned long id, const std::string &path)
{
    if(shouldShutdown())
        return;

    luabind::object _G = luabind::globals(getNativeState());

    if(luabind::type(_G["player_class_table"]) != LUA_TTABLE)
        _G["player_class_table"] = luabind::newtable(getNativeState());

    if(luabind::type(_G["player_class_table"][id]) != LUA_TNIL)
        return;

    _G["player_class_table"][id] = loadClassAPI(path);
}

LevelScene *LuaLevelEngine::getScene()
{
    return dynamic_cast<LevelScene *>(getBaseScene());
}
std::string LuaLevelEngine::getNpcBaseClassPath() const
{
    return m_npcBaseClassPath;
}

void LuaLevelEngine::setNpcBaseClassPath(const std::string &npcBaseClassPath)
{
    m_npcBaseClassPath = npcBaseClassPath;
}

std::string LuaLevelEngine::getPlayerBaseClassPath() const
{
    return m_playerBaseClassPath;
}

void LuaLevelEngine::setPlayerBaseClassPath(const std::string &playerBaseClassPath)
{
    m_playerBaseClassPath = playerBaseClassPath;
}

void LuaLevelEngine::onBindAll()
{
    luabind::module(getNativeState())[
        Binding_Core_Data::bindToLua(),
        Binding_Level_Class_PhysObj::bindBaseToLua(),
        Binding_Level_Class_PhysObj::bindToLua(),
        Binding_Level_Class_InAreaDetector::bindToLua(),
        Binding_Level_Class_PlayerPosDetector::bindToLua(),
        Binding_Level_Class_ContactDetector::bindToLua(),
        Binding_Level_ClassWrapper_LVL_Player::HarmEvent_bindToLua(),
        Binding_Level_ClassWrapper_LVL_Player::bindToLua(),
        Binding_Level_ClassWrapper_LVL_NPC::bindToLua(),
        Binding_Level_ClassWrapper_LVL_NPC::HarmEvent_bindToLua(),
        Binding_Level_ClassWrapper_LVL_NPC::KillEvent_bindToLua(),
        Binding_Level_GlobalFuncs_Player::bindToLua(),
        Binding_Level_GlobalFuncs_BLOCKS::bindToLua(),
        Binding_Level_GlobalFuncs_BGO::bindToLua(),
        Binding_Level_GlobalFuncs_NPC::bindToLua(),
        lua_LevelPlayerState::bindToLua(),
        Binding_Level_CommonFuncs::bindToLua(),
        LVL_Block::bindToLua(),
        LVL_Bgo::bindToLua(),
        PGE_LevelCamera::bindToLua()
    ];
    Binding_Global_Constants::bindToLua(getNativeState());
    {
        luabind::object _G = luabind::globals(getNativeState());

        if(luabind::type(_G["bases"]) != LUA_TTABLE)
            _G["bases"] = luabind::newtable(getNativeState());

        _G["bases"]["npc"] = loadClassAPI(m_npcBaseClassPath);
        _G["bases"]["player"] = loadClassAPI(m_playerBaseClassPath);
    }
}
