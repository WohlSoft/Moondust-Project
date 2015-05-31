#ifndef LUAENGINE_H
#define LUAENGINE_H

#include <QObject>
#include <QString>
#include <QtDebug>
#include <luabind/luabind.hpp>

extern "C"{
#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>
}

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
private:
    Q_DISABLE_COPY(LuaEngine)

public:
    LuaEngine();
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
    inline bool isValid() { return L != NULL; }

    QString coreFile() const;
    void setCoreFile(const QString &coreFile);

protected:
    virtual void registerFunctions() {}



private:
    lua_State* L;
    QString m_coreFile;
};

#endif // LUAENGINE_H
