#include "luafuncs_core_renderer.h"

#include <scenes/scene.h>
#include <fontman/font_manager.h>

#define DEFAULT_FONT_SIZE 14
#define DEFAULT_ZORDER 3.0L

void Binding_Core_GlobalFuncs_Renderer::printText(std::string text, int x, int y, lua_State *L)
{
    printTextWP(text, x, y, FontManager::DefaultRaster, DEFAULT_ZORDER, L);
}

void Binding_Core_GlobalFuncs_Renderer::printText(std::string text, int x, int y, int fontType, lua_State *L)
{
    printTextWP(text, x, y, fontType, DEFAULT_FONT_SIZE, DEFAULT_ZORDER, L);
}

void Binding_Core_GlobalFuncs_Renderer::printText_LUNA(std::string text, int fontType, int x, int y, lua_State *L)
{
    int textcolor=0xFFFFFFFF;
    if(fontType==2)
        textcolor = 0x000000FF;
    printTextWP(text, x, y, fontType, DEFAULT_FONT_SIZE, textcolor, DEFAULT_ZORDER, L);
}

void Binding_Core_GlobalFuncs_Renderer::printText(std::string text, int x, int y, int fontType, int size, lua_State *L)
{
    printTextWP(text, x, y, fontType, size, 0xFFFFFFFF, DEFAULT_ZORDER, L);
}

void Binding_Core_GlobalFuncs_Renderer::printText(std::string text, int x, int y, int fontType, int size, unsigned int rgba, lua_State *L)
{
    printTextWP(text, x, y, fontType, size, rgba, DEFAULT_ZORDER, L);
}

void Binding_Core_GlobalFuncs_Renderer::printTextWP(std::string text, int x, int y, long double zorder, lua_State *L)
{
    printTextWP(text, x, y, FontManager::DefaultRaster, zorder, L);
}

void Binding_Core_GlobalFuncs_Renderer::printTextWP(std::string text, int x, int y, int fontType, long double zorder, lua_State *L)
{
    printTextWP(text, x, y, fontType, DEFAULT_FONT_SIZE, zorder, L);
}

void Binding_Core_GlobalFuncs_Renderer::printTextWP_LUNA(std::string text, int fontType, int x, int y, long double zorder, lua_State *L)
{
    int textcolor=0xFFFFFFFF;
    if(fontType==2)
        textcolor = 0x000000FF;
    printTextWP(text, x, y, fontType, DEFAULT_FONT_SIZE, textcolor, zorder, L);
}

void Binding_Core_GlobalFuncs_Renderer::printTextWP(std::string text, int x, int y, int fontType, int size, long double zorder, lua_State *L)
{
    printTextWP(text, x, y, fontType, size, 0xFFFFFFFF, zorder, L);
}

void Binding_Core_GlobalFuncs_Renderer::printTextWP(std::string text, int x, int y, int fontType, int size, unsigned int rgba, long double zorder, lua_State *L)
{
    LuaGlobal::getEngine(L)->getBaseScene()->renderArrayAddFunction([=](){
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
    }, zorder);
}

void Binding_Core_GlobalFuncs_Renderer::showMessageBox(std::string text, lua_State* L)
{
    LuaGlobal::getEngine(L)->getBaseScene()->m_messages.showMsg(text);
}

luabind::scope Binding_Core_GlobalFuncs_Renderer::bindToLuaRenderer()
{
    using namespace luabind;
    return
        namespace_("Renderer")[
            def("printText", (void(*)(std::string, int, int, lua_State*))&printText),
            def("printText", (void(*)(std::string, int, int, int, lua_State*))&printText),
            def("printText", (void(*)(std::string, int, int, int, int, lua_State*))&printText),
            def("printText", (void(*)(std::string, int, int, int, int, unsigned int, lua_State*))&printText),
            def("printTextWP", (void(*)(std::string, int, int, long double, lua_State*))&printTextWP),
            def("printTextWP", (void(*)(std::string, int, int, int, long double, lua_State*))&printTextWP),
            def("printTextWP", (void(*)(std::string, int, int, int, int, long double, lua_State*))&printTextWP),
            def("printTextWP", (void(*)(std::string, int, int, int, int, unsigned int, long double, lua_State*))&printTextWP)
            ];
}

luabind::scope Binding_Core_GlobalFuncs_Renderer::bindToLuaText()
{
    using namespace luabind;
    return
        namespace_("Text")[
            def("print", (void(*)(std::string, int, int, lua_State*))&printText),
            def("print", (void(*)(std::string, int, int, int, lua_State*))&printText_LUNA),
            def("printWP", (void(*)(std::string, int, int, long double, lua_State*))&printTextWP),
            def("printWP", (void(*)(std::string, int, int, int, long double, lua_State*))&printTextWP_LUNA),
            def("showMessageBox", &showMessageBox)
            ];
}
