/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <QString>
#include <QtDebug>

enum class PGE_LogLevel
{
    Debug       = 5,
    Warning     = 4,
    Critical    = 3,
    Fatal       = 2,
    System      = 1,
    NoLog       = 0,
};

void LoadLogSettings();
void WriteToLog(PGE_LogLevel type, const QString &msg, bool noConsole=false);

// Regular loggers
#define LogDebug(msg)       WriteToLog(PGE_LogLevel::Debug, (msg), false)
#define LogWarning(msg)     WriteToLog(PGE_LogLevel::Warning, (msg), false)
#define LogCritical(msg)    WriteToLog(PGE_LogLevel::Critical, (msg), false)
#define LogFatal(msg)       WriteToLog(PGE_LogLevel::Fatal, (msg), false)

// Regular loggers with disabled console logging
#define LogDebugNC(msg)     WriteToLog(PGE_LogLevel::Debug, (msg), true)
#define LogWarningNC(msg)   WriteToLog(PGE_LogLevel::Warning, (msg), true)
#define LogCriticalNC(msg)  WriteToLog(PGE_LogLevel::Critical, (msg), true)
#define LogFatalNC(msg)     WriteToLog(PGE_LogLevel::Fatal, (msg), true)

// Thread-save loggers
#define LogDebugQD(msg)     qDebug() << (msg)
#define LogWarningQD(msg)   qWarning() << (msg)
#define LogCriticalQD(msg)  qCritical() << (msg)
#define LogFatalQD(msg)     qFatal() << (msg)

#endif // LOGGER_H
