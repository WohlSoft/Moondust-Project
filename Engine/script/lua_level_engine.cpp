#include "lua_level_engine.h"

#include <scenes/scene_level.h>

LuaLevelEngine::LuaLevelEngine(LevelScene *scene) : LuaEngine(), m_scene(scene)
{

}

LuaLevelEngine::~LuaLevelEngine()
{

}

LevelScene *LuaLevelEngine::getScene()
{
    return m_scene;
}

void LuaLevelEngine::onBindAll()
{

}

