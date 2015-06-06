#include "luaengine.h"

#include "luaevent.h"
#include "luautils.h"

//Core libraries:
#include "bindings/core/globalfuncs/luafuncs_logger.h"
#include "bindings/core/events/luaevents_engine.h"

#include <QFile>
#include <tuple>

LuaEngine::LuaEngine() : L(nullptr), m_coreFile("")
{}

LuaEngine::~LuaEngine()
{
    shutdown();
}

void LuaEngine::init()
{
    qDebug() <<"check 'is valid'";
    //First check if lua engine is already active
    if(isValid()){
        qWarning() << "LuaEngine: Called init(), while engine is already initialized!";
        return;
    }

    qDebug() <<"Nee state...";

    //Create our new lua state
    L = luaL_newstate();
    luaopen_base(L);
    luaL_openlibs(L);

    qDebug() <<"luabind...open";
    //Activate Luabind for out state
    luabind::open(L);

    qDebug() <<"pushes...";
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

    qDebug() <<"bind core";
    //Now let's bind our functions
    bindCore();

    qDebug() <<"bind all";
    //The rest of the functions
    onBindAll();


    qDebug() <<"core file...";
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

    LuaEvent initEvent = BindingCore_Events_Engine::createInitEngineEvent(this);
    dispatchEvent(initEvent);
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
    L = nullptr;
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

void LuaEngine::dispatchEvent(LuaEvent &toDispatchEvent)
{
    if(!isValid()){
        qWarning() << "Dispatching events while engine is invalid!";
        return;
    }

    lua_getglobal(L, "__native_event");
    if(!lua_isfunction(L,-1))
    {
        qWarning() << "Did not find __native_event function in core!";
        lua_pop(L,1);
        return;
    }

    lua_pushstring(L, toDispatchEvent.eventName().toStdString().c_str());

    int argsNum = 0;
    for(luabind::object& obj : toDispatchEvent.objList){
        argsNum++;
        obj.push(L);
    }

    if (lua_pcall(L, argsNum + 1, 0, 0) != 0) {
        onReportError(lua_tostring(L, -1));
        shutdown();
        return;
    }

}

void LuaEngine::onReportError(const QString &errMsg)
{
    qWarning() << "Lua-Error: ";
    qWarning() << errMsg;
}

void LuaEngine::bindCore()
{
    BindingCore_GlobalFuncs_Logger::bindToLua(L);
}

void LuaEngine::error()
{
    qWarning() << "Runtime Lua Error, shutting down";
    shutdown();
}


