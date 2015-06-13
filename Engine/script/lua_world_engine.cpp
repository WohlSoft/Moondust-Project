#include "lua_world_engine.h"

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
}

