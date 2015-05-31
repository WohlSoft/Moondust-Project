#include "luaengine.h"

LuaEngine::LuaEngine()
{}

LuaEngine::~LuaEngine()
{}

void LuaEngine::init()
{
    //First check if lua engine is already active
    if(isValid()){
        qWarning() << "LuaEngine: Called init(), while engine is already initialized!";
        return;
    }

    //Create our new lua state
    L = luaL_newstate();

    //Activate Luabind for out state
    luabind::open(L);


}

void LuaEngine::shutdown()
{
    if(!isValid()){
        qWarning() << "LuaEngine: Trying to shutdown invalid lua";
        return;
    }

    // FIXME: Add shutdown event

    forceShutdown();
}

void LuaEngine::forceShutdown()
{
    if(!isValid()){
        qWarning() << "LuaEngine: Trying to force shutdown invalid lua";
        return;
    }

    lua_close(L);
    L = NULL;
}

QString LuaEngine::coreFile() const
{
    return m_coreFile;
}

void LuaEngine::setCoreFile(const QString &coreFile)
{
    if(isValid()){
        qWarning() << "Trying to change core lua file while lua engine is already initialized!";
        return;
    }
    m_coreFile = coreFile;
}


