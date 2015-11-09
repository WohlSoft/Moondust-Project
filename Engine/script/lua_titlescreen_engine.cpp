#include "lua_titlescreen_engine.h"

#include "bindings/core/lua_global_constants.h"

#include <scenes/scene_title.h>

LuaTitleScreenEngine::LuaTitleScreenEngine(TitleScene *scene) : LuaEngine(scene)
{}

LuaTitleScreenEngine::~LuaTitleScreenEngine()
{}

TitleScene *LuaTitleScreenEngine::getScene()
{
    return dynamic_cast<TitleScene*>(getBaseScene());
}

void LuaTitleScreenEngine::onBindAll()
{
    Binding_Global_Constants::bindToLua(getNativeState());
}

