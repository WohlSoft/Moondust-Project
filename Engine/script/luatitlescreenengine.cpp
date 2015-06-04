#include "luatitlescreenengine.h"

LuaTitleScreenEngine::LuaTitleScreenEngine() : LuaEngine()
{

}

LuaTitleScreenEngine::~LuaTitleScreenEngine()
{

}

void LuaTitleScreenEngine::onReportError(const QString &errMsg)
{
    LuaEngine::onReportError(errMsg);
}

