#include <scenes/scene_level.h>
#include "luafuncs_level.h"

void Binding_Level_CommonFuncs::Lua_ToggleSwitch(lua_State *L, int switchID)
{
    LevelScene* scene = LuaGlobal::getLevelEngine(L)->getScene();
    scene->toggleSwitch(switchID);
}

bool Binding_Level_CommonFuncs::Lua_getSwitchState(lua_State *L, uint32_t switchID)
{
    LevelScene* scene = LuaGlobal::getLevelEngine(L)->getScene();
    return scene->lua_switchState(switchID);
}

void Binding_Level_CommonFuncs::Lua_triggerEvent(lua_State *L, std::string eventName)
{
    LevelScene* scene = LuaGlobal::getLevelEngine(L)->getScene();
    scene->m_events.triggerEvent( eventName );
}

void Binding_Level_CommonFuncs::Lua_ShakeScreen(lua_State *L, double forceX, double forceY, double decX, double decY)
{
    LevelScene* scene = LuaGlobal::getLevelEngine(L)->getScene();
    for(LevelScene::LVL_CameraList::iterator it = scene->m_cameras.begin(); it != scene->m_cameras.end(); it++)
    {
        PGE_LevelCamera* cam=&(*it);
        cam->shakeScreen(forceX, forceY, decX, decY);
    }
}

void Binding_Level_CommonFuncs::Lua_ShakeScreenX(lua_State *L, double forceX, double decX)
{
    LevelScene* scene = LuaGlobal::getLevelEngine(L)->getScene();
    for(LevelScene::LVL_CameraList::iterator it = scene->m_cameras.begin(); it != scene->m_cameras.end(); it++)
    {
        PGE_LevelCamera* cam=&(*it);
        cam->shakeScreenX(forceX, decX);
    }
}

void Binding_Level_CommonFuncs::Lua_ShakeScreenY(lua_State *L, double forceY, double decY)
{
    LevelScene* scene = LuaGlobal::getLevelEngine(L)->getScene();
    for(LevelScene::LVL_CameraList::iterator it = scene->m_cameras.begin(); it != scene->m_cameras.end(); it++)
    {
        PGE_LevelCamera* cam=&(*it);
        cam->shakeScreenY(forceY, decY);
    }
}

/***
Level specific functions and classes
@module LevelCommon
*/

luabind::scope Binding_Level_CommonFuncs::bindToLua()
{
    using namespace luabind;
    return
        /***
        Generic level static functions
        @section LevelGeneric
        */
        namespace_("Level")[
            /***
            Toggle state of switch blocks group of given index
            @function Level.toggleSwitch
            @tparam int switchID Index of switch group, defined in the config pack
            */
            def("toggleSwitch", &Binding_Level_CommonFuncs::Lua_ToggleSwitch),

            /***
            Get current state of switch blocks group of given index
            @function Level.getSwitchState
            @tparam int switchID Index of switch group, defined in the config pack
            @treturn bool true or false state
            */
            def("getSwitchState", &Binding_Level_CommonFuncs::Lua_getSwitchState),

            /***
            Execute an event program of current level file
            @function Level.triggerEvent
            @tparam string Name of event program to execute
            */
            def("triggerEvent", &Binding_Level_CommonFuncs::Lua_triggerEvent),

            /***
            Set screen shaking effect (or turn it off by giving of zero force)
            @function Level.shakeScreen
            @tparam double forceX Horizontal shake force (pixels offset at initial position)
            @tparam double forceY Vertical shake force (pixels offset at initial position)
            @tparam double decX Deceleration of horizontal shake force per millisecond. Zero value means keeping a shake effect forever
            @tparam double decY Deceleration of vertical shake force per millisecond. Zero value means keeping a shake effect forever
            */
            def("shakeScreen", &Binding_Level_CommonFuncs::Lua_ShakeScreen),

            /***
            Set screen shaking effect for horizontal dimension (or turn it off by giving of zero force)
            @function Level.shakeScreenX
            @tparam double forceX Horizontal shake force (pixels offset at initial position)
            @tparam double decX Deceleration of horizontal shake force per millisecond. Zero value means keeping a shake effect forever
            */
            def("shakeScreenX", &Binding_Level_CommonFuncs::Lua_ShakeScreenX),

            /***
            Set screen shaking effect for vertical dimension (or turn it off by giving of zero force)
            @function Level.shakeScreenY
            @tparam double forceY Vertical shake force (pixels offset at initial position)
            @tparam double decY Deceleration of vertical shake force per millisecond. Zero value means keeping a shake effect forever
            */
            def("shakeScreenY", &Binding_Level_CommonFuncs::Lua_ShakeScreenY)
        ];
}
