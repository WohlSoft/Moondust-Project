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
#include <chrono>

#include <IniProcessor/ini_processing.h>
#include <fmt/fmt_format.h>
#include <fmt/fmt_printf.h>
#include <DirManager/dirman.h>
#include <Utils/files.h>
#include <cstdio>

#include "app_path.h"
#include "logger_sets.h"

static std::mutex g_lockLocker;
#define OUT_BUFFER_SIZE 10240
static char       g_outputBuffer[OUT_BUFFER_SIZE];

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

std::string     LogWriter::m_logFilePath;
PGE_LogLevel    LogWriter::m_logLevel;
bool            LogWriter::m_enabled;

bool  LogWriter::m_logIsOpened = false;
SDL_RWops *LogWriter::m_logout = nullptr;

void LogWriter::LoadLogSettings()
{
    MutexLocker mutex(&g_lockLocker);
    (void)(mutex);

    auto now = std::chrono::system_clock::now();
    std::time_t in_time_t = std::chrono::system_clock::to_time_t(now);
    tm *t = std::localtime(&in_time_t);
    std::string logFileName = fmt::format("PGE_Engine_log_{0}_{1}_{2}_{3}_{4}_{5}.txt",
                                          t->tm_year, t->tm_mon, t->tm_mday,
                                          t->tm_hour, t->tm_min, t->tm_sec);

    m_logLevel = PGE_LogLevel::Debug;
    std::string mainIniFile = AppPathManager::settingsFileSTD();
    IniProcessing logSettings(mainIniFile);
    DirMan defLogDir(AppPathManager::userAppDirSTD() + "/logs");

    if(!defLogDir.exists())
        if(!defLogDir.mkpath(AppPathManager::userAppDirSTD() + "/logs"))
            defLogDir.setPath(AppPathManager::userAppDirSTD());

    logSettings.beginGroup("logging");
    m_logFilePath = logSettings.value("log-path", defLogDir.absolutePath() + "/" + logFileName).toString();

    if(!DirMan::exists(Files::dirname(m_logFilePath)))
        m_logFilePath = defLogDir.absolutePath() + "/" + logFileName;

    m_logLevel  = static_cast<PGE_LogLevel>(logSettings.value("log-level", static_cast<int>(PGE_LogLevel::Debug)).toInt());
    m_enabled   = (m_logLevel != PGE_LogLevel::NoLog);
    logSettings.endGroup();

    fmt::print("LogLevel {0}, log file {1}\n\n", static_cast<int>(m_logLevel), m_logFilePath);

    if(m_enabled)
    {
        m_logout = SDL_RWFromFile(m_logFilePath.c_str(), "a");
        if(m_logout)
        {
            m_logIsOpened = true;
        }
        else
        {
            fmt::fprintf(stderr, "Impossible to open %s for write, all logs are will be printed through QtDebug...\n", m_logFilePath);
            fflush(stderr);
        }
    }
}

void LoadLogSettings()
{
    LogWriter::LoadLogSettings();
}

void CloseLog()
{
    MutexLocker mutex(&g_lockLocker);
    (void)(mutex);
    SDL_RWclose(LogWriter::m_logout);
    LogWriter::m_logout = nullptr;
    //LogWriter::m_out_stream.reset();
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
    ((void)mutex);
    va_start(arg, format);
    SDL_RWwrite(LogWriter::m_logout, reinterpret_cast<const void *>("Debug: "), 1, 7);
    int len = std::vsnprintf(g_outputBuffer, OUT_BUFFER_SIZE, format, arg);
    SDL_RWwrite(LogWriter::m_logout, g_outputBuffer, 1, len);
    SDL_RWwrite(LogWriter::m_logout, reinterpret_cast<const void *>("\n"), 1, 1);
    va_end(arg);
}

void pLogWarning(const char *format, ...)
{
    va_list arg;
    if(LogWriter::m_logout == nullptr)
        return;
    if(LogWriter::m_logLevel < PGE_LogLevel::Warning)
        return;

    MutexLocker mutex(&g_lockLocker);
    ((void)mutex);
    va_start(arg, format);
    SDL_RWwrite(LogWriter::m_logout, reinterpret_cast<const void *>("Warning: "), 1, 9);
    int len = std::vsnprintf(g_outputBuffer, OUT_BUFFER_SIZE, format, arg);
    SDL_RWwrite(LogWriter::m_logout, g_outputBuffer, 1, len);
    SDL_RWwrite(LogWriter::m_logout, reinterpret_cast<const void *>("\n"), 1, 1);
    va_end(arg);
}

