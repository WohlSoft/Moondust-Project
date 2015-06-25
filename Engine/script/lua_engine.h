#ifndef LUAENGINE_H
#define LUAENGINE_H

#include <QObject>
#include <QString>
#include <QtDebug>
#include <QHash>
#include <QFile>

#include <functional>

#include <luabind/luabind.hpp>
#include <lua_inclues/lua.hpp>

class Scene;
class LuaEvent;
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
    Q_DISABLE_COPY(LuaEngine)

public:
    LuaEngine();
    LuaEngine(Scene* scene);
    virtual ~LuaEngine();

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
    luabind::object loadClassAPI(const QString& path); //!< Reads a lua class and returns the object
    void loadClassAPI(const QString& nameInGlobal, const QString& path); //!< Reads a lua class and puts it in a global object

    // ///////  LOADING FUCS END ///////////// //

    QString coreFile() const; //!< The core lua filename
    void setCoreFile(const QString &coreFile); //!< The core lua filename

    void dispatchEvent(LuaEvent& toDispatchEvent); //!< Dispatches a lua event
    void setErrorReporterFunc(const std::function<void (const QString &, const QString&)> &func); //!< The error reporter function

    Scene *getBaseScene() const; //!< The base-scene for the lua engine (may need for interacting with current scene)

    QString getLuaScriptPath() const;
    void setLuaScriptPath(const QString &luaScriptPath);

    bool shouldShutdown() const;
    void setLateShutdown(bool value);

    void postLateShutdownError(luabind::error& error);

    void runGarbageCollector();

    QString getUserFile() const;
    void setUserFile(const QString &userFile);

protected:
    virtual void onBindAll() {}
    void loadMultiRet(QFile *file);

    lua_State* getNativeState() {return L; }

private:
    void bindCore();
    void error();
    QHash<QString, luabind::object > loadedFiles;

    bool m_lateShutdown; //!< If true, then the lua engine will shutdown as soon as possible
    QString m_luaScriptPath;
    std::function<void (const QString & /*error message*/, const QString& /*stack trace*/)> m_errorReporterFunc;
    Scene* m_baseScene;
    lua_State* L;
    QString m_coreFile;
    QString m_userFile;
};

extern void push_pcall_handler(lua_State* L);
extern int pcall_handler(lua_State* L);

#endif // LUAENGINE_H
