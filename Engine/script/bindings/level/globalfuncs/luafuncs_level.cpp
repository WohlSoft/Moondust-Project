#include <scenes/scene_level.h>
#include "luafuncs_level.h"

void Binding_Level_CommonFuncs::Lua_ToggleSwitch(lua_State *L, int switchID)
{
    LevelScene* scene = LuaGlobal::getLevelEngine(L)->getScene();
    scene->toggleSwitch(switchID);
}

bool Binding_Level_CommonFuncs::Lua_getSwitchState(lua_State *L, int switchID)
{
    LevelScene* scene = LuaGlobal::getLevelEngine(L)->getScene();
    return scene->lua_switchState(switchID);
}

void Binding_Level_CommonFuncs::Lua_triggerEvent(lua_State *L, std::string eventName)
{
    LevelScene* scene = LuaGlobal::getLevelEngine(L)->getScene();
    scene->events.triggerEvent( QString::fromStdString(eventName) );
}

luabind::scope Binding_Level_CommonFuncs::bindToLua()
{
    using namespace luabind;
    return
        namespace_("Level")[
            def("toggleSwitch", &Binding_Level_CommonFuncs::Lua_ToggleSwitch),
            def("getSwitchState", &Binding_Level_CommonFuncs::Lua_getSwitchState),
            def("TriggerEvent", &Binding_Level_CommonFuncs::Lua_triggerEvent)
        ];
}
