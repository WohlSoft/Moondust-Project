/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef LOGGER_H
#define LOGGER_H
#include <QString>
#include <QtMsgHandler>

enum class PGE_LogLevel {
    Debug=4,
    Warning=3,
    Critical=2,
    Fatal=1,
    NoLog=0,
};

void LoadLogSettings();
void WriteToLog(PGE_LogLevel type, QString msg, bool noConsole=false);

#define LogDebug(msg) WriteToLog(PGE_LogLevel::Debug, msg, false);
#define LogWarning(msg) WriteToLog(PGE_LogLevel::Warning, msg, false);
#define LogCritical(msg) WriteToLog(PGE_LogLevel::Critical, msg, false);
#define LogFatal(msg) WriteToLog(PGE_LogLevel::Fatal, msg, false);

#define LogDebugNC(msg) WriteToLog(PGE_LogLevel::Debug, msg, true);
#define LogWarningNC(msg) WriteToLog(PGE_LogLevel::Warning, msg, true);
#define LogCriticalNC(msg) WriteToLog(PGE_LogLevel::Critical, msg, true);
#define LogFatalNC(msg) WriteToLog(PGE_LogLevel::Fatal, msg, true);

#endif // LOGGER_H
