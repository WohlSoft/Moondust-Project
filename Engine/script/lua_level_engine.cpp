#include "lua_level_engine.h"

#include <scenes/scene_level.h>
#include <scenes/level/lvl_player.h>
#include <scenes/level/lvl_npc.h>

#include "bindings/level/classes/luaclass_level_lvl_player.h"
#include "bindings/level/classes/luaclass_level_lvl_npc.h"
#include "bindings/level/classes/luaclass_level_physobj.h"
#include "bindings/level/classes/luaclass_level_inareadetector.h"
#include "bindings/level/classes/luaclass_level_playerposdetector.h"

#include "bindings/level/globalfuncs/luafuncs_level_lvl_npc.h"
#include "bindings/level/globalfuncs/luafuncs_level_lvl_player.h"

#include <luabind/adopt_policy.hpp>

LuaLevelEngine::LuaLevelEngine(LevelScene *scene) : LuaEngine(scene)
{

}

LuaLevelEngine::~LuaLevelEngine()
{

}

LVL_Player *LuaLevelEngine::createLuaPlayer()
{
    try {
        return luabind::call_function<LVL_Player*>(getNativeState(), "__create_luaplayer");
    } catch (luabind::error& error){
        postLateShutdownError(error);
        return nullptr;
    }
}

LVL_Npc *LuaLevelEngine::createLuaNpc(unsigned int id)
{
    try {
        return luabind::call_function<LVL_Npc*>(getNativeState(), "__create_luanpc", id);
    } catch (luabind::error& error){
        postLateShutdownError(error);
        return nullptr;
    }
}

void LuaLevelEngine::destoryLuaNpc(LVL_Npc *npc)
{
    try {
        luabind::call_function<void>(getNativeState(), "__destroy_luanpc", npc);
    } catch (luabind::error& error){
        postLateShutdownError(error);
        return;
    }
}

void LuaLevelEngine::loadNPCClass(int id, const QString &path)
{
    if(shouldShutdown())
        return;

    luabind::object _G = luabind::globals(getNativeState());
    if(luabind::type(_G["npc_class_table"]) != LUA_TTABLE){
        _G["npc_class_table"] = luabind::newtable(getNativeState());
    }

    if(luabind::type(_G["npc_class_table"][id]) != LUA_TNIL)
        return;

    _G["npc_class_table"][id] = loadClassAPI(path);
}

LevelScene *LuaLevelEngine::getScene()
{
    return dynamic_cast<LevelScene*>(getBaseScene());
}
QString LuaLevelEngine::getNpcBaseClassPath() const
{
    return m_npcBaseClassPath;
}

void LuaLevelEngine::setNpcBaseClassPath(const QString &npcBaseClassPath)
{
    m_npcBaseClassPath = npcBaseClassPath;
}
QString LuaLevelEngine::getPlayerBaseClassPath() const
{
    return m_playerBaseClassPath;
}

void LuaLevelEngine::setPlayerBaseClassPath(const QString &playerBaseClassPath)
{
    m_playerBaseClassPath = playerBaseClassPath;
}



void LuaLevelEngine::onBindAll()
{
    luabind::module(getNativeState())[
        Binding_Level_Class_PhysObj::bindToLua(),
        Binding_Level_Class_InAreaDetector::bindToLua(),
        Binding_Level_Class_PlayerPosDetector::bindToLua(),
        Binding_Level_ClassWrapper_LVL_Player::bindToLua(),
        Binding_Level_ClassWrapper_LVL_NPC::bindToLua(),
        Binding_Level_GlobalFuncs_Player::bindToLua(),
        Binding_Level_GlobalFuncs_NPC::bindToLua()
    ];

    {
        luabind::object _G = luabind::globals(getNativeState());
        if(luabind::type(_G["bases"]) != LUA_TTABLE){
            _G["bases"] = luabind::newtable(getNativeState());
        }

        _G["bases"]["npc"] = loadClassAPI(m_npcBaseClassPath);
        _G["bases"]["player"] = loadClassAPI(m_playerBaseClassPath);
    }

}

