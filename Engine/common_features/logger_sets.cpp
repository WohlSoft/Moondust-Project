/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#include <IniProcessor/ini_processing.h>
#include <common_features/fmt_format_ne.h>
#include <fmt/fmt_printf.h>
#include <DirManager/dirman.h>
#include <Utils/files.h>
#include <stdarg.h>
#include <cstdio>
#include <mutex>
#include <chrono>  // chrono::system_clock
#include <ctime>   // localtime
#include <sstream>

#include "app_path.h"
#include "logger_sets.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/trace.h>
#define LOG_CHANNEL "Application"
#endif

#ifdef __ANDROID__
#include <android/log.h>
#endif

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

#ifndef __EMSCRIPTEN__
static std::string return_current_time_and_date()
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    char out[24];
    memset(out, 0, sizeof(out));
    if(0 < strftime(out, sizeof(out), "%Y_%m_%d_%H_%M_%S", std::localtime(&in_time_t)))
        return std::string(out);
    else
        return "0000_00_00_00_00_00";
}
#endif//__EMSCRIPTEN__

void LogWriter::LoadLogSettings()
{
    MutexLocker mutex(&g_lockLocker);
    (void)(mutex);
#ifdef __EMSCRIPTEN__
    m_logLevel = PGE_LogLevel::Debug;
    m_logIsOpened = false;
    m_enabled = true;
    std::fprintf(stdout, "Emscripten logs stdout.\n");
#else
    std::string logFileName = fmt::format_ne("PGE_Engine_log_{0}.txt", return_current_time_and_date());

    m_logLevel = PGE_LogLevel::Debug;
    std::string mainIniFile = AppPathManager::settingsFileSTD();
    IniProcessing logSettings(mainIniFile);

    std::string logPath = AppPathManager::userAppDirSTD() + "logs";
    DirMan defLogDir(logPath);

    if(!defLogDir.exists())
    {
        if(!defLogDir.mkpath())
            defLogDir.setPath(AppPathManager::userAppDirSTD());
    }

    logSettings.beginGroup("logging");
    {
        logSettings.read("log-path", m_logFilePath, defLogDir.absolutePath() + "/" + logFileName);
        if(!DirMan::exists(Files::dirname(m_logFilePath)))
            m_logFilePath = defLogDir.absolutePath() + "/" + logFileName;
        m_logLevel  = static_cast<PGE_LogLevel>(logSettings.value("log-level", static_cast<int>(PGE_LogLevel::Debug)).toInt());
        m_enabled   = (m_logLevel != PGE_LogLevel::NoLog);
    }
    logSettings.endGroup();

    try
    {
        fmt::print("LogLevel {0}, log file {1}\n\n", static_cast<int>(m_logLevel), m_logFilePath);
    }
    catch(const fmt::FormatError &err)
    {
        std::fprintf(stderr, "fmt::print failed with exception: %s\n", err.what());
        abort();
    }

    if(m_enabled)
    {
        m_logout = SDL_RWFromFile(m_logFilePath.c_str(), "a");
        if(m_logout)
        {
            m_logIsOpened = true;
        }
        else
        {
            std::fprintf(stderr, "Impossible to open %s for write, log printing is disabled!\n", m_logFilePath.c_str());
            std::fflush(stderr);
        }
    }
#endif
}

void LoadLogSettings()
{
    LogWriter::LoadLogSettings();
}

void CloseLog()
{
#ifndef __EMSCRIPTEN__
    MutexLocker mutex(&g_lockLocker);
    (void)(mutex);
    if(LogWriter::m_logout)
        SDL_RWclose(LogWriter::m_logout);
    LogWriter::m_logout = nullptr;
    //LogWriter::m_out_stream.reset();
    LogWriter::m_logIsOpened = false;
#endif
}

#ifdef _WIN32
#define OS_NEWLINE "\r\n"
#define OS_NEWLINE_LEN 2
#else
#define OS_NEWLINE "\n"
#define OS_NEWLINE_LEN 1
#endif

void pLogDebug(const char *format, ...)
{
    va_list arg;

#if defined(__ANDROID__)
    va_start(arg, format);
    __android_log_vprint(ANDROID_LOG_DEBUG, "TRACKERS", format, arg);
    va_end(arg);
#endif

#ifndef __EMSCRIPTEN__
    if(LogWriter::m_logout == nullptr)
        return;
#endif
    if(LogWriter::m_logLevel < PGE_LogLevel::Debug)
        return;

    MutexLocker mutex(&g_lockLocker);
    ((void)mutex);
    va_start(arg, format);
#ifdef __EMSCRIPTEN__
    int len = std::vsnprintf(g_outputBuffer, OUT_BUFFER_SIZE, format, arg);
    std::fprintf(stdout, "Debug: %s\n", g_outputBuffer);
    std::fflush(stdout);
#else
    SDL_RWwrite(LogWriter::m_logout, reinterpret_cast<const void *>("Debug: "), 1, 7);
    int len = std::vsnprintf(g_outputBuffer, OUT_BUFFER_SIZE, format, arg);
    SDL_RWwrite(LogWriter::m_logout, g_outputBuffer, 1, (size_t)len);
    SDL_RWwrite(LogWriter::m_logout, reinterpret_cast<const void *>(OS_NEWLINE), 1, OS_NEWLINE_LEN);
#endif
    va_end(arg);
}

