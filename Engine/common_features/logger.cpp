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
PGE_LogLevel   LogWriter::logLevel;
bool        LogWriter::enabled;

bool LogWriter::_file_is_opened = false;
std::shared_ptr<QFile>          LogWriter::_out_file;
std::shared_ptr<QTextStream>    LogWriter::_out_stream;

void LogWriter::LoadLogSettings()
{
    QString logFileName = QString("PGE_Engine_log_%1-%2-%3_%4-%5-%6.txt")
            .arg(QDate().currentDate().year())
            .arg(QDate().currentDate().month())
            .arg(QDate().currentDate().day())
            .arg(QTime().currentTime().hour())
            .arg(QTime().currentTime().minute())
            .arg(QTime().currentTime().second());
    logLevel = PGE_LogLevel::Debug;

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

        logLevel = (PGE_LogLevel)logSettings.value("log-level", (int)PGE_LogLevel::Debug).toInt();
        enabled = ((int)logLevel!=0);
    logSettings.endGroup();
    qDebug()<< QString("LogLevel %1, log file %2").arg((int)logLevel).arg(DebugLogFile);

    _out_file = std::make_shared<QFile>(DebugLogFile);
    if(_out_file.get()->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        _out_stream = std::make_shared<QTextStream>(_out_file.get());
        _out_stream.get()->setCodec("UTF-8");
        qInstallMessageHandler(logMessageHandler);
        _file_is_opened=true;
    } else {
        qWarning() << "Impossible to open " << DebugLogFile << " for write, all logs are will be printed through QtDebug...";
    }
}

void LogWriter::WriteToLog(PGE_LogLevel type, QString msg)
{
    if(!enabled) return; //if logging disabled
    if(!_file_is_opened)
    {
        switch (type)
        {
            case PGE_LogLevel::Debug:
                qDebug() << msg; break;
            case PGE_LogLevel::Warning:
                qWarning() << msg; break;
            case PGE_LogLevel::Critical:
                qCritical() << msg; break;
            case PGE_LogLevel::Fatal:
                qCritical() << msg; break;
            default: break;
        }
        return;
    }

    QString txt;

    switch (type)
    {
        case PGE_LogLevel::Debug:
            if(logLevel==PGE_LogLevel::Fatal) return;
        case PGE_LogLevel::Warning:
            if(logLevel==PGE_LogLevel::Critical) return;
        case PGE_LogLevel::Critical:
            if(logLevel==PGE_LogLevel::Warning) return;
        case PGE_LogLevel::Fatal:
            break;
        case PGE_LogLevel::NoLog: return;
    }

    switch (type)
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
            txt = QString("Fatal: %1").arg(msg);
        break;
        case PGE_LogLevel::NoLog: return;
    }

    *_out_stream.get() << txt << "\n";
    _out_stream.get()->flush();

}

void LogWriter::logMessageHandler(QtMsgType type,
                  const QMessageLogContext& context,
                             const QString& msg)
{
    if(!_file_is_opened)
    {
        switch (type)
        {
            case QtDebugMsg:
                qDebug() << msg; break;
            case QtWarningMsg:
                qWarning() << msg; break;
            case QtCriticalMsg:
                qCritical() << msg; break;
            case QtFatalMsg:
                qCritical() << msg; break;
            default: break;
        }
        return;
    }

    switch (type)
    {
        case QtDebugMsg:
            if(logLevel==PGE_LogLevel::Warning) return;
        case QtWarningMsg:
            if(logLevel==PGE_LogLevel::Critical) return;
        case QtCriticalMsg:
            if(logLevel==PGE_LogLevel::Fatal) return;
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

    *_out_stream.get() << txt << "\n";
    _out_stream.get()->flush();
//    QFile outFile(DebugLogFile);
//    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
//    QTextStream ts(&outFile);
//    ts << txt << endl;
//    outFile.close();
}


void LoadLogSettings()
{
    LogWriter::LoadLogSettings();
}

void WriteToLog(PGE_LogLevel type, QString msg)
{
    LogWriter::WriteToLog(type, msg);
}

void CloseLog()
{
    LogWriter::_out_file.get()->close();
    LogWriter::_out_stream.reset();
    LogWriter::_out_file.reset();
    LogWriter::_file_is_opened=false;
}
