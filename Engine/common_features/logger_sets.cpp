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

#include <stdarg.h>
#include <mutex>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QSettings>
#include <QDateTime>
#include <QtDebug>

#include "app_path.h"
#include "logger_sets.h"

static std::mutex g_lockLocker;

class MutexLocker
{
        std::mutex *m_mutex;
    public:
        MutexLocker(std::mutex *mutex)
        {
            m_mutex = mutex;
            m_mutex->lock();
        }
        ~MutexLocker()
        {
            m_mutex->unlock();
        }
};

QString         LogWriter::m_logFilePath;
PGE_LogLevel    LogWriter::m_logLevel;
bool            LogWriter::m_enabled;

bool  LogWriter::m_logIsOpened = false;
FILE *LogWriter::m_logout = nullptr;

std::shared_ptr<QTextStream>    LogWriter::m_out_stream;

void LogWriter::LoadLogSettings()
{
    MutexLocker mutex(&g_lockLocker);
    Q_UNUSED(mutex);
    QString logFileName = QString("PGE_Engine_log_%1-%2-%3_%4-%5-%6.txt")
                          .arg(QDate().currentDate().year())
                          .arg(QDate().currentDate().month())
                          .arg(QDate().currentDate().day())
                          .arg(QTime().currentTime().hour())
                          .arg(QTime().currentTime().minute())
                          .arg(QTime().currentTime().second());
    m_logLevel = PGE_LogLevel::Debug;
    QString mainIniFile = AppPathManager::settingsFile();
    QSettings logSettings(mainIniFile, QSettings::IniFormat);
    QDir defLogDir(AppPathManager::userAppDir() + "/logs");

    if(!defLogDir.exists())
        if(!defLogDir.mkpath(AppPathManager::userAppDir() + "/logs"))
            defLogDir.setPath(AppPathManager::userAppDir());

    logSettings.beginGroup("logging");
    m_logFilePath = logSettings.value("log-path", defLogDir.absolutePath() + "/" + logFileName).toString();

    if(!QFileInfo(m_logFilePath).absoluteDir().exists())
        m_logFilePath = defLogDir.absolutePath() + "/" + logFileName;

    m_logLevel  = static_cast<PGE_LogLevel>(logSettings.value("log-level", static_cast<int>(PGE_LogLevel::Debug)).toInt());
    m_enabled   = (m_logLevel != PGE_LogLevel::NoLog);
    logSettings.endGroup();
    qDebug() << QString("LogLevel %1, log file %2").arg(static_cast<int>(m_logLevel)).arg(m_logFilePath);

    if(m_enabled)
    {
#ifdef _WIN32
        m_logout = _wfopen(m_logFilePath.toStdWString().data(), L"a");
#else
        m_logout = fopen(m_logFilePath.toLocal8Bit().data(), "a");
#endif

        if(m_logout)
        {
            m_out_stream = std::make_shared<QTextStream>(m_logout);
            m_out_stream.get()->setCodec("UTF-8");
            qInstallMessageHandler(logMessageHandler);
            m_logIsOpened = true;
        }
        else
            qWarning() << "Impossible to open " << m_logFilePath << " for write, all logs are will be printed through QtDebug...";
    }
}

void LoadLogSettings()
{
    LogWriter::LoadLogSettings();
}

void CloseLog()
{
    MutexLocker mutex(&g_lockLocker);
    Q_UNUSED(mutex);
    fclose(LogWriter::m_logout);
    LogWriter::m_logout = nullptr;
    LogWriter::m_out_stream.reset();
    LogWriter::m_logIsOpened = false;
}

void pLogDebug(const char *format, ...)
{
    va_list arg;

    if(LogWriter::m_logout == nullptr)
        return;

    if(LogWriter::m_logLevel < PGE_LogLevel::Debug)
        return;

    MutexLocker mutex(&g_lockLocker);
    Q_UNUSED(mutex);
    va_start(arg, format);
    fwrite(reinterpret_cast<const void *>("Debug: "), 1, 7, LogWriter::m_logout);
    vfprintf(LogWriter::m_logout, format, arg);
    fwrite(reinterpret_cast<const void *>("\n"), 1, 1, LogWriter::m_logout);
    va_end(arg);
    fflush(LogWriter::m_logout);
}

void pLogWarning(const char *format, ...)
{
    va_list arg;

    if(LogWriter::m_logout == nullptr)
        return;

    if(LogWriter::m_logLevel < PGE_LogLevel::Warning)
        return;

    MutexLocker mutex(&g_lockLocker);
    Q_UNUSED(mutex);
    va_start(arg, format);
    fwrite(reinterpret_cast<const void *>("Warning: "), 1, 9, LogWriter::m_logout);
    vfprintf(LogWriter::m_logout, format, arg);
    fwrite(reinterpret_cast<const void *>("\n"), 1, 1, LogWriter::m_logout);
    va_end(arg);
    fflush(LogWriter::m_logout);
}

void pLogCritical(const char *format, ...)
{
    va_list arg;

    if(LogWriter::m_logout == nullptr)
        return;

    if(LogWriter::m_logLevel < PGE_LogLevel::Critical)
        return;

    MutexLocker mutex(&g_lockLocker);
    Q_UNUSED(mutex);
    va_start(arg, format);
    fwrite(reinterpret_cast<const void *>("Critical: "), 1, 10, LogWriter::m_logout);
    vfprintf(LogWriter::m_logout, format, arg);
    fwrite(reinterpret_cast<const void *>("\n"), 1, 1, LogWriter::m_logout);
    va_end(arg);
    fflush(LogWriter::m_logout);
}

