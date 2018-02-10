#include "lua_engine.h"

#include "lua_event.h"
#include "lua_utils.h"
#include "lua_global.h"

#include <SDL2/SDL_rwops.h>

//Core libraries:
#include "bindings/core/globalfuncs/luafuncs_core_audio.h"
#include "bindings/core/globalfuncs/luafuncs_core_effect.h"
#include "bindings/core/globalfuncs/luafuncs_core_logger.h"
#include "bindings/core/globalfuncs/luafuncs_core_renderer.h"
#include "bindings/core/globalfuncs/luafuncs_core_settings.h"
#include "bindings/core/globalfuncs/luafuncs_core_paths.h"
#include "bindings/core/events/luaevents_core_engine.h"

#include "bindings/core/classes/luaclass_core_simpleevent.h"
#include "bindings/core/classes/luaclass_core_graphics.h"

#include "bindings/core/classes/luaclass_core_scene_effects.h"

#include <Utils/files.h>
#include <Utils/sdl_file.h>
#include <common_features/logger.h>
#include <common_features/fmt_format_ne.h>

#include <sstream>

#ifdef ANDROID
#include <string>
#include <sstream>
template <typename T>
std::string to_string(T value)
{
    std::ostringstream os ;
    os << value ;
    return os.str() ;
}
#define NUM2STR(x) to_string(x)
#else
#define NUM2STR(x) std::to_string(x)
#endif

#ifdef USE_LUA_JIT
extern "C" {
#include <lua_includes/luajit.h>
}
#endif

LuaEngine::LuaEngine() : LuaEngine(nullptr)
{}

LuaEngine::LuaEngine(Scene *scene) : m_lateShutdown(false), m_luaScriptPath(""), m_baseScene(scene), L(nullptr), m_coreFile("")
{}

LuaEngine::~LuaEngine()
{
    //Clear all user caches
    Binding_Core_Graphics::clearCache();

    if(isValid())
        shutdown();
}

static void splitErrorMsg(const std::string &inMsg, std::string &msg1, std::string &msg2)
{
    std::string::size_type lp = inMsg.find('\n');
    if(lp != std::string::npos)
    {
        msg1 = inMsg.substr(0, lp);
        std::string::size_type lp2 = inMsg.find('\n', lp);
        if(lp2 != std::string::npos)
            msg2 = inMsg.substr(lp + 1, lp2);
        else
            msg2 = inMsg.substr(lp + 1);
    }
    else
        msg1 = inMsg;
}

