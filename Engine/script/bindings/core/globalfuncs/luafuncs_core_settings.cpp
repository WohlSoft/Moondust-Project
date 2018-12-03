/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "luafuncs_core_settings.h"

#include <settings/global_settings.h>
#include <graphics/window.h>

bool Binding_Core_GlobalFuncs_Settings::isFullscreenActive()
{
    return g_AppSettings.fullScreen;
}

int Binding_Core_GlobalFuncs_Settings::getScreenWidth()
{
    return g_AppSettings.ScreenWidth;
}

int Binding_Core_GlobalFuncs_Settings::getScreenHeight()
{
    return g_AppSettings.ScreenHeight;
}

//int Binding_Core_GlobalFuncs_Settings::getMaxFPS()
//{
//    return AppSettings.MaxFPS;
//}

double Binding_Core_GlobalFuncs_Settings::getFrameRate()
{
    return g_AppSettings.frameRate;
}

double Binding_Core_GlobalFuncs_Settings::getFrameDelay()
{
    return g_AppSettings.timeOfFrame;
}

bool Binding_Core_GlobalFuncs_Settings::useFrameSkip()
{
    return g_AppSettings.frameSkip;
}

int Binding_Core_GlobalFuncs_Settings::getSoundVolume()
{
    return g_AppSettings.volume_sound;
}

int Binding_Core_GlobalFuncs_Settings::getMusicVolume()
{
    return g_AppSettings.volume_music;
}

bool Binding_Core_GlobalFuncs_Settings::isDebugInfoShown()
{
    return PGE_Window::showDebugInfo;
}

void Binding_Core_GlobalFuncs_Settings::setDebugInfoShown(bool debugInfoShown)
{
    PGE_Window::showDebugInfo = debugInfoShown; //Actual usable flag is here!
    g_AppSettings.showDebugInfo = debugInfoShown; //this flag holds to be saved/loaded from config file
                                                //(but later I will move from PGE Window into settings to don't have this confusion)
}

/***
Manager of Game Settings
@module SettingsManager
*/

luabind::scope Binding_Core_GlobalFuncs_Settings::bindToLua()
{
    using namespace luabind;
    return
        namespace_("Settings")[
            /***
            Identifies is full screen mode is active or not
            @function Settings.isFullscreenActive
            @treturn bool true if is a fullscree mode, or false if windowed.
            */
            def("isFullscreenActive", &isFullscreenActive),

            /***
            Get screen width settings (don't returns real screen width)
            @function Settings.getScreenWidth
            @treturn int Width of screen from settings
            */
            def("getScreenWidth", &getScreenWidth),

            /***
            Get screen height settings (don't returns real screen height)
            @function Settings.getScreenHeight
            @return int Height of screen from settings
            */
            def("getScreenHeight", &getScreenHeight),

            //def("getMaxFPS", &getMaxFPS),// Useless field, never used

            /***
            Get pre-confirued framerate value
            @function Settings.getFrameRate
            @treturn double Frames per second value
            */
            def("getFrameRate", &getFrameRate),

            /***
            Get pre-confirued framerate value, alias to @{Settings.getFrameRate}
            @function Settings.getTicksPerSecond
            @treturn double Frames per second value
            */
            def("getTicksPerSecond", &getFrameRate),

            /***
            Get a frame skip value
            @function Settings.useFrameSkip
            @treturn bool true if frameskip is turned on
            */
            def("useFrameSkip", &useFrameSkip),

            /***
            Get a master sound volume value
            @function Settings.getSoundVolume
            @treturn int Sound volume value between 0 and 128
            */
            def("getSoundVolume", &getSoundVolume),

            /***
            Get a master music volume value
            @function Settings.getMusicVolume
            @treturn int Sound volume value between 0 and 128
            */
            def("getMusicVolume", &getMusicVolume),

            /***
            Is debug information showing is turned on?
            @function Settings.isDebugInfoShown
            @treturn bool state of debug information visibility
            */
            def("isDebugInfoShown", &isDebugInfoShown),

            /***
            Turn on debug information visibility
            @function Settings.setDebugInfoShown
            @tparam bool State of debug information visibility
            */
            def("setDebugInfoShown", &setDebugInfoShown)
        ];
}
