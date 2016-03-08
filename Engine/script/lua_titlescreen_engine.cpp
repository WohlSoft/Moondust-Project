#include "lua_titlescreen_engine.h"

#include "bindings/core/lua_global_constants.h"

#include <scenes/scene_title.h>

LuaTitleScreenEngine::LuaTitleScreenEngine(TitleScene *scene)
    : LuaEngine(scene),
      m_scene(scene)
{}

LuaTitleScreenEngine::~LuaTitleScreenEngine()
{}

TitleScene *LuaTitleScreenEngine::getScene()
{
    return m_scene;
}

void LuaTitleScreenEngine::onBindAll()
{
    Binding_Global_Constants::bindToLua(getNativeState());
}

