/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#include "luafuncs_core_renderer.h"

#include <scenes/scene.h>
#include <fontman/font_manager.h>
#include <graphics/window.h>

#define DEFAULT_FONT_SIZE 14
#define DEFAULT_ZORDER 3.0L

int Binding_Core_GlobalFuncs_Renderer::getFontID(std::string fontName, lua_State *)
{
    return FontManager::getFontID(fontName);
}

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
    uint32_t textcolor = 0xFFFFFFFF;
    if(fontType == 2)
        textcolor = 0x000000FF;
    printTextWP(text, x, y, fontType, DEFAULT_FONT_SIZE, textcolor, DEFAULT_ZORDER, L);
}

void Binding_Core_GlobalFuncs_Renderer::printText(std::string text, int x, int y, int fontType, uint32_t size, lua_State *L)
{
    printTextWP(text, x, y, fontType, size, 0xFFFFFFFF, DEFAULT_ZORDER, L);
}

void Binding_Core_GlobalFuncs_Renderer::printText(std::string text, int x, int y, int fontType, uint32_t size, uint32_t rgba, lua_State *L)
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
    uint32_t textcolor = 0xFFFFFFFF;
    if(fontType == 2)
        textcolor = 0x000000FF;
    printTextWP(text, x, y, fontType, DEFAULT_FONT_SIZE, textcolor, zorder, L);
}

void Binding_Core_GlobalFuncs_Renderer::printTextWP(std::string text, int x, int y, int fontType, uint32_t size, long double zorder, lua_State *L)
{
    printTextWP(text, x, y, fontType, size, 0xFFFFFFFF, zorder, L);
}

void Binding_Core_GlobalFuncs_Renderer::printTextWP(std::string text, int x, int y, int fontType, uint32_t size, uint32_t rgba, long double zorder, lua_State *L)
{
    LuaGlobal::getEngine(L)->getBaseScene()->renderArrayAddFunction([ = ](double /*CameraX*/, double /*CameraY*/)
    {
        FontManager::printText(
            text.c_str(),
            x,
            y,
            fontType,
            float((rgba & 0xFF000000) >> 24) / 255.0f,
            float((rgba & 0x00FF0000) >> 16) / 255.0f,
            float((rgba & 0x0000FF00) >> 8) / 255.0f,
            float((rgba & 0x000000FF)) / 255.0f,
            size);
    }, zorder);
}

void Binding_Core_GlobalFuncs_Renderer::showMessageBox(std::string text, lua_State *L)
{
    LuaGlobal::getEngine(L)->getBaseScene()->m_messages.showMsg(text);
}

void Binding_Core_GlobalFuncs_Renderer::showExternalMessageBox(std::string text, lua_State *)
{
    PGE_Window::msgBoxInfo("Lua debug message", text);
}



/***
Text printing and drawing functions
@module TextAndRenderFuncs
*/

void Binding_Core_GlobalFuncs_Renderer::bindConstants(lua_State *L)
{
    luabind::object _G = luabind::globals(L);
    if(luabind::type(_G["FontType"]) != LUA_TNIL)
        return;
    /***
    Enums
    @section FontType
    */

    /***
    Built-in font types are can be passed as font ID for built-in fonts
    @enum FontType

    @field DefaultRaster Use default raster font
    @field DefaultTTF Use default TTF font
    */

    luabind::object fontTypes = luabind::newtable(L);
    fontTypes["DefaultRaster"] = FontManager::DefaultRaster;
    fontTypes["DefaultTTF"] = FontManager::DefaultTTF_Font;
    _G["FontType"] = fontTypes;
}

luabind::scope Binding_Core_GlobalFuncs_Renderer::bindToLuaFont()
{
    using namespace luabind;
    return
        /***
        Font manager functions
        @section FontNamespace
         */
        namespace_("Font")[
            /***
            Get ID of the font by name
            @function Font.getId
            @tparam string text Textual name of the font
            @treturn int ID of the font which can be used in print functions
            */
            def("getId", &getFontID)
        ];
}

