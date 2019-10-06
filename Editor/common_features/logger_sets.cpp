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

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QApplication>
#include <QSettings>
#include <QtDebug>
#include <QDate>
#include <QMutex>
#include <QMutexLocker>
#include <QComboBox>

#include <dev_console/devconsole.h>

#include "crashhandler.h"

#include "app_path.h"
#include "logger_sets.h"

#ifdef DEBUG_BUILD
#define ONSCREEN_LOGGING
#endif


QString         LogWriter::DebugLogFile;
PGE_LogLevel    LogWriter::logLevel;

static PGE_LogLevel qMsg2PgeLL(QtMsgType type)
{
    switch(type)
    {
    case QtDebugMsg:
        return PGE_LogLevel::Debug;
    case QtWarningMsg:
        return PGE_LogLevel::Warning;
    case QtCriticalMsg:
        return PGE_LogLevel::Critical;
    case QtFatalMsg:
        return PGE_LogLevel::Fatal;
    default:
        return PGE_LogLevel::Warning;
    }
}

static QString PgeLL2Str(PGE_LogLevel type)
{
    switch(type)
    {
    case PGE_LogLevel::Debug:
        return "Debug";
    case PGE_LogLevel::Warning:
        return "Warning";
    case PGE_LogLevel::Critical:
        return "Critical";
    case PGE_LogLevel::Fatal:
        return "Fatal";
    case PGE_LogLevel::System:
        return "System";
    case PGE_LogLevel::NoLog:
        return "NoLog";
    }
    return "Unknown";
}


void LogWriter::loadLogLevels(QComboBox *targetComboBox)
{
    targetComboBox->clear();
    targetComboBox->addItem(QObject::tr("Disable logging"));
    targetComboBox->addItem(QObject::tr("System messages"));
    targetComboBox->addItem(QObject::tr("Fatal"));
    targetComboBox->addItem(QObject::tr("Critical"));
    targetComboBox->addItem(QObject::tr("Warning"));
    targetComboBox->addItem(QObject::tr("Debug"));
}

LogWriterSignal::LogWriterSignal(QObject *parent) : QObject(parent)
{}

LogWriterSignal::LogWriterSignal(DevConsole *console, QObject *parent) : QObject(parent)
{
    setup(console);
}

void LogWriterSignal::setup(DevConsole *console)
{
    connect(this, SIGNAL(logToConsole(const QString&, const QString&)),
            console, SLOT(logMessage(const QString&, const QString&)));
}

void LogWriterSignal::log(const QString &msg, const QString &chan)
{
    emit logToConsole(msg, chan);
}



LogWriterSignal *LogWriter::consoleConnector = nullptr;

void LogWriter::LoadLogSettings()
{
    QDate d = QDate::currentDate();
    QTime t = QTime::currentTime();
    QString logFileName = QString("PGE_Editor_log_%1-%2-%3_%4-%5-%6.txt")
            .arg(d.year())
            .arg(d.month())
            .arg(d.day())
            .arg(t.hour())
            .arg(t.minute())
            .arg(t.second());
    logLevel = PGE_LogLevel::Debug;

    QString mainIniFile = AppPathManager::settingsFile();
    QSettings logSettings(mainIniFile, QSettings::IniFormat);

    QString logDirS = AppPathManager::logsDir();
    QDir defLogDir(logDirS);
    if(!defLogDir.exists())
    {
        if(!defLogDir.mkpath(logDirS))
            defLogDir.setPath(AppPathManager::userAppDir());
    }

    logSettings.beginGroup("logging");
        DebugLogFile = logSettings.value("log-path", defLogDir.absolutePath() + "/" + logFileName).toString();
        if(!QFileInfo(DebugLogFile).absoluteDir().exists())
            DebugLogFile = defLogDir.absolutePath()+"/" + logFileName;
        DebugLogFile = QFileInfo(DebugLogFile).absoluteDir().absolutePath() + "/" + logFileName;
        logLevel = PGE_LogLevel(logSettings.value("log-level", int(PGE_LogLevel::Warning)).toInt());
    logSettings.endGroup();

    qDebug() << QString("LogLevel %1, log file %2")
                .arg( PgeLL2Str(logLevel) )
                .arg( DebugLogFile );

    qInstallMessageHandler(logMessageHandler);

}

