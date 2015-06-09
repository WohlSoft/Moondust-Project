#include "lua_titlescreen_engine.h"

#include "bindings/title/globalfuncs/luafuncs_title_renderer.h"

#include <scenes/scene_title.h>

LuaTitleScreenEngine::LuaTitleScreenEngine(TitleScene *scene) : LuaEngine(), m_scene(scene)
{}

LuaTitleScreenEngine::~LuaTitleScreenEngine()
{}

TitleScene *LuaTitleScreenEngine::getScene()
{
    return m_scene;
}

void LuaTitleScreenEngine::onBindAll()
{
    luabind::module(getNativeState())[
        Binding_Title_Renderer::bindToLua()
        ];
}

