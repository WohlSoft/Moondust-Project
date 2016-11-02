#include "luafuncs_core_logger.h"
#include <common_features/logger.h>

void Binding_Core_GlobalFuncs_Logger::debug(const std::string &msg)
{
    pLogDebug(msg.c_str());
}

void Binding_Core_GlobalFuncs_Logger::warning(const std::string &msg)
{
    pLogDebug(msg.c_str());
}

void Binding_Core_GlobalFuncs_Logger::critical(const std::string &msg)
{
    pLogDebug(msg.c_str());
}

luabind::scope Binding_Core_GlobalFuncs_Logger::bindToLua()
{
    using namespace luabind;
    return
        namespace_("Logger")[
            def("debug", &Binding_Core_GlobalFuncs_Logger::debug),
            def("warning", &Binding_Core_GlobalFuncs_Logger::warning),
            def("critical", &Binding_Core_GlobalFuncs_Logger::critical)
        ];
}
