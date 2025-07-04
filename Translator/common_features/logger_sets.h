/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2025 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef LOGGER_SETS_H
#define LOGGER_SETS_H

#include <QString>
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QtMsgHandler>
#endif

class LogWriter
{
public:
    static QString DebugLogFile;
    static QtMsgType logLevel;
    static bool   enabled;

    static void WriteToLog(QtMsgType type, QString msg);
    static void LoadLogSettings();

    static void logMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
};


#endif // LOGGER_SETS_H
