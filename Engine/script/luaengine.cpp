#include "luaengine.h"

#include "luaevent.h"
#include "luautils.h"
#include "luaglobal.h"

//Core libraries:
#include "bindings/core/globalfuncs/luafuncs_core_logger.h"
#include "bindings/core/globalfuncs/luafuncs_core_renderer.h"
#include "bindings/core/events/luaevents_core_engine.h"

#include <QFile>
#include <sstream>


LuaEngine::LuaEngine() : LuaEngine(nullptr)
{}

LuaEngine::LuaEngine(Scene *scene) : m_baseScene(scene), L(nullptr), m_coreFile("")
{}

LuaEngine::~LuaEngine()
{
    if(isValid())
        shutdown();
}

void LuaEngine::init()
{
    //First check if lua engine is already active
    if(isValid()){
        qWarning() << "LuaEngine: Called init(), while engine is already initialized!";
        return;
    }

    //Create our new lua state
    L = luaL_newstate();

    //Add it as global
    LuaGlobal::add(L, this);

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
    lua_pushcfunction(L, luaopen_package);
    lua_call(L,0,0);

    //Activate Luabind for out state
    luabind::open(L);

    //Add error handler
    luabind::set_pcall_callback(&push_pcall_handler);

    //Add error reporter
    if(!m_errorReporterFunc){
        m_errorReporterFunc = [this](const QString& errMsg, const QString& stacktrace) {
            qWarning() << "Lua-Error: ";
            qWarning() << "Error Message: " << errMsg;
            qWarning() << "Stacktrace: \n" << stacktrace;
        };
    }

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
    int errorCode = luautil_loadlua(L, luaCoreCode.toLocal8Bit().data(), luaCoreCode.length(), m_coreFile.section('/', -1).section('\\', -1).toLocal8Bit().data());
    //If we get an error, then handle it
    if(errorCode){
        qWarning() << "Got lua error, reporting...";
        m_errorReporterFunc(QString(lua_tostring(L, -1)), QString(""));
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

    LuaGlobal::remove(L);
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

    luabind::object(L, toDispatchEvent).push(L);

    int argsNum = 0;
    for(luabind::object& obj : toDispatchEvent.objList){
        argsNum++;
        obj.push(L);
    }

    if (luabind::detail::pcall(L, argsNum + 1, 0) != 0) {
        QString runtimeErrorMsg = QString(lua_tostring(L, -1));
        m_errorReporterFunc(runtimeErrorMsg.section('\n', 0, 0), runtimeErrorMsg.section('\n', 1));
        shutdown();
        return;
    }
}


void LuaEngine::bindCore()
{
    luabind::module(L)[
        LuaEvent::bindToLua(),
        Binding_Core_GlobalFuncs_Logger::bindToLua()
    ];
    if(m_baseScene){
        luabind::module(L)[Binding_Core_GlobalFuncs_Renderer::bindToLua()];
    }
}

void LuaEngine::error()
{
    qWarning() << "Runtime Lua Error, shutting down";
    shutdown();
}

Scene *LuaEngine::getBaseScene() const
{
    return m_baseScene;
}

void LuaEngine::setErrorReporterFunc(const std::function<void (const QString &, const QString &)> &value)
{
    m_errorReporterFunc = value;
}



int pcall_handler(lua_State *L)
{
    std::string msg = "";

    int level = 1;
    lua_Debug d;
    while(lua_getstack(L, level, &d)){
        lua_getinfo(L, "Sln", &d);
        if(level == 1){
            std::string err = lua_tostring(L, -1);
            lua_pop(L, 1);
            msg += err + "\n";
        }else{
            std::string nextEntry = "\tat ";

            if(util::strempty(d.short_src)){
                nextEntry += std::string(d.short_src) + " ";
            }else{
                nextEntry += "[unknown codefile] ";
            }
            if(util::strempty(d.what)){
                nextEntry += std::string(d.what) + " ";
            }else{
                nextEntry += "[unknown source] ";
            }
            if(util::strempty(d.namewhat)){
                nextEntry += std::string(d.namewhat) + " ";
            }else{
                nextEntry += "[unknown type] ";
            }
            if(util::strempty(d.name)){
                nextEntry += std::string(d.name) + " ";
            }else{
                nextEntry += "[unknown name] ";
            }
            nextEntry += std::string("at line ") + std::to_string(d.currentline) + std::string("\n");

            msg += nextEntry;
        }

        level++;
    }
    msg.erase(msg.end()-1, msg.end());

    lua_pushstring(L, msg.c_str());
    return 1;
}


void push_pcall_handler(lua_State *L)
{
    lua_pushcfunction(L, pcall_handler);
}
