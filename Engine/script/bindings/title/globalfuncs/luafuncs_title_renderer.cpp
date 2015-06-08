#include "luafuncs_title_renderer.h"

#include <scenes/scene_title.h>
#include <fontman/font_manager.h>

void Binding_Title_Renderer::printText(std::string text, int x, int y, lua_State *L)
{
    LuaGlobal::getTitleScreenEngine(L)->getScene()->addRenderFunction([=](){
        FontManager::printText(QString(text.c_str()), x, y);
    });
}

luabind::scope Binding_Title_Renderer::bindToLua()
{
    using namespace luabind;
    return
        namespace_("Renderer")[
            def("printText", &printText)
        ];
}
