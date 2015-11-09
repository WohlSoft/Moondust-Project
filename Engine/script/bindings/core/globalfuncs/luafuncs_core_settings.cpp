#include "luafuncs_core_settings.h"

#include <settings/global_settings.h>
#include <graphics/window.h>

bool Binding_Core_GlobalFuncs_Settings::isFullscreenActive()
{
    return AppSettings.fullScreen;
}

int Binding_Core_GlobalFuncs_Settings::getScreenWidth()
{
    return AppSettings.ScreenWidth;
}

int Binding_Core_GlobalFuncs_Settings::getScreenHeight()
{
    return AppSettings.ScreenHeight;
}

//int Binding_Core_GlobalFuncs_Settings::getMaxFPS()
//{
//    return AppSettings.MaxFPS;
//}

int Binding_Core_GlobalFuncs_Settings::getTicksPerSecond()
{
    return AppSettings.TicksPerSecond;
}

bool Binding_Core_GlobalFuncs_Settings::useFrameSkip()
{
    return AppSettings.frameSkip;
}

int Binding_Core_GlobalFuncs_Settings::getSoundVolume()
{
    return AppSettings.volume_sound;
}

int Binding_Core_GlobalFuncs_Settings::getMusicVolume()
{
    return AppSettings.volume_music;
}

bool Binding_Core_GlobalFuncs_Settings::isDebugInfoShown()
{
    return PGE_Window::showDebugInfo;
}

void Binding_Core_GlobalFuncs_Settings::setDebugInfoShown(bool debugInfoShown)
{
    PGE_Window::showDebugInfo = debugInfoShown; //Actual usable flag is here!
    AppSettings.showDebugInfo = debugInfoShown; //this flag holds to be saved/loaded from config file
                                                //(but later I will move from PGE Window into settings to don't have this confusion)
}

luabind::scope Binding_Core_GlobalFuncs_Settings::bindToLua()
{
    using namespace luabind;
    return
        namespace_("Settings")[
            def("isFullscreenActive", &isFullscreenActive),
            def("getScreenWidth", &getScreenWidth),
            def("getScreenHeight", &getScreenHeight),
            //def("getMaxFPS", &getMaxFPS),// Useless field, never used
            def("getTicksPerSecond", &getTicksPerSecond),
            def("useFrameSkip", &useFrameSkip),
            def("getSoundVolume", &getSoundVolume),
            def("getMusicVolume", &getMusicVolume),
            def("isDebugInfoShown", &isDebugInfoShown),
            def("setDebugInfoShown", &setDebugInfoShown)
        ];
}
