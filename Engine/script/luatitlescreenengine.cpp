#include "luatitlescreenengine.h"


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

}

