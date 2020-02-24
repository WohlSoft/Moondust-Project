/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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
