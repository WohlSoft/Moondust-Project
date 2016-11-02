#pragma once
#ifndef LUA_LOGGER_H
#define LUA_LOGGER_H

#include <string>

#include <luabind/luabind.hpp>
#include <lua_inclues/lua.hpp>

class Binding_Core_GlobalFuncs_Logger
{
    public:
        static void debug(const std::string &msg);
        static void warning(const std::string &msg);
        static void critical(const std::string &msg);

        static luabind::scope bindToLua();

};

#endif // LUA_LOGGER_H
