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

/***
Engine logger functions
@module LoggerFuncs
*/

luabind::scope Binding_Core_GlobalFuncs_Logger::bindToLua()
{
    using namespace luabind;
    return
        namespace_("Logger")[
            /***
            Write log message of Debug level
            @function Logger.debug
            @tparam string msg Message to write into log file
            */
            def("debug", &Binding_Core_GlobalFuncs_Logger::debug),

            /***
            Write log message of Warning level
            @function Logger.warning
            @tparam string msg Message to write into log file
            */
            def("warning", &Binding_Core_GlobalFuncs_Logger::warning),

            /***
            Write log message of Critical level
            @function Logger.critical
            @tparam string msg Message to write into log file
            */
            def("critical", &Binding_Core_GlobalFuncs_Logger::critical)
        ];
}
