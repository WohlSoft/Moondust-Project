#include "lua_engine.h"

#include "lua_event.h"
#include "lua_utils.h"
#include "lua_global.h"

//Core libraries:
#include "bindings/core/globalfuncs/luafuncs_core_logger.h"
#include "bindings/core/globalfuncs/luafuncs_core_renderer.h"
#include "bindings/core/globalfuncs/luafuncs_core_settings.h"
#include "bindings/core/events/luaevents_core_engine.h"

#include <QFile>
#include <sstream>
#include <QFileInfo>

LuaEngine::LuaEngine() : LuaEngine(nullptr)
{}

LuaEngine::LuaEngine(Scene *scene) : m_lateShutdown(false), m_luaScriptPath(""), m_baseScene(scene), L(nullptr), m_coreFile("")
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

    //Complete the lua script path
    if(!m_luaScriptPath.endsWith("/"))
        m_luaScriptPath += "/";

    //Add config package path
    if(m_luaScriptPath != ""){
        luabind::object _G = luabind::globals(L);
        luabind::object package = _G["package"];
        std::string allPaths = luabind::object_cast<std::string>(package["path"]);
        allPaths += std::string(";") +  m_luaScriptPath.toStdString() + "?.lua";
        package["path"] = allPaths;
    }

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
    QString coreFilePath;
    if(QFileInfo(m_coreFile).isAbsolute()){
        coreFilePath = m_coreFile;
    }else{
        if(m_luaScriptPath != ""){
            coreFilePath = m_luaScriptPath + m_coreFile;
        }
    }

    QFile luaCoreFile(coreFilePath);
    if(!luaCoreFile.open(QIODevice::ReadOnly)){
        qWarning() << "Failed to load up \"" << coreFilePath << "\"! Wrong path or insufficient access?";
        m_lateShutdown = true;
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
        m_lateShutdown = true;
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

luabind::object LuaEngine::loadClassAPI(const QString &path)
{
    QFile luaCoreFile(path);
    if(!luaCoreFile.open(QIODevice::ReadOnly)){
        qWarning() << "Failed to load up \"" << path << "\"! Wrong path or insufficient access?";
        shutdown();
        return luabind::object();
    }

    //Now read our code.
    QString luaCoreCode = QTextStream(&luaCoreFile).readAll();

    //Now load our code by lua and check for common compile errors
    int errorCode = luautil_loadclass(L, luaCoreCode.toLocal8Bit().data(), luaCoreCode.length(), m_coreFile.section('/', -1).section('\\', -1).toLocal8Bit().data());
    //If we get an error, then handle it
    if(errorCode){
        qWarning() << "Got lua error, reporting...";
        m_errorReporterFunc(QString(lua_tostring(L, -1)), QString(""));
        m_lateShutdown = true;
        return luabind::object();
    }

    luabind::object tReturn(luabind::from_stack(L, -1));
    if(luabind::type(tReturn) != LUA_TUSERDATA){
        qWarning() << "Invalid return type of loading class";
        return luabind::object();
    }
    return tReturn;
}

void LuaEngine::loadClassAPI(const QString &nameInGlobal, const QString &path)
{
    luabind::globals(L)[nameInGlobal.toStdString()] = loadClassAPI(path);
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
    if(m_lateShutdown)
        return;

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
        //Be sure to cleanup all objects
        toDispatchEvent.cleanupAllParams();

        QString runtimeErrorMsg = QString(lua_tostring(L, -1));
        m_errorReporterFunc(runtimeErrorMsg.section('\n', 0, 0), runtimeErrorMsg.section('\n', 1));
        m_lateShutdown = true;
    }
}


void LuaEngine::bindCore()
{
    luabind::module(L)[
        LuaEvent::bindToLua(),
        Binding_Core_GlobalFuncs_Logger::bindToLua(),
        Binding_Core_GlobalFuncs_Settings::bindToLua()
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
bool LuaEngine::shouldShutdown() const
{
    return m_lateShutdown;
}

void LuaEngine::setLateShutdown(bool value)
{
    m_lateShutdown = value;
}

void LuaEngine::postLateShutdownError(luabind::error &error)
{
    QString runtimeErrorMsg = error.what();
    m_errorReporterFunc(runtimeErrorMsg.section('\n', 0, 0), runtimeErrorMsg.section('\n', 1));
    m_lateShutdown = true;
}

QString LuaEngine::getLuaScriptPath() const
{
    return m_luaScriptPath;
}

void LuaEngine::setLuaScriptPath(const QString &luaScriptPath)
{
    m_luaScriptPath = luaScriptPath;
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
    bool gotInfoStacktrace = false;
    while(lua_getstack(L, level, &d)){
        gotInfoStacktrace = true;
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

    if(msg.size() > 0)
        msg.erase(msg.end()-1, msg.end());

    if(!gotInfoStacktrace){
        if(lua_gettop(L) > 0)
            if(lua_type(L, -1) == LUA_TSTRING)
                msg = lua_tostring(L, -1);
    }

    lua_pushstring(L, msg.c_str());
    return 1;
}


void push_pcall_handler(lua_State *L)
{
    lua_pushcfunction(L, pcall_handler);
}
