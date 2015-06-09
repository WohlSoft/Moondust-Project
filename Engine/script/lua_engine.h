#ifndef LUAENGINE_H
#define LUAENGINE_H

#include <QObject>
#include <QString>
#include <QtDebug>

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

    QString coreFile() const;
    void setCoreFile(const QString &coreFile);

    void dispatchEvent(LuaEvent& toDispatchEvent);
    void setErrorReporterFunc(const std::function<void (const QString &, const QString&)> &func);

    Scene *getBaseScene() const;
protected:
    virtual void onBindAll() {}

    lua_State* getNativeState() {return L; }

private:
    void bindCore();
    void error();

    std::function<void (const QString & /*error message*/, const QString& /*stack trace*/)> m_errorReporterFunc;
    Scene* m_baseScene;
    lua_State* L;
    QString m_coreFile;
};

extern void push_pcall_handler(lua_State* L);
extern int pcall_handler(lua_State* L);

#endif // LUAENGINE_H