void pLogFatal(const char *format, ...)
{
    va_list arg;

    if(LogWriter::m_logout == nullptr)
        return;

    if(LogWriter::m_logLevel < PGE_LogLevel::Fatal)
        return;

    MutexLocker mutex(&g_lockLocker);
    Q_UNUSED(mutex);
    va_start(arg, format);
    fwrite(reinterpret_cast<const void *>("Fatal: "), 1, 7, LogWriter::m_logout);
    vfprintf(LogWriter::m_logout, format, arg);
    fwrite(reinterpret_cast<const void *>("\n"), 1, 1, LogWriter::m_logout);
    va_end(arg);
    fflush(LogWriter::m_logout);
}

void pLogInfo(const char *format, ...)
{
    va_list arg;

    if(LogWriter::m_logout == nullptr)
        return;

    if(LogWriter::m_logLevel < PGE_LogLevel::Fatal)
        return;

    MutexLocker mutex(&g_lockLocker);
    Q_UNUSED(mutex);
    va_start(arg, format);
    fwrite(reinterpret_cast<const void *>("Info: "), 1, 6, LogWriter::m_logout);
    vfprintf(LogWriter::m_logout, format, arg);
    fwrite(reinterpret_cast<const void *>("\n"), 1, 1, LogWriter::m_logout);
    va_end(arg);
    fflush(LogWriter::m_logout);
}

void WriteToLog(PGE_LogLevel type, QString msg)
{
    LogWriter::WriteToLog(type, msg);
}


void LogWriter::WriteToLog(PGE_LogLevel type, QString msg)
{
    MutexLocker mutex(&g_lockLocker);
    Q_UNUSED(mutex);

    if(!m_enabled) return; //if logging disabled

    if(!m_logIsOpened)
    {
        switch(type)
        {
        case PGE_LogLevel::Debug:
            qDebug() << msg;
            break;

        case PGE_LogLevel::Warning:
            qWarning() << msg;
            break;

        case PGE_LogLevel::Critical:
            qCritical() << msg;
            break;

        case PGE_LogLevel::Fatal:
            qCritical() << msg;
            break;

        case PGE_LogLevel::NoLog:
            break;
        }

        return;
    }

    QString txt;

    switch(type)
    {
    case PGE_LogLevel::Debug:
        if(m_logLevel < PGE_LogLevel::Debug)
            return;

        break;

    case PGE_LogLevel::Warning:
        if(m_logLevel < PGE_LogLevel::Warning)
            return;

        break;

    case PGE_LogLevel::Critical:
        if(m_logLevel < PGE_LogLevel::Critical)
            return;

        break;

    case PGE_LogLevel::Fatal:
        if(m_logLevel < PGE_LogLevel::Fatal)
            return;

        break;

    case PGE_LogLevel::NoLog:
        return;
    }

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
        txt = QString("Fatal: %1").arg(msg);
        break;

    case PGE_LogLevel::NoLog:
        return;
    }

    *m_out_stream.get() << txt << "\n";
    m_out_stream.get()->flush();
}

void LogWriter::logMessageHandler(QtMsgType type,
                                  const QMessageLogContext &context,
                                  const QString &msg)
{
    MutexLocker mutex(&g_lockLocker);
    Q_UNUSED(mutex);

    if(!m_logIsOpened)
    {
        switch(type)
        {
        case QtDebugMsg:
            qDebug() << msg;
            break;

        case QtWarningMsg:
            qWarning() << msg;
            break;

        case QtCriticalMsg:
            qCritical() << msg;
            break;

        case QtFatalMsg:
            qCritical() << "<FATAL ERROR> " << msg;
            break;
#if QT_VERSION >= 050500

        case QtInfoMsg:
            qDebug() << "<INFO> " << msg;
            break;
#endif
        }

        return;
    }

    switch(type)
    {
    case QtDebugMsg:
        if(m_logLevel < PGE_LogLevel::Debug)
            return;

        break;

    case QtWarningMsg:
        if(m_logLevel < PGE_LogLevel::Warning)
            return;

        break;

    case QtCriticalMsg:
        if(m_logLevel < PGE_LogLevel::Critical)
            return;

        break;

    case QtFatalMsg:
        if(m_logLevel < PGE_LogLevel::Fatal)
            return;

        break;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))

    case QtInfoMsg:
        break;
#endif
    }

    QByteArray lMessage = msg.toLocal8Bit();
    QString txt;

    switch(type)
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
        break;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))

    case QtInfoMsg:
        txt = QString("Info: (%1:%2, %3): %4")
              .arg(context.file)
              .arg(context.line)
              .arg(context.function)
              .arg(lMessage.constData());
        break;
#endif
    }

    *m_out_stream.get() << txt << "\n";
    m_out_stream.get()->flush();

    if(type == QtFatalMsg)
        abort();

    //    QFile outFile(DebugLogFile);
    //    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    //    QTextStream ts(&outFile);
    //    ts << txt << endl;
    //    outFile.close();
}
