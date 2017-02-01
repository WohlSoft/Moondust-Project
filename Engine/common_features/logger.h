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

#pragma once
#ifndef LOGGER_H
#define LOGGER_H
#include <string>

enum class PGE_LogLevel
{
    Debug    = 4,
    Warning  = 3,
    Critical = 2,
    Fatal    = 1,
    NoLog    = 0,
};

extern void LoadLogSettings();
extern void CloseLog();

extern void pLogDebug(const char *format, ...);
extern void pLogWarning(const char *format, ...);
extern void pLogCritical(const char *format, ...);
extern void pLogFatal(const char *format, ...);
extern void pLogInfo(const char *format, ...);

extern void WriteToLog(PGE_LogLevel type, std::string msg);

#ifdef DEBUG_BUILD
#define D_pLogDebug(fmt, ...) pLogDebug(fmt, ##__VA_ARGS__)
#define D_pLogWarning(fmt, ...) pLogWarning(fmt, ##__VA_ARGS__)
#define D_pLogCritical(fmt, ...) pLogCritical(fmt, ##__VA_ARGS__)
#define D_pLogFatal(fmt, ...) pLogFatal(fmt, ##__VA_ARGS__)
#define D_pLogInfo(fmt, ...) pLogInfo(fmt, ##__VA_ARGS__)
#else
#define D_pLogDebug(fmt, ...)
#define D_pLogWarning(fmt, ...)
#define D_pLogCritical(fmt, ...)
#define D_pLogFatal(fmt, ...)
#define D_pLogInfo(fmt, ...)
#endif

#define LogDebug(msg) WriteToLog(PGE_LogLevel::Debug, msg)
#define LogWarning(msg) WriteToLog(PGE_LogLevel::Warning, msg)
#define LogCritical(msg) WriteToLog(PGE_LogLevel::Critical, msg)
#define LogFatal(msg) WriteToLog(PGE_LogLevel::Fatal, msg)

#endif // LOGGER_H
