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

#ifndef LOGGER_SETS_H
#define LOGGER_SETS_H

#include <QString>
#include <QtMsgHandler>
#include <QObject>

class DevConsole;

class LogWriterSignal :public QObject
{
    Q_OBJECT

    friend class LogWriter;
    friend void WriteToLog(QtMsgType type, QString msg, bool noConsole);
    LogWriterSignal(QObject *parent=0);
    LogWriterSignal(DevConsole *console, QObject *parent=0);
    void setup(DevConsole*console);
    virtual ~LogWriterSignal();
    void log(QString msg, QString chan);

signals:
    void logToConsole(QString msg, QString chan);//!<msg, channel
};

class LogWriter
{
public:
    static QString DebugLogFile;
    static QtMsgType logLevel;
    static bool   enabled;

    static void WriteToLog(QtMsgType type, QString msg);
    static void LoadLogSettings();

    static void logMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    static void installConsole(DevConsole *console);
    static void uninstallConsole();
private:
    friend void ::WriteToLog(QtMsgType type, QString msg, bool noConsole);
    static LogWriterSignal *consoleConnector;
};


#endif // LOGGER_SETS_H