void LuaEngine::init()
{
    //First check if lua engine is already active
    if(isValid())
    {
        pLogWarning("LuaEngine: Called init(), while engine is already initialized!");
        return;
    }

    //Create our new lua state
    L = luaL_newstate();

    //Add it as global
    LuaGlobal::add(L, this);

    //Open up "safe" standard lua libraries
    // FIXME: Add more accurate sandbox
    lua_pushcfunction(L, luaopen_base);
    lua_call(L, 0, 0);
    lua_pushcfunction(L, luaopen_math);
    lua_call(L, 0, 0);
    lua_pushcfunction(L, luaopen_string);
    lua_call(L, 0, 0);
    lua_pushcfunction(L, luaopen_table);
    lua_call(L, 0, 0);
    lua_pushcfunction(L, luaopen_debug);
    lua_call(L, 0, 0);
    lua_pushcfunction(L, luaopen_os);
    lua_call(L, 0, 0);
    lua_pushcfunction(L, luaopen_package);
    lua_call(L, 0, 0);
    #ifdef USE_LUA_JIT
    lua_pushcfunction(L, luaopen_bit);
    lua_call(L, 0, 0);
    lua_pushcfunction(L, luaopen_ffi);
    lua_call(L, 0, 0);
    lua_pushcfunction(L, luaopen_jit);
    lua_call(L, 0, 0);
    #endif

    //SOCKET TESTING STUFF
    lua_pushcfunction(L, luaopen_io);
    lua_call(L, 0, 0);

    lua_getfield(L, LUA_GLOBALSINDEX, "package");
    lua_getfield(L, -1, "preload");

    //lua_pushcfunction(L, luaopen_socket_core);
    //lua_setfield(L, -2, "socket.core");

    //lua_pushcfunction(L, luaopen_mime_core);
    //lua_setfield(L, -2, "mime.core");

    //Remove unsafe apis
    {
        luabind::object _G =    luabind::globals(L);
        luabind::object osTable = _G["os"];
        osTable["execute"] =    luabind::object();
        osTable["exit"] =       luabind::object();
        //osTable["getenv"] =   object();
        osTable["remove"] =     luabind::object();
        osTable["rename"] =     luabind::object();
        osTable["setlocal"] =   luabind::object();
        osTable["tmpname"] =    luabind::object();
    }

    //Activate Luabind for out state
    luabind::open(L);

    //Add error handler
    luabind::set_pcall_callback(&push_pcall_handler);

    //Complete the lua script path
    std::string fullPaths;
    for(size_t i = 0; i < m_luaScriptPaths.size(); i++)
    {
        std::string luaPath = m_luaScriptPaths[i];
        if(!luaPath.empty() && luaPath.back() != '/')
            luaPath += "/";
        fullPaths += luaPath + "?.lua";
        if(i < (m_luaScriptPaths.size() - 1))
            fullPaths.push_back(';');
    }

    //Add config package path
    if(fullPaths != "")
    {
        luabind::object _G = luabind::globals(L);
        luabind::object package = _G["package"];
        std::string allPaths;
        try
        {
            allPaths = luabind::object_cast<std::string>(package["path"]);
        }
        catch(const luabind::cast_failed &e)
        {
            pLogCritical("LuaEngine fail: Exception thrown on attempt to cast 'package[\"path\"]': %s", e.what());
            allPaths = ".";
        }
        //allPaths += std::string(";") +  m_luaScriptPath.toStdString() + "?.lua";
        allPaths += std::string(";") +  fullPaths;
        package["path"] = allPaths;
    }

    //Add error reporter
    if(!m_errorReporterFunc)
    {
        m_errorReporterFunc = [this](const std::string & errMsg, const std::string & stacktrace)
        {
            pLogWarning("Lua-Error: ");
            pLogWarning("Error Message: %s", errMsg.c_str());
            pLogWarning("Stacktrace: \n%s", stacktrace.c_str());
        };
    }

    //Now let's bind our functions
    bindCore();

    //The rest of the functions
    onBindAll();

    //Now read the core file. This file should manage incoming events
    //and process them to all other files.
    std::string coreFilePath = util::resolveRelativeOrAbsolute(m_coreFile, {m_luaScriptPath});
    std::string userFilePath = util::resolveRelativeOrAbsolute(m_userFile, {m_luaScriptPath});

    SdlFile luaCoreFile(coreFilePath);
    if(!luaCoreFile.open(SdlFile::Read))
    {
        pLogWarning("Failed to load up \"%s\"! Wrong path or insufficient access?", coreFilePath.c_str());
        m_lateShutdown = true;
        shutdown();
        return;
    }

    loadMultiRet(&luaCoreFile);
    luaCoreFile.close();

    // OPTIONAL: User code file
    SdlFile luaUserFile(userFilePath);
    if(luaUserFile.open(SdlFile::Read))
    {
        loadMultiRet(&luaUserFile);
        luaUserFile.close();
    }

    LuaEvent initEvent = BindingCore_Events_Engine::createInitEngineEvent(this);
    dispatchEvent(initEvent);
}

void LuaEngine::shutdown()
{
    if(!isValid())
    {
        pLogWarning("LuaEngine: Trying to shutdown invalid lua");
        return;
    }

    loadedFiles.clear();

    // FIXME: Add shutdown event

    forceShutdown();
}

void LuaEngine::forceShutdown()
{
    if(!isValid())
    {
        pLogWarning("LuaEngine: Trying to force shutdown invalid lua");
        return;
    }

    LuaGlobal::remove(L);
    lua_close(L);
    L = nullptr;
}

