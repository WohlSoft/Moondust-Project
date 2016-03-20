#include <scenes/scene_level.h>
#include "luafuncs_level.h"

void Binding_Level_CommonFuncs::Lua_ToggleSwitch(lua_State *L, int switchID)
{
    LevelScene* scene = LuaGlobal::getLevelEngine(L)->getScene();
    scene->toggleSwitch(switchID);
}

luabind::scope Binding_Level_CommonFuncs::bindToLua()
{
    using namespace luabind;
    return
        namespace_("Level")[
            def("toggleSwitch", &Binding_Level_CommonFuncs::Lua_ToggleSwitch)
        ];
}
