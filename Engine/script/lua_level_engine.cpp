#include "lua_level_engine.h"

#include <scenes/scene_level.h>
#include <scenes/level/lvl_player.h>
#include <scenes/level/lvl_npc.h>
#include "bindings/level/classes/luaclass_level_lvl_player.h"
#include "bindings/level/classes/luaclass_level_lvl_npc.h"
#include "bindings/level/classes/luaclass_level_physobj.h"

#include <luabind/adopt_policy.hpp>

LuaLevelEngine::LuaLevelEngine(LevelScene *scene) : LuaEngine(scene)
{

}

LuaLevelEngine::~LuaLevelEngine()
{

}

LVL_Player *LuaLevelEngine::createLuaPlayer()
{
    return luabind::call_function<LVL_Player*>(getNativeState(), "__create_luaplayer");
}

LVL_Npc *LuaLevelEngine::createLuaNpc(unsigned int id)
{
    return luabind::call_function<LVL_Npc*>(getNativeState(), "__create_luanpc", id);
}

void LuaLevelEngine::loadNPCClass(int id, const QString &path)
{
    if(shouldShutdown())
        return;

    luabind::object _G = luabind::globals(getNativeState());
    if(luabind::type(_G["npc_class_table"]) != LUA_TTABLE){
        _G["npc_class_table"] = luabind::newtable(getNativeState());
    }

    luabind::object npcClassTable = _G["npc_class_table"];
    if(luabind::type(npcClassTable[id]) != LUA_TNIL)
        return;

    npcClassTable[id] = loadClassAPI(path);
}

LevelScene *LuaLevelEngine::getScene()
{
    return dynamic_cast<LevelScene*>(getBaseScene());
}

void LuaLevelEngine::onBindAll()
{
    luabind::module(getNativeState())[
        Binding_Level_Class_PhysObj::bindToLua(),
        Binding_Level_ClassWrapper_LVL_Player::bindToLua(),
        Binding_Level_ClassWrapper_LVL_NPC::bindToLua()
        ];
}