luabind::scope Binding_Core_GlobalFuncs_Renderer::bindToLuaRenderer()
{
    using namespace luabind;
    return
        /***
        Renderer functions
        @section RendererNamespace
         */
        namespace_("Renderer")[
            /***
            Draw a text with default capabilities
            @function Renderer.printText
            @tparam string text Text to print
            @tparam int x X position on screen
            @tparam int y Y position on screen
            */
            def("printText", (void(*)(std::string, int, int, lua_State *))&printText),
            /***
            Draw a text with default capabilities by specified font
            @function Renderer.printText
            @tparam string text Text to print
            @tparam int x X position on screen
            @tparam int y Y position on screen
            @tparam int fontType ID of a font registered in config pack. @{FontType} enum also can be used here.
            */
            def("printText", (void(*)(std::string, int, int, int, lua_State *))&printText),
            /***
            Draw a text by specified font and custom pixel size
            @function Renderer.printText
            @tparam string text Text to print
            @tparam int x X position on screen
            @tparam int y Y position on screen
            @tparam int fontType ID of a font registered in config pack. @{FontType} enum also can be used here.
            @tparam uint size Pixel size of one font letter
            */
            def("printText", (void(*)(std::string, int, int, int, uint32_t, lua_State *))&printText),
            /***
            Draw a text with by specified font, pixel size, and RGBA-color
            @function Renderer.printText
            @tparam string text Text to print
            @tparam int x X position on screen
            @tparam int y Y position on screen
            @tparam int fontType ID of a font registered in config pack. @{FontType} enum also can be used here.
            @tparam uint size Pixel size of one font letter
            @tparam uint rgba RGBA (Red-Green-Blue-Alpha) color value in next format: 0x<span style="color:red;">RR</span><span style="color:green;">GG</span><span style="color:blue;">BB</span><span style="color:gray;">AA</span>
            */
            def("printText", (void(*)(std::string, int, int, int, uint32_t, uint32_t, lua_State *))&printText),

            /***
            Draw a text with default capabilities with render prioritizing
            @function Renderer.printTextWP
            @tparam string text Text to print
            @tparam int x X position on screen
            @tparam int y Y position on screen
            @tparam double zValue Z Value which defines a render priority
            */
            def("printTextWP", (void(*)(std::string, int, int, long double, lua_State *))&printTextWP),
            /***
            Draw a text with default capabilities by specified font with render prioritizing
            @function Renderer.printTextWP
            @tparam string text Text to print
            @tparam int x X position on screen
            @tparam int y Y position on screen
            @tparam int fontType ID of a font registered in config pack. @{FontType} enum also can be used here.
            @tparam double zValue Z Value which defines a render priority
            */
            def("printTextWP", (void(*)(std::string, int, int, int, long double, lua_State *))&printTextWP),
            /***
            Draw a text by specified font, custom pixel size, and render prioritizing
            @function Renderer.printTextWP
            @tparam string text Text to print
            @tparam int x X position on screen
            @tparam int y Y position on screen
            @tparam int fontType ID of a font registered in config pack. @{FontType} enum also can be used here.
            @tparam uint size Pixel size of one font letter
            @tparam double zValue Z Value which defines a render priority
            */
            def("printTextWP", (void(*)(std::string, int, int, int, uint32_t, long double, lua_State *))&printTextWP),
            /***
            Draw a text with by specified font, pixel size, RGBA-color, and render prioritizing
            @function Renderer.printTextWP
            @tparam string text Text to print
            @tparam int x X position on screen
            @tparam int y Y position on screen
            @tparam int fontType ID of a font registered in config pack. @{FontType} enum also can be used here.
            @tparam uint size Pixel size of one font letter
            @tparam uint rgba RGBA (Red-Green-Blue-Alpha) color value in next format: 0x<span style="color:red;">RR</span><span style="color:green;">GG</span><span style="color:blue;">BB</span><span style="color:gray;">AA</span>
            @tparam double zValue Z Value which defines a render priority
            */
            def("printTextWP", (void(*)(std::string, int, int, int, uint32_t, uint32_t, long double, lua_State *))&printTextWP)
        ];
}

