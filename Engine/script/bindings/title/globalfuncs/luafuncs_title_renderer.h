#ifndef BINDING_TITLE_RENDERER_H
#define BINDING_TITLE_RENDERER_H

#include <script/lua_global.h>

#include <luabind/luabind.hpp>
#include <lua_inclues/lua.hpp>

class Binding_Title_Renderer
{
public:
    static void printText(std::string text, int x, int y, lua_State* L);

    static luabind::scope bindToLua();
};

#endif // BINDING_TITLE_RENDERER_H
