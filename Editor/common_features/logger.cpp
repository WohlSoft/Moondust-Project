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

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QApplication>
#include <QSettings>
#include <QtDebug>
#include <QDate>

#include <dev_console/devconsole.h>

#include "app_path.h"
#include "logger_sets.h"

QString     LogWriter::DebugLogFile;
QtMsgType   LogWriter::logLevel;
bool        LogWriter::enabled;

LogWriterSignal::LogWriterSignal(QObject *parent) : QObject(parent)
{}

LogWriterSignal::LogWriterSignal(DevConsole *console, QObject *parent) : QObject(parent)
{
    setup(console);
}

void LogWriterSignal::setup(DevConsole *console)
{
    connect(this, SIGNAL(logToConsole(QString, QString)), console, SLOT(logMessage(QString,QString)));
}

LogWriterSignal::~LogWriterSignal()
{}

void LogWriterSignal::log(QString msg, QString chan)
{
    emit logToConsole(msg, chan);
}



LogWriterSignal *LogWriter::consoleConnector=NULL;

void LogWriter::LoadLogSettings()
{
    QString logFileName = QString("PGE_Editor_log_%1-%2-%3_%4-%5-%6.txt")
            .arg(QDate().currentDate().year())
            .arg(QDate().currentDate().month())
            .arg(QDate().currentDate().day())
            .arg(QTime().currentTime().hour())
            .arg(QTime().currentTime().minute())
            .arg(QTime().currentTime().second());
    logLevel = QtDebugMsg;

    QString mainIniFile = AppPathManager::settingsFile();
    QSettings logSettings(mainIniFile, QSettings::IniFormat);

    QDir defLogDir(AppPathManager::userAppDir()+"/logs");
    if(!defLogDir.exists())
        if(!defLogDir.mkpath(AppPathManager::userAppDir()+"/logs"))
            defLogDir.setPath(AppPathManager::userAppDir());

    logSettings.beginGroup("logging");
        DebugLogFile = logSettings.value("log-path", defLogDir.absolutePath()+"/"+logFileName).toString();
        if(!QFileInfo(DebugLogFile).absoluteDir().exists())
            DebugLogFile = defLogDir.absolutePath()+"/"+logFileName;
        DebugLogFile = QFileInfo(DebugLogFile).absoluteDir().absolutePath()+"/"+logFileName;

        enabled = true;
        switch( logSettings.value("log-level", "3").toInt() )
        {
            case 4:
                logLevel=QtDebugMsg; break;
            case 3:
                logLevel=QtWarningMsg; break;
            case 2:
                logLevel=QtCriticalMsg; break;
            case 1:
                logLevel=QtFatalMsg; break;
            case 0:
            default:
                enabled=false;
                logLevel=QtFatalMsg; break;
        }

    logSettings.endGroup();
    qDebug()<< QString("LogLevel %1, log file %2").arg(logLevel).arg(DebugLogFile);
    qInstallMessageHandler(logMessageHandler);
}

void LogWriter::WriteToLog(QtMsgType type, QString msg)
{
    if(!enabled) return; //if logging disabled

    QString txt;

    switch (type)
    {
    case QtDebugMsg:
        if(logLevel==QtFatalMsg) return;
    case QtWarningMsg:
        if(logLevel==QtCriticalMsg) return;
    case QtCriticalMsg:
        if(logLevel==QtWarningMsg) return;
    case QtFatalMsg:
        break;
    }

    switch (type)
    {
    case QtDebugMsg:
        txt = QString("Debug: %1").arg(msg);
    break;
    case QtWarningMsg:
        txt = QString("Warning: %1").arg(msg);
    break;
    case QtCriticalMsg:
        txt = QString("Critical: %1").arg(msg);
    break;
    case QtFatalMsg:
        txt = QString("Fatal: %1").arg(msg);
    }

QFile outFile(DebugLogFile);
outFile.open(QIODevice::WriteOnly | QIODevice::Append);
QTextStream ts(&outFile);
ts << txt << endl;
outFile.close();
}

void LogWriter::logMessageHandler(QtMsgType type,
                  const QMessageLogContext& context,
                             const QString& msg)
{
    switch (type)
    {
        case QtDebugMsg:
            if(logLevel==QtWarningMsg) return;
        case QtWarningMsg:
            if(logLevel==QtCriticalMsg) return;
        case QtCriticalMsg:
            if(logLevel==QtFatalMsg) return;
        case QtFatalMsg:
            break;
    }

    QByteArray lMessage = msg.toLocal8Bit();
    QString txt = NULL;
    switch (type)
    {
        case QtDebugMsg:
        txt = QString("Debug (%1:%2, %3): %4")
                .arg(context.file)
                .arg(context.line)
                .arg(context.function)
                .arg(lMessage.constData());
        break;
        case QtWarningMsg:
        txt = QString("Warning: (%1:%2, %3): %4")
                .arg(context.file)
                .arg(context.line)
                .arg(context.function)
                .arg(lMessage.constData());
        break;
        case QtCriticalMsg:
        txt = QString("Critical: (%1:%2, %3): %4")
                .arg(context.file)
                .arg(context.line)
                .arg(context.function)
                .arg(lMessage.constData());
        break;
        case QtFatalMsg:
        txt = QString("Fatal: (%1:%2, %3): %4")
                .arg(context.file)
                .arg(context.line)
                .arg(context.function)
                .arg(lMessage.constData());
    }

    QFile outFile(DebugLogFile);
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << endl;
    outFile.close();

    if(type == QtFatalMsg)
        abort();

    if(!LogWriter::consoleConnector)
        return;

    switch (type) {
    case QtDebugMsg:
        LogWriter::consoleConnector->log(msg, QString("Debug"));
        break;
    case QtWarningMsg:
        LogWriter::consoleConnector->log(msg, QString("Warning"));
        break;
    case QtCriticalMsg:
        LogWriter::consoleConnector->log(msg, QString("Critical"));
        break;
    case QtFatalMsg:
        LogWriter::consoleConnector->log(msg, QString("Fatal"));
        break;
    default:
        LogWriter::consoleConnector->log(msg, QString("System"));
        break;
    }
}

void LogWriter::installConsole(DevConsole* console)
{
    if(consoleConnector)
        delete consoleConnector;
    consoleConnector = new LogWriterSignal;
    consoleConnector->setup(console);
}

void LogWriter::uninstallConsole()
{
    if(consoleConnector)
        delete consoleConnector;
    consoleConnector=NULL;
}

void LoadLogSettings()
{
    LogWriter::LoadLogSettings();
}

void WriteToLog(QtMsgType type, QString msg, bool noConsole)
{
    LogWriter::WriteToLog(type, msg);

    if(noConsole)
        return;
    if(!LogWriter::consoleConnector)
        return;

    switch (type) {
    case QtDebugMsg:
        LogWriter::consoleConnector->log(msg, QString("Debug"));
        break;
    case QtWarningMsg:
        LogWriter::consoleConnector->log(msg, QString("Warning"));
        break;
    case QtCriticalMsg:
        LogWriter::consoleConnector->log(msg, QString("Critical"));
        break;
    case QtFatalMsg:
        LogWriter::consoleConnector->log(msg, QString("Fatal"));
        break;
    default:
        LogWriter::consoleConnector->log(msg, QString("System"));
        break;
    }
}

