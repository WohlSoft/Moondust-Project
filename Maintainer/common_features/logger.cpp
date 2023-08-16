/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2023 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <QDateTime>
#include <QtDebug>

#include "app_path.h"
#include "logger_sets.h"

QString     LogWriter::DebugLogFile;
QtMsgType   LogWriter::logLevel;
bool        LogWriter::enabled;

void LogWriter::LoadLogSettings()
{
    QString logFileName = QString("PGE_Maintainer_log_%1-%2-%3_%4-%5-%6.txt")
            .arg(QDate().currentDate().year())
            .arg(QDate().currentDate().month())
            .arg(QDate().currentDate().day())
            .arg(QTime().currentTime().hour())
            .arg(QTime().currentTime().minute())
            .arg(QTime().currentTime().second());
    logLevel = QtDebugMsg;

    QString mainIniFile = AppPathManager::settingsFile();
    QSettings logSettings(mainIniFile, QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    logSettings.setIniCodec("UTF-8");
#endif

    QDir defLogDir(AppPathManager::userAppDir()+"/logs");
    if(!defLogDir.exists())
        if(!defLogDir.mkpath(AppPathManager::userAppDir()+"/logs"))
            defLogDir.setPath(AppPathManager::userAppDir());

    logSettings.beginGroup("logging");
        DebugLogFile = logSettings.value("log-path", defLogDir.absolutePath()+"/"+logFileName).toString();
        if(!QFileInfo(DebugLogFile).absoluteDir().exists())
            DebugLogFile = defLogDir.absolutePath()+"/"+logFileName;

        enabled = true;
        switch( logSettings.value("log-level", "4").toInt() )
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
    #ifndef DEBUG_BUILD
    qInstallMessageHandler(logMessageHandler);
    #endif
}

void LogWriter::WriteToLog(QtMsgType type, QString msg)
{
    if(!enabled) return; //if logging disabled

    QString txt;

    switch (type)
    {
    case QtDebugMsg:
        if(logLevel==QtFatalMsg) return;/*fallthrough*/
    case QtWarningMsg:
        if(logLevel==QtCriticalMsg) return;/*fallthrough*/
    case QtCriticalMsg:
        if(logLevel==QtWarningMsg) return;/*fallthrough*/
    case QtFatalMsg:
        break;
    default: break;
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
    break;
    default:
        txt = QString("Info: %1").arg(msg);
    }

    QFile outFile(DebugLogFile);
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    ts << txt << Qt::endl;
#else
    ts << txt << endl;
#endif
    outFile.close();
}

void LogWriter::logMessageHandler(QtMsgType type,
                  const QMessageLogContext& context,
                             const QString& msg)
{
    switch (type)
    {
        case QtDebugMsg:
            if(logLevel==QtWarningMsg) return;/*fallthrough*/
        case QtWarningMsg:
            if(logLevel==QtCriticalMsg) return;/*fallthrough*/
        case QtCriticalMsg:
            if(logLevel==QtFatalMsg) return;/*fallthrough*/
        case QtFatalMsg:
            break;
        default: break;
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
        abort();
        break;
        default:
            txt = QString("Info: (%1:%2, %3): %4")
                    .arg(context.file)
                    .arg(context.line)
                    .arg(context.function)
                    .arg(lMessage.constData());
    }

    QFile outFile(DebugLogFile);
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    ts << txt << Qt::endl;
#else
    ts << txt << endl;
#endif
    outFile.close();
}


void LoadLogSettings()
{
    LogWriter::LoadLogSettings();
}

void WriteToLog(QtMsgType type, QString msg)
{
    LogWriter::WriteToLog(type, msg);
}
