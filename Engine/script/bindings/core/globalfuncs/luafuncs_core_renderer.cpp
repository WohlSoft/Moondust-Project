#include "luafuncs_core_renderer.h"

#include <scenes/scene_title.h>
#include <fontman/font_manager.h>

void Binding_Core_GlobalFuncs_Renderer::printText(std::string text, int x, int y, lua_State *L)
{
    printText(text, x, y, FontManager::DefaultRaster, L);
}

void Binding_Core_GlobalFuncs_Renderer::printText(std::string text, int x, int y, int fontType, lua_State *L)
{
    printText(text, x, y, fontType, 14, L);
}

void Binding_Core_GlobalFuncs_Renderer::printText(std::string text, int x, int y, int fontType, int size, lua_State *L)
{
    printText(text, x, y, fontType, size, 0xFFFFFFFF, L);
}

void Binding_Core_GlobalFuncs_Renderer::printText(std::string text, int x, int y, int fontType, int size, unsigned int rgba, lua_State *L)
{
    LuaGlobal::getEngine(L)->getBaseScene()->addRenderFunction([=](){
        FontManager::printText(
                    QString(text.c_str()),
                    x,
                    y,
                    fontType,
                    ((float)((rgba & 0xFF000000) >> 24)) / 255.0f,
                    ((float)((rgba & 0x00FF0000) >> 16)) / 255.0f,
                    ((float)((rgba & 0x0000FF00) >> 8)) / 255.0f,
                    ((float)((rgba & 0x000000FF))) / 255.0f,
                    size);
    });
}


luabind::scope Binding_Core_GlobalFuncs_Renderer::bindToLua()
{
    using namespace luabind;
    return
        namespace_("Renderer")[
            def("printText", (void(*)(std::string, int, int, lua_State*))&printText),
            def("printText", (void(*)(std::string, int, int, int, lua_State*))&printText),
            def("printText", (void(*)(std::string, int, int, int, int, lua_State*))&printText),
            def("printText", (void(*)(std::string, int, int, int, int, unsigned int, lua_State*))&printText)
        ];
}
