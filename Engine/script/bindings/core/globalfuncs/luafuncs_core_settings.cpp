#include "luafuncs_core_settings.h"

#include <settings/global_settings.h>

bool Binding_Core_GlobalFuncs_Settings::isDebugInfoShown()
{
    return AppSettings.showDebugInfo;
}

void Binding_Core_GlobalFuncs_Settings::setDebugInfoShown(bool debugInfoShown)
{
    AppSettings.showDebugInfo = debugInfoShown;
}

luabind::scope Binding_Core_GlobalFuncs_Settings::bindToLua()
{
    using namespace luabind;
    return
        namespace_("Settings")[
            def("isDebugInfoShown", &isDebugInfoShown),
            def("setDebugInfoShown", &setDebugInfoShown)
        ];
}