luabind::object LuaEngine::loadClassAPI(const std::string &path)
{
    ScriptsHash::iterator existFile = loadedFiles.find(path);
    if(existFile != loadedFiles.end())
        return existFile->second;

    SdlFile luaCoreFile(path);
    if(!luaCoreFile.open(SdlFile::Read))
    {
        pLogWarning("Failed to load up \"%s\"! Wrong path or insufficient access?", path.c_str());
        shutdown();
        #ifdef DEBUG_BUILD
        abort();
        #endif
        return luabind::object();
    }

    //Now read our code.
    std::string luaCoreCode = luaCoreFile.readAll();

    //Now load our code by lua and check for common compile errors
    int errorCode = luautil_loadclass(L,
                                      luaCoreCode.data(),
                                      luaCoreCode.length(),
                                      Files::basename(m_coreFile).c_str());
    //If we get an error, then handle it
    if(errorCode)
    {
        pLogWarning("Got lua error, reporting...");
        m_errorReporterFunc(std::string(lua_tostring(L, -1)), std::string(""));
        m_lateShutdown = true;
        return luabind::object();
    }

    luabind::object tReturn(luabind::from_stack(L, -1));
    if(luabind::type(tReturn) != LUA_TUSERDATA)
    {
        pLogWarning("Invalid return type of loading class");
        return luabind::object();
    }

    loadedFiles.insert({path, tReturn});
    return tReturn;
}

void LuaEngine::loadClassAPI(const std::string &nameInGlobal, const std::string &path)
{
    luabind::globals(L)[nameInGlobal] = loadClassAPI(path);
}

std::string LuaEngine::coreFile() const
{
    return m_coreFile;
}

void LuaEngine::setCoreFile(const std::string &coreFile)
{
    if(isValid())
    {
        pLogWarning("Trying to change core lua file while lua engine is already initialized!");
        return;
    }
    m_coreFile = coreFile;
}

void LuaEngine::dispatchEvent(LuaEvent &toDispatchEvent)
{
    if(m_lateShutdown)
        return;

    if(!isValid())
    {
        pLogWarning("Dispatching events while engine is invalid!");
        return;
    }

    lua_getglobal(L, "__native_event");
    if(!lua_isfunction(L, -1))
    {
        pLogWarning("Did not find __native_event function in core!");
        lua_pop(L, 1);
        return;
    }

    try
    {
        luabind::object(L, toDispatchEvent).push(L);

        int argsNum = 0;
        for(luabind::object &obj : toDispatchEvent.objList)
        {
            argsNum++;
            obj.push(L);
        }

        if(luabind::detail::pcall(L, argsNum + 1, 0) != 0)
            throw(std::runtime_error(lua_tostring(L, -1)));
    }
    catch(const std::runtime_error &e)
    {
        //Be sure to cleanup all objects
        toDispatchEvent.cleanupAllParams();
        std::string runtimeErrorMsg = e.what();
        std::string msg1, msg2;
        splitErrorMsg(runtimeErrorMsg, msg1, msg2);
        m_errorReporterFunc(msg1, msg2);
        m_lateShutdown = true;
    }
    catch(...)
    {
        //Be sure to cleanup all objects
        toDispatchEvent.cleanupAllParams();
        std::string runtimeErrorMsg = fmt::format_ne("Thrown unknown exception (Lua error: [{0}])", lua_tostring(L, -1));
        std::string msg1, msg2;
        splitErrorMsg(runtimeErrorMsg, msg1, msg2);
        m_errorReporterFunc(msg1, msg2);
        m_lateShutdown = true;
    }
}


void LuaEngine::bindCore()
{
    luabind::module(L)[
        LuaEvent::bindToLua(),
        Binding_Core_GlobalFuncs_Logger::bindToLua(),
        Binding_Core_GlobalFuncs_Settings::bindToLua(),
        Binding_Core_GlobalFuncs_Audio::bindToLua(),
        Binding_Core_GlobalFuncs_Paths::bindToLua(),
        Binding_Core_Class_SimpleEvent::bindToLua(),
        Binding_Core_Scene_Effects::bindToLua()
    ];
    if(m_baseScene)
    {
        luabind::module(L)[
            Binding_Core_GlobalFuncs_Renderer::bindToLuaRenderer(),
            Binding_Core_GlobalFuncs_Renderer::bindToLuaText(),
            Binding_Core_Graphics::PGETexture_bindToLua(),
            Binding_Core_Graphics::bindToLua(),
            Binding_Core_GlobalFuncs_Effect::bindToLua()
        ];
    }

    //Bind constants
    Binding_Core_GlobalFuncs_Audio::bindConstants(L);
}

