#include "lua_level_engine.h"

#include <scenes/scene_level.h>
#include "bindings/level/classes/luaclass_core_lvl_player.h"

LuaLevelEngine::LuaLevelEngine(LevelScene *scene) : LuaEngine(scene)
{

}

LuaLevelEngine::~LuaLevelEngine()
{

}

LevelScene *LuaLevelEngine::getScene()
{
    return dynamic_cast<LevelScene*>(getBaseScene());
}

void LuaLevelEngine::onBindAll()
{
    luabind::module(getNativeState())[
        Binding_Level_ClassWrapper_LVL_Player::bindToLuaBase(),
        Binding_Level_ClassWrapper_LVL_Player::bindToLua()
        ];
}

