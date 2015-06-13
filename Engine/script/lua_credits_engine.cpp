#include "lua_credits_engine.h"

#include <scenes/scene_credits.h>

LuaCreditsEngine::LuaCreditsEngine(CreditsScene* scene) : LuaEngine(scene)
{}

LuaCreditsEngine::~LuaCreditsEngine()
{}

CreditsScene *LuaCreditsEngine::getScene()
{
    return dynamic_cast<CreditsScene*>(getBaseScene());
}

void LuaCreditsEngine::onBindAll()
{
    luabind::module(getNativeState())/*[

        ]*/;
}

