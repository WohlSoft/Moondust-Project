#include "lua_world_engine.h"
#include "bindings/core/lua_global_constants.h"

#include <scenes/scene_world.h>

LuaWorldEngine::LuaWorldEngine(WorldScene *scene) : LuaEngine(scene)
{}

LuaWorldEngine::~LuaWorldEngine()
{}

WorldScene *LuaWorldEngine::getScene()
{
    return dynamic_cast<WorldScene*>(getBaseScene());
}

void LuaWorldEngine::onBindAll()
{
    luabind::module(getNativeState())/*[]*/;
    Binding_Global_Constants::bindToLua(getNativeState());
}

