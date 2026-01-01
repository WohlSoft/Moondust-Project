/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2026 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "js_system.h"

PGE_JS_System::PGE_JS_System(QObject *parent)
    : QObject{parent}
{}

PGE_JS_System::~PGE_JS_System()
{}

QString PGE_JS_System::osName()
{
#if defined(Q_OS_LINUX)
    return "linux";
#elif defined(Q_OS_WIN)
    return "windows";
#elif defined(Q_OS_HAIKU)
    return "haiku";
#elif defined(Q_OS_FREEBSD)
    return "freebsd";
#elif defined(Q_OS_OPENBSD)
    return "openbsd";
#elif defined(Q_OS_NETBSD)
    return "netbsd";
#elif defined(Q_OS_MAC)
    return "macos";
#elif defined(Q_OS_UNIX)
    return "unix";
#else
    return "unknown";
#endif
}
