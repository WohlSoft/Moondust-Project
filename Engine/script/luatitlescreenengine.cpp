#include "luatitlescreenengine.h"

#include <scenes/scene_title.h>

LuaTitleScreenEngine::LuaTitleScreenEngine(TitleScene *scene) : LuaEngine(), m_scene(scene)
{}

LuaTitleScreenEngine::~LuaTitleScreenEngine()
{}

TitleScene *LuaTitleScreenEngine::getScene()
{
    return m_scene;
}

