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
    static void printText_LUNA(std::string text, int fontType, int x, int y, lua_State *L);
    static void printText(std::string text, int x, int y, int fontType, int size, lua_State* L);
    static void printText(std::string text, int x, int y, int fontType, int size, uint32_t rgba, lua_State* L);

    static void printTextWP(std::string text, int x, int y, long double zorder, lua_State* L);
    static void printTextWP(std::string text, int x, int y, int fontType, long double zorder, lua_State* L);
    static void printTextWP_LUNA(std::string text, int fontType, int x, int y, long double zorder, lua_State *L);
    static void printTextWP(std::string text, int x, int y, int fontType, int size, long double zorder, lua_State* L);
    static void printTextWP(std::string text, int x, int y, int fontType, int size, uint32_t rgba, long double zorder, lua_State* L);

    static void showMessageBox(std::string text, lua_State *L);

    static luabind::scope bindToLuaRenderer();
    static luabind::scope bindToLuaText();

};

#endif // BINDING_TITLE_RENDERER_H