void LuaEngine::error()
{
    pLogWarning("Runtime Lua Error, shutting down");
    shutdown();
}
std::string LuaEngine::getUserFile() const
{
    return m_userFile;
}

void LuaEngine::setUserFile(const std::string &userFile)
{
    m_userFile = userFile;
}

void LuaEngine::loadMultiRet(SdlFile *file, const std::string &name)
{
    SDL_assert(file);
    //Now read our code.
    std::string luaCode = file->readAll();

    //Now load our code by lua and check for common compile errors
    int errorCode = luautil_loadlua(L,
                                    luaCode.data(),
                                    luaCode.length(),
                                    name.data());
    //If we get an error, then handle it
    if(errorCode)
    {
        pLogWarning("Got lua error, reporting... [loadMultiRet] %s", file->fileName().c_str());
        m_errorReporterFunc(std::string(lua_tostring(L, -1)), std::string(""));
        m_lateShutdown = true;
        shutdown();
        return;
    }
}

bool LuaEngine::shouldShutdown() const
{
    return m_lateShutdown;
}

void LuaEngine::setLateShutdown(bool value)
{
    m_lateShutdown = value;
}

void LuaEngine::postLateShutdownError(const char *what)
{
    std::string runtimeErrorMsg = what;
    std::string msg1, msg2;
    splitErrorMsg(runtimeErrorMsg, msg1, msg2);
    m_errorReporterFunc(msg1, msg2);
    m_lateShutdown = true;
}

void LuaEngine::postLateShutdownError(luabind::error &error)
{
    postLateShutdownError(error.what());
}

void LuaEngine::runGarbageCollector()
{
    lua_gc(L, LUA_GCCOLLECT, 0);
}

std::string LuaEngine::getLuaScriptPath() const
{
    return m_luaScriptPath;
}

void LuaEngine::setLuaScriptPath(const std::string &luaScriptPath)
{
    m_luaScriptPaths.clear();
    m_luaScriptPaths.push_back(luaScriptPath);
    m_luaScriptPath = luaScriptPath;
}

void LuaEngine::appendLuaScriptPath(const std::string &luaScriptPath)
{
    m_luaScriptPaths.push_back(luaScriptPath);
}

void LuaEngine::setFileSearchPath(const std::string &path)
{
    Binding_Core_Graphics::setRootPath(path);
}

Scene *LuaEngine::getBaseScene() const
{
    return m_baseScene;
}

void LuaEngine::setErrorReporterFunc(const std::function<void (const std::string &, const std::string &)> &value)
{
    m_errorReporterFunc = value;
}

int pcall_handler(lua_State *L)
{
    std::string msg = "";

    int level = 1;
    lua_Debug d;
    bool gotInfoStacktrace = false;
    while(lua_getstack(L, level, &d))
    {
        gotInfoStacktrace = true;
        lua_getinfo(L, "Sln", &d);
        if(level == 1)
        {
            std::string err = lua_tostring(L, -1);
            lua_pop(L, 1);
            msg += err + "\n";
        }
        else
        {
            std::string nextEntry = "\tat ";

            if(util::strempty(d.short_src))
                nextEntry += std::string(d.short_src) + " ";
            else
                nextEntry += "[unknown codefile] ";
            if(util::strempty(d.what))
                nextEntry += std::string(d.what) + " ";
            else
                nextEntry += "[unknown source] ";
            if(util::strempty(d.namewhat))
                nextEntry += std::string(d.namewhat) + " ";
            else
                nextEntry += "[unknown type] ";
            if(util::strempty(d.name))
                nextEntry += std::string(d.name) + " ";
            else
                nextEntry += "[unknown name] ";
            nextEntry += std::string("at line ") + NUM2STR(d.currentline) + std::string("\n");

            msg += nextEntry;
        }

        level++;
    }

    if(msg.size() > 0)
        msg.erase(msg.end() - 1, msg.end());

    if(!gotInfoStacktrace)
    {
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
