/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#ifndef LUAENGINE_H
#define LUAENGINE_H

#include <vector>
#include <functional>
#include <unordered_map>

#include <luabind/luabind.hpp>
#include <lua_includes/lua.hpp>

class Scene;
class LuaEvent;
class SdlFile;
#include "../common_features/util.h"

///
/// \brief This class should have basic functions for interacting with lua
/// To run the lua engine you have to construct the lua engine and then call init()
/// After that you can interpret any lua string with the interpretLua function.
///
/// To add custom class for luabind this class should be derived.
/// As custom features can be added.
///
class LuaEngine
{
    friend class LuaEvent;
private:
    LuaEngine(const LuaEngine&) = delete;
    LuaEngine operator=(const LuaEngine&) = delete;

public:
    LuaEngine();
    LuaEngine(Scene* scene);
    virtual ~LuaEngine();

    static const char *getEngineInfo();

    ///
    /// \brief Init a new lua engine. The lua engine must be in a invalid state.
    ///
    void init();

    ///
    /// \brief Shutdowns the lua engine and post a cleanup event
    ///
    void shutdown();

    ///
    /// \brief Shutdowns the lua engine directly
    ///
    void forceShutdown();

    ///
    /// \brief isValid Checks if the lua engine is valid
    /// \return True, if the engine is valid
    ///
    inline bool isValid() { return L != nullptr; }

    // ///////  LOADING FUCS ///////////// //
    luabind::object loadClassAPI(const std::string &path); //!< Reads a lua class and returns the object
    void loadClassAPI(const std::string& nameInGlobal, const std::string& path); //!< Reads a lua class and puts it in a global object

    // ///////  LOADING FUCS END ///////////// //

    std::string coreFile() const; //!< The core lua filename
    void setCoreFile(const std::string& coreFile); //!< The core lua filename

    void dispatchEvent(LuaEvent& toDispatchEvent); //!< Dispatches a lua event
    void setErrorReporterFunc(const std::function<void (const std::string &, const std::string&)> &func); //!< The error reporter function

    Scene *getBaseScene() const; //!< The base-scene for the lua engine (may need for interacting with current scene)

    std::string getLuaScriptPath() const;
    void setLuaScriptPath(const std::string &luaScriptPath);
    void appendLuaScriptPath(const std::string& luaScriptPath);

    /*!
     * \brief Set a path where look for a various files (images, sounds, etc.)
     * \param path Root path where look various files
     */
    void setFileSearchPath(const std::string &path);

    bool shouldShutdown() const;
    void setLateShutdown(bool value);

    void postLateShutdownError(const char *what);
    void postLateShutdownError(luabind::error &error);

    void runGarbageCollector();

    std::string getUserFile() const;
    void setUserFile(const std::string& userFile);

protected:
    virtual void onBindAll() {}
    void loadMultiRet(SdlFile *file, const std::string &name = "unknown");

    lua_State* getNativeState() {return L; }

private:
    void bindCore();
    void error();

    typedef std::unordered_map<std::string, luabind::object > ScriptsHash;
    ScriptsHash loadedFiles;

    bool m_lateShutdown; //!< If true, then the lua engine will shutdown as soon as possible
    std::string m_luaScriptPath;
    std::vector<std::string> m_luaScriptPaths;
    std::function<void (const std::string & /*error message*/, const std::string& /*stack trace*/)> m_errorReporterFunc;
    Scene* m_baseScene;
    lua_State* L;
    std::string m_coreFile;
    std::string m_userFile;
};

extern void push_pcall_handler(lua_State* L);
extern int pcall_handler(lua_State* L);

#endif // LUAENGINE_H
