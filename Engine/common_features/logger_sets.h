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

#ifndef LOGGER_SETS_H
#define LOGGER_SETS_H

#include <QString>
#include <QtMsgHandler>
#include <QFile>
#include <QTextStream>
#include <memory>

#include "logger.h"

class LogWriter
{
public:
    static QString m_logFilePath;
    static PGE_LogLevel m_logLevel;
    //! Is logging system is enabmed
    static bool  m_enabled;
    //! Is log file is opened
    static bool  m_logIsOpened;
    //! Output file
    static FILE*                        m_logout;
    //! Output text stream
    static std::shared_ptr<QTextStream> m_out_stream;

    static void WriteToLog(PGE_LogLevel type, QString msg);
    static void LoadLogSettings();

    static void logMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
};

#endif // LOGGER_SETS_H
