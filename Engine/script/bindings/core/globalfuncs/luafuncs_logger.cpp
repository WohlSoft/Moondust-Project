#include "luafuncs_logger.h"

#include <QtDebug>

void Binding_Logger::debug(const std::string &msg)
{
    qDebug() << msg.c_str();
}

void Binding_Logger::warning(const std::string &msg)
{
    qWarning() << msg.c_str();
}

void Binding_Logger::critical(const std::string &msg)
{
    qCritical() << msg.c_str();
}

void Binding_Logger::bindToLua(lua_State *L)
{
    using namespace luabind;

    module(L)[
        namespace_("Logger")[
            def("debug", &Binding_Logger::debug),
            def("warning", &Binding_Logger::warning),
            def("critical", &Binding_Logger::critical)
        ]

    ];
}
