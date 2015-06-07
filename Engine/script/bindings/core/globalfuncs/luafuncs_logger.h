#ifndef LOGGER_H
#define LOGGER_H

#include <string>

#include <luabind/luabind.hpp>
#include <lua_inclues/lua.hpp>

class BindingCore_GlobalFuncs_Logger
{
public:

    static void debug(const std::string& msg);
    static void warning(const std::string& msg);
    static void critical(const std::string& msg);

    static luabind::scope bindToLua();

};

#endif // LOGGER_H