void pLogCritical(const char *format, ...)
{
    va_list arg;
    if(LogWriter::m_logout == nullptr)
        return;
    if(LogWriter::m_logLevel < PGE_LogLevel::Critical)
        return;

    MutexLocker mutex(&g_lockLocker);
    ((void)mutex);
    va_start(arg, format);
    SDL_RWwrite(LogWriter::m_logout, reinterpret_cast<const void *>("Critical: "), 1, 10);
    int len = std::vsnprintf(g_outputBuffer, OUT_BUFFER_SIZE, format, arg);
    SDL_RWwrite(LogWriter::m_logout, g_outputBuffer, 1, len);
    SDL_RWwrite(LogWriter::m_logout, reinterpret_cast<const void *>("\n"), 1, 1);
    va_end(arg);
}

void pLogFatal(const char *format, ...)
{
    va_list arg;
    if(LogWriter::m_logout == nullptr)
        return;
    if(LogWriter::m_logLevel < PGE_LogLevel::Fatal)
        return;

    MutexLocker mutex(&g_lockLocker);
    ((void)mutex);
    va_start(arg, format);
    SDL_RWwrite(LogWriter::m_logout, reinterpret_cast<const void *>("Fatal: "), 1, 7);
    int len = std::vsnprintf(g_outputBuffer, OUT_BUFFER_SIZE, format, arg);
    SDL_RWwrite(LogWriter::m_logout, g_outputBuffer, 1, len);
    SDL_RWwrite(LogWriter::m_logout, reinterpret_cast<const void *>("\n"), 1, 1);
    va_end(arg);
}

void pLogInfo(const char *format, ...)
{
    va_list arg;
    if(LogWriter::m_logout == nullptr)
        return;
    if(LogWriter::m_logLevel < PGE_LogLevel::Fatal)
        return;

    MutexLocker mutex(&g_lockLocker);
    ((void)mutex);
    va_start(arg, format);
    SDL_RWwrite(LogWriter::m_logout, reinterpret_cast<const void *>("Info: "), 1, 6);
    int len = std::vsnprintf(g_outputBuffer, OUT_BUFFER_SIZE, format, arg);
    SDL_RWwrite(LogWriter::m_logout, g_outputBuffer, 1, len);
    SDL_RWwrite(LogWriter::m_logout, reinterpret_cast<const void *>("\n"), 1, 1);
    va_end(arg);
}

void WriteToLog(PGE_LogLevel type, std::string msg)
{
    LogWriter::WriteToLog(type, msg);
}


void LogWriter::WriteToLog(PGE_LogLevel type, const std::string &msg)
{
    if(!m_enabled)
        return; //if logging disabled

    if(!m_logIsOpened)
    {
        switch(type)
        {
        case PGE_LogLevel::Debug:
            fprintf(stderr, "DEBUG: %s\n", msg.c_str());
            break;

        case PGE_LogLevel::Warning:
            fprintf(stderr, "WARNING: %s\n", msg.c_str());
            break;

        case PGE_LogLevel::Critical:
            fprintf(stderr, "CRITICAL ERROR: %s\n", msg.c_str());
            break;

        case PGE_LogLevel::Fatal:
            fprintf(stderr, "FATAL ERROR: %s\n", msg.c_str());
            break;

        case PGE_LogLevel::NoLog:
            break;
        }

        fflush(stderr);
        return;
    }

    switch(type)
    {
    case PGE_LogLevel::Debug:
        if(m_logLevel < PGE_LogLevel::Debug)
            return;
        pLogDebug("%s", msg.c_str());
        break;

    case PGE_LogLevel::Warning:
        if(m_logLevel < PGE_LogLevel::Warning)
            return;
        pLogWarning("%s", msg.c_str());
        break;

    case PGE_LogLevel::Critical:
        if(m_logLevel < PGE_LogLevel::Critical)
            return;
        pLogCritical("%s", msg.c_str());
        break;

    case PGE_LogLevel::Fatal:
        if(m_logLevel < PGE_LogLevel::Fatal)
            return;
        pLogFatal("%s", msg.c_str());
        break;

    case PGE_LogLevel::NoLog:
        return;
    }
}
