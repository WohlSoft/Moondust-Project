#include "luafuncs_core_renderer.h"

#include <scenes/scene_title.h>
#include <fontman/font_manager.h>

void Binding_Core_GlobalFuncs_Renderer::printText(std::string text, int x, int y, lua_State *L)
{
    LuaGlobal::getEngine(L)->getBaseScene()->addRenderFunction([=](){
        FontManager::printText(QString(text.c_str()), x, y);
    });
}

luabind::scope Binding_Core_GlobalFuncs_Renderer::bindToLua()
{
    using namespace luabind;
    return
        namespace_("Renderer")[
            def("printText", &printText)
        ];
}
