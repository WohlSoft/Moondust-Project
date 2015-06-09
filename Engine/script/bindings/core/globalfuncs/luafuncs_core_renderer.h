#ifndef BINDING_TITLE_RENDERER_H
#define BINDING_TITLE_RENDERER_H

#include <script/lua_global.h>

#include <luabind/luabind.hpp>
#include <lua_inclues/lua.hpp>

class Binding_Core_GlobalFuncs_Renderer
{
public:
    static void printText(std::string text, int x, int y, lua_State* L);
    static void printText(std::string text, int x, int y, int fontType, lua_State* L);
    static void printText(std::string text, int x, int y, int fontType, int size, lua_State* L);
    static void printText(std::string text, int x, int y, int fontType, int size, unsigned int rgba, lua_State* L);

    static luabind::scope bindToLua();
};

#endif // BINDING_TITLE_RENDERER_H
