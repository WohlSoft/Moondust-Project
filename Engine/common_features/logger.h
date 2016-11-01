/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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

enum class PGE_LogLevel
{
    Debug    = 4,
    Warning  = 3,
    Critical = 2,
    Fatal    = 1,
    NoLog    = 0,
};

void LoadLogSettings();
void CloseLog();

void pLogDebug(const char *format, ...);
void pLogWarning(const char *format, ...);
void pLogCritical(const char *format, ...);
void pLogFatal(const char *format, ...);
void pLogInfo(const char *format, ...);

void WriteToLog(PGE_LogLevel type, QString msg);

#define LogDebug(msg) WriteToLog(PGE_LogLevel::Debug, msg)
#define LogWarning(msg) WriteToLog(PGE_LogLevel::Warning, msg)
#define LogCritical(msg) WriteToLog(PGE_LogLevel::Critical, msg)
#define LogFatal(msg) WriteToLog(PGE_LogLevel::Fatal, msg)

#endif // LOGGER_H