static void writeToFile(const QString &txt)
{
    QFile outFile(LogWriter::DebugLogFile);
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << endl;
    outFile.close();
}


#ifdef ONSCREEN_LOGGING
static void writeToScreen(const QString &txt)
{
    QTextStream ts(stdout);
    ts << txt << endl;
    ts.flush();
}
#endif


void LogWriter::writeLog(PGE_LogLevel type, const QString &msg)
{
    QString txt;

#ifndef ONSCREEN_LOGGING
    if(type == PGE_LogLevel::NoLog)
        return;
    if(type > logLevel)
        return;
#endif

    switch(type)
    {
        case PGE_LogLevel::Debug:
            txt = QString("Debug: %1").arg(msg);
        break;
        case PGE_LogLevel::Warning:
            txt = QString("Warning: %1").arg(msg);
        break;
        case PGE_LogLevel::Critical:
            txt = QString("Critical: %1").arg(msg);
        break;
        case PGE_LogLevel::Fatal:
            txt = QString("Fatal: %1\n\nStack trace:\n%2").arg(msg).arg(CrashHandler::getStacktrace());
        break;
        case PGE_LogLevel::System:
            txt = QString("System: %1").arg(msg);
        break;
        default:
            txt = QString("Info: %1").arg(msg);
        break;
    }

#ifdef ONSCREEN_LOGGING
    writeToScreen(txt);
    if(type == PGE_LogLevel::NoLog)
        return;
    if(type > logLevel)
        return;
#endif
    writeToFile(txt);
}

void LogWriter::logMessageHandler(QtMsgType type,
                  const QMessageLogContext& context,
                             const QString& msg)
{

    PGE_LogLevel ptype = qMsg2PgeLL(type);

#ifndef ONSCREEN_LOGGING
    if(ptype == PGE_LogLevel::NoLog)
        return;
    if(ptype > logLevel)
        return;
#endif

    QByteArray lMessage = msg.toLocal8Bit();
    QString txt;
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
        txt = QString("Fatal: (%1:%2, %3): %4\n\nStack trace:\n%5")
                .arg(context.file)
                .arg(context.line)
                .arg(context.function)
                .arg(lMessage.constData())
                .arg(CrashHandler::getStacktrace());
        break;
        default:
        txt = QString("Info: (%1:%2, %3): %4")
                .arg(context.file)
                .arg(context.line)
                .arg(context.function)
                .arg(lMessage.constData());
    }

#ifdef ONSCREEN_LOGGING
    writeToScreen(txt);
    if(ptype == PGE_LogLevel::NoLog)
        return;
    if(ptype > logLevel)
        return;
#endif
    writeToFile(txt);

    if(!LogWriter::consoleConnector)
        return;

    switch (type)
    {
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
        LogWriter::consoleConnector->log(msg, QString("Info"));
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
    consoleConnector = nullptr;
}

void LoadLogSettings()
{
    LogWriter::LoadLogSettings();
}

static QMutex logger_mutex;

void WriteToLog(PGE_LogLevel type, const QString &msg, bool noConsole)
{
    QMutexLocker muLocker(&logger_mutex);
    Q_UNUSED(muLocker)
    LogWriter::writeLog(type, msg);

    if(noConsole)
        return;

    if(!LogWriter::consoleConnector)
        return;

    switch (type)
    {
    case PGE_LogLevel::Debug:
        LogWriter::consoleConnector->log(msg, QString("Debug"));
        break;
    case PGE_LogLevel::Warning:
        LogWriter::consoleConnector->log(msg, QString("Warning"));
        break;
    case PGE_LogLevel::Critical:
        LogWriter::consoleConnector->log(msg, QString("Critical"));
        break;
    case PGE_LogLevel::Fatal:
        LogWriter::consoleConnector->log(msg, QString("Fatal"));
        break;
    case PGE_LogLevel::System:
        LogWriter::consoleConnector->log(msg, QString("System"));
        break;
    default:
        LogWriter::consoleConnector->log(msg, QString("Info"));
        break;
    }
}
