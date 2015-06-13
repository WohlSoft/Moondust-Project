#include "lua_level_engine.h"

#include <scenes/scene_level.h>
#include <scenes/level/lvl_player.h>
#include "bindings/level/classes/luaclass_level_lvl_player.h"
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

LevelScene *LuaLevelEngine::getScene()
{
    return dynamic_cast<LevelScene*>(getBaseScene());
}

void LuaLevelEngine::onBindAll()
{
    luabind::module(getNativeState())[
        Binding_Level_Class_PhysObj::bindToLua(),
        Binding_Level_ClassWrapper_LVL_Player::bindToLua()
        ];
}

