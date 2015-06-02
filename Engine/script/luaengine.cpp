#include "luaengine.h"


#include "luautils.h"

//Core libraries:
#include "bindings/core/globalfuncs/luafuncs_logger.h"

#include <QFile>

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

    //Open up "safe" standard lua libraries
    // FIXME: Add more accurate sandbox
    lua_pushcfunction(L, luaopen_base);
    lua_call(L,0,0);
    lua_pushcfunction(L, luaopen_math);
    lua_call(L,0,0);
    lua_pushcfunction(L, luaopen_string);
    lua_call(L,0,0);
    lua_pushcfunction(L, luaopen_table);
    lua_call(L,0,0);
    lua_pushcfunction(L, luaopen_debug);
    lua_call(L,0,0);

    //Now let's bind our functions
    bindCore();

    //The rest of the functions
    onBindAll();


    //Now read the core file. This file should manage incoming events
    //and process them to all other files.
    QFile luaCoreFile(m_coreFile);
    if(!luaCoreFile.open(QIODevice::ReadOnly)){
        qWarning() << "Failed to load up \"" << m_coreFile << "\"! Wrong path or insufficient access?";
        shutdown();
        return;
    }

    //Now read our code.
    QString luaCoreCode = QTextStream(&luaCoreFile).readAll();

    //Now load our code by lua and check for common compile errors
    int errorCode = luautil_loadlua(L, luaCoreCode.toLocal8Bit().data(), luaCoreCode.length(), m_coreFile.toLocal8Bit().data());
    //If we get an error, then handle it
    if(errorCode){
        qWarning() << "Got lua error, reporting...";
        onReportError(lua_tostring(L, -1));
        shutdown();
        return;
    }


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

void LuaEngine::onReportError(const QString &errMsg)
{
    qWarning() << "Lua-Error: ";
    qWarning() << errMsg;
}

void LuaEngine::bindCore()
{
    Binding_Logger::bindToLua(L);
}