void pLogWarning(const char *format, ...)
{
    va_list arg;

#if defined(__ANDROID__)
    va_start(arg, format);
    __android_log_vprint(ANDROID_LOG_WARN, "TRACKERS", format, arg);
    va_end(arg);
#endif

#ifndef __EMSCRIPTEN__
    if(LogWriter::m_logout == nullptr)
        return;
#endif
    if(LogWriter::m_logLevel < PGE_LogLevel::Warning)
        return;

    MutexLocker mutex(&g_lockLocker);
    ((void)mutex);
    va_start(arg, format);
#ifdef __EMSCRIPTEN__
    int len = std::vsnprintf(g_outputBuffer, OUT_BUFFER_SIZE, format, arg);
    std::fprintf(stdout, "Warning: %s\n", g_outputBuffer);
    std::fflush(stdout);
#else
    SDL_RWwrite(LogWriter::m_logout, reinterpret_cast<const void *>("Warning: "), 1, 9);
    int len = std::vsnprintf(g_outputBuffer, OUT_BUFFER_SIZE, format, arg);
    SDL_RWwrite(LogWriter::m_logout, g_outputBuffer, 1, (size_t)len);
    SDL_RWwrite(LogWriter::m_logout, reinterpret_cast<const void *>(OS_NEWLINE), 1, OS_NEWLINE_LEN);
#endif
    va_end(arg);
}

void pLogCritical(const char *format, ...)
{
    va_list arg;

#if defined(__ANDROID__)
    va_start(arg, format);
    __android_log_vprint(ANDROID_LOG_ERROR, "TRACKERS", format, arg);
    va_end(arg);
#endif

#ifndef __EMSCRIPTEN__
    if(LogWriter::m_logout == nullptr)
        return;
#endif
    if(LogWriter::m_logLevel < PGE_LogLevel::Critical)
        return;

    MutexLocker mutex(&g_lockLocker);
    ((void)mutex);
    va_start(arg, format);
#ifdef __EMSCRIPTEN__
    int len = std::vsnprintf(g_outputBuffer, OUT_BUFFER_SIZE, format, arg);
    std::fprintf(stdout, "Critical: %s\n", g_outputBuffer);
    std::fflush(stdout);
#else
    SDL_RWwrite(LogWriter::m_logout, reinterpret_cast<const void *>("Critical: "), 1, 10);
    int len = std::vsnprintf(g_outputBuffer, OUT_BUFFER_SIZE, format, arg);
    SDL_RWwrite(LogWriter::m_logout, g_outputBuffer, 1, (size_t)len);
    SDL_RWwrite(LogWriter::m_logout, reinterpret_cast<const void *>(OS_NEWLINE), 1, OS_NEWLINE_LEN);
#endif
    va_end(arg);
}

void pLogFatal(const char *format, ...)
{
    va_list arg;

#if defined(__ANDROID__)
    va_start(arg, format);
    __android_log_vprint(ANDROID_LOG_FATAL, "TRACKERS", format, arg);
    va_end(arg);
#endif

#ifndef __EMSCRIPTEN__
    if(LogWriter::m_logout == nullptr)
        return;
#endif
    if(LogWriter::m_logLevel < PGE_LogLevel::Fatal)
        return;

    MutexLocker mutex(&g_lockLocker);
    ((void)mutex);
    va_start(arg, format);
#ifdef __EMSCRIPTEN__
    int len = std::vsnprintf(g_outputBuffer, OUT_BUFFER_SIZE, format, arg);
    std::fprintf(stdout, "Fatal: %s\n", g_outputBuffer);
    std::fflush(stdout);
#else
    SDL_RWwrite(LogWriter::m_logout, reinterpret_cast<const void *>("Fatal: "), 1, 7);
    int len = std::vsnprintf(g_outputBuffer, OUT_BUFFER_SIZE, format, arg);
    SDL_RWwrite(LogWriter::m_logout, g_outputBuffer, 1, (size_t)len);
    SDL_RWwrite(LogWriter::m_logout, reinterpret_cast<const void *>(OS_NEWLINE), 1, OS_NEWLINE_LEN);
#endif
    va_end(arg);
}

void pLogInfo(const char *format, ...)
{
    va_list arg;

#if defined(__ANDROID__)
    va_start(arg, format);
    __android_log_vprint(ANDROID_LOG_INFO, "TRACKERS", format, arg);
    va_end(arg);
#endif

#ifndef __EMSCRIPTEN__
    if(LogWriter::m_logout == nullptr)
        return;
#endif
    if(LogWriter::m_logLevel < PGE_LogLevel::Fatal)
        return;

    MutexLocker mutex(&g_lockLocker);
    ((void)mutex);
    va_start(arg, format);
#ifdef __EMSCRIPTEN__
    int len = std::vsnprintf(g_outputBuffer, OUT_BUFFER_SIZE, format, arg);
    std::fprintf(stdout, "Info: %s\n", g_outputBuffer);
    std::fflush(stdout);
#else
    SDL_RWwrite(LogWriter::m_logout, reinterpret_cast<const void *>("Info: "), 1, 6);
    int len = std::vsnprintf(g_outputBuffer, OUT_BUFFER_SIZE, format, arg);
    SDL_RWwrite(LogWriter::m_logout, g_outputBuffer, 1, (size_t)len);
    SDL_RWwrite(LogWriter::m_logout, reinterpret_cast<const void *>(OS_NEWLINE), 1, OS_NEWLINE_LEN);
#endif
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
            std::fprintf(stderr, "DEBUG: %s\n", msg.c_str());
            break;
        case PGE_LogLevel::Warning:
            std::fprintf(stderr, "WARNING: %s\n", msg.c_str());
            break;
        case PGE_LogLevel::Critical:
            std::fprintf(stderr, "CRITICAL ERROR: %s\n", msg.c_str());
            break;
        case PGE_LogLevel::Fatal:
            std::fprintf(stderr, "FATAL ERROR: %s\n", msg.c_str());
            break;
        case PGE_LogLevel::NoLog:
            break;
        }
        std::fflush(stderr);
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
