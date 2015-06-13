#include "luafuncs_core_logger.h"

#include <QtDebug>

void Binding_Core_GlobalFuncs_Logger::debug(const std::string &msg)
{
    qDebug() << msg.c_str();
}

void Binding_Core_GlobalFuncs_Logger::warning(const std::string &msg)
{
    qWarning() << msg.c_str();
}

void Binding_Core_GlobalFuncs_Logger::critical(const std::string &msg)
{
    qCritical() << msg.c_str();
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
