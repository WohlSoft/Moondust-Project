#include "lualevelengine.h"

#include <scenes/scene_level.h>

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

}

