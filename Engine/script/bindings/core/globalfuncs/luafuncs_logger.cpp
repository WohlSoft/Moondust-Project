#include "luafuncs_logger.h"

#include <QtDebug>

void BindingCore_GlobalFuncs_Logger::debug(const std::string &msg)
{
    qDebug() << msg.c_str();
}

void BindingCore_GlobalFuncs_Logger::warning(const std::string &msg)
{
    qWarning() << msg.c_str();
}

void BindingCore_GlobalFuncs_Logger::critical(const std::string &msg)
{
    qCritical() << msg.c_str();
}

void BindingCore_GlobalFuncs_Logger::bindToLua(lua_State *L)
{
    using namespace luabind;

    module(L)[
        namespace_("Logger")[
            def("debug", &BindingCore_GlobalFuncs_Logger::debug),
            def("warning", &BindingCore_GlobalFuncs_Logger::warning),
            def("critical", &BindingCore_GlobalFuncs_Logger::critical)
        ]

    ];
}