luabind::scope Binding_Core_GlobalFuncs_Renderer::bindToLuaText()
{
    using namespace luabind;
    return
        /***
        LunaLua text functions. Added for compatibility with LunaLua API.
        @section TextNamespace
         */
        namespace_("Text")[
            /***
            Writes debugText in an external message box and shows it to the user.
            @function Text.windowDebug
            @tparam string debugText Message text to show
            */
            def("windowDebug", &showExternalMessageBox),

            /***
            Draw a text with default capabilities. Same as @{Renderer.printText}.
            @function Text.print
            @tparam string text Text to print
            @tparam int x X position on screen
            @tparam int y Y position on screen
            */
            def("print", (void(*)(std::string, int, int, lua_State *))&printText),

            /***
            Draw a text with default capabilities and specified font
            @function Text.print
            @tparam string text Text to print
            @tparam int fontType Fond type ID (<u>Note:</u> font #2 will be shown in black color)
            @tparam int x X position on screen
            @tparam int y Y position on screen
            */
            def("print", (void(*)(std::string, int, int, int, lua_State *))&printText_LUNA),

            /***
            Draw a text with default capabilities and render prioritizing. Same as @{Renderer.printTextWP}.
            @function Text.printWP
            @tparam string text Text to print
            @tparam int x X position on screen
            @tparam int y Y position on screen
            @tparam double zValue Z Value which defines a render priority
            */
            def("printWP", (void(*)(std::string, int, int, long double, lua_State *))&printTextWP),
            /***
            Draw a text with default capabilities, specified font, and render prioritizing
            @function Text.printWP
            @tparam string text Text to print
            @tparam int fontType Fond type ID (<u>Note:</u> font #2 will be shown in black color)
            @tparam int x X position on screen
            @tparam int y Y position on screen
            @tparam double zValue Z Value which defines a render priority
            */
            def("printWP", (void(*)(std::string, int, int, int, long double, lua_State *))&printTextWP_LUNA),

            /***
            Displays In-Game Message Box.<br>
            As difference to LunaLua, this message box works everywhere while in LunaLua works on levels only.
            @function Text.showMessageBox
            @tparam string msgText Message text
            */
            def("showMessageBox", &showMessageBox)
            ];
}

luabind::scope Binding_Core_GlobalFuncs_Renderer::bindToLuaDeprecated()
{
    /***
    Deprecated LunaLua functions. Added for compatibility with LunaLua API.
    @section DeprecatedTextNamespace
    */
    using namespace luabind;
    return
            /***
            Draw a text with default capabilities <b>[Deprecated, use @{Text.print} or @{Renderer.printText}]</b>
            @function printText
            @see Text.print
            @tparam string text Text to print
            @tparam int x X position on screen
            @tparam int y Y position on screen
            */
            def("printText", (void(*)(std::string, int, int, lua_State *))&printText),

            /***
            Draw a text with default capabilities and specified font <b>[Deprecated, use @{Text.print} or @{Renderer.printText}]</b>
            @function printText
            @see Text.print
            @tparam string text Text to print
            @tparam int fontType Fond type ID (<u>Note:</u> font #2 will be shown in black color)
            @tparam int x X position on screen
            @tparam int y Y position on screen
            */
            def("printText", (void(*)(std::string, int, int, int, lua_State *))&printText_LUNA),

            /***
            Writes debugText in an external message box and shows it to the user  <b>[Deprecated, use @{Text.windowDebug}]</b>
            @function windowDebug
            @tparam string debugText Message text to show
            */
            def("windowDebug", &showExternalMessageBox)
            ;
}
