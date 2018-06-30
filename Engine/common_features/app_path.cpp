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

#include <DirManager/dirman.h>
#include <Utils/files.h>
#include <IniProcessor/ini_processing.h>
#define FMT_NOEXCEPT
#include <fmt/fmt_format.h>

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <PGE_File_Formats/pge_file_lib_globs.h>
#include "apple/app_path_macosx.h"
#endif

#ifdef __gnu_linux__
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif

#ifdef _WIN32
#include <windows.h>
#include <winreg.h>
#include <algorithm> // std::replace from \\ into /
#endif

#include "app_path.h"
#include "../version.h"

#include <SDL2/SDL.h>

std::string  ApplicationPathSTD;

std::string AppPathManager::m_settingsPath;
std::string AppPathManager::m_userPath;

#if defined(__ANDROID__) || defined(__APPLE__)
#define UserDirName "/PGE Project"
#else
#define UserDirName "/.PGE_Project"
#endif

/**
 * @brief Retreive User Home directory with appending of the PGE user data directory
 * @return Absolute directory path
 */
static std::string getPgeUserDirectory()
{
    std::string path = "";
#if defined(__APPLE__)
    {
        char *base_path = getAppSupportDir();
        if(base_path)
        {
            path.append(base_path);
            SDL_free(base_path);
        }
    }
#elif defined(_WIN32)
    {
        wchar_t pathW[MAX_PATH];
        DWORD path_len = GetEnvironmentVariableW(L"UserProfile", pathW, MAX_PATH);
        assert(path_len);
        path.resize(path_len * 2);
        path_len = WideCharToMultiByte(CP_UTF8, 0,
                                       pathW,       path_len,
                                       &path[0],    path.size(),
                                       0, FALSE);
        path.resize(path_len);
    }
#elif defined(__ANDROID__)
    path = "/sdcard/";
#elif defined(__gnu_linux__)
    {
        passwd* pw = getpwuid(getuid());
        path.append(pw->pw_dir);
    }
#endif
    return path.empty() ? std::string(".") : (path + UserDirName);
}


void AppPathManager::initAppPath()
{
    //PGE_Application::setOrganizationName(V_COMPANY);
    //PGE_Application::setOrganizationDomain(V_PGE_URL);
    //PGE_Application::setApplicationName("PGE Engine");
    /*
    ApplicationPathSTD = DirMan(Files::dirname(argv0)).absolutePath();
    ApplicationPath =   QString::fromStdString(ApplicationPathSTD);
    ApplicationPath_x = ApplicationPath;
    */
#ifdef __APPLE__
    {
        CFURLRef appUrlRef;
        appUrlRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
        CFStringRef filePathRef = CFURLGetString(appUrlRef);
        char temporaryCString[PATH_MAX];
        bzero(temporaryCString, PATH_MAX);
        CFStringGetCString(filePathRef, temporaryCString, PATH_MAX, kCFStringEncodingUTF8);
        ApplicationPathSTD = PGE_URLDEC(std::string(temporaryCString));
        {
            std::string::size_type i = ApplicationPathSTD.find_last_of(".app");
            i = ApplicationPathSTD.find_last_of('/', i);
            ApplicationPathSTD.erase(i, ApplicationPathSTD.size() - i);
            if(ApplicationPathSTD.compare(0, 7, "file://") == 0)
                ApplicationPathSTD.erase(0, 7);
            if(!ApplicationPathSTD.empty() && (ApplicationPathSTD.back() != '/'))
                ApplicationPathSTD.push_back('/');
        }
        //CFRelease(filePathRef);
        CFRelease(appUrlRef);
    }
#else //__APPLE__
    char* path = SDL_GetBasePath();//DirMan(Files::dirname(argv0)).absolutePath();
    if(!path)
    {
        std::fprintf(stderr, "== Failed to recogonize application path by using of SDL_GetBasePath! Using current working directory \"./\" instead.\n");
        std::fflush(stderr);
        path = SDL_strdup("./");
    }
    ApplicationPathSTD = std::string(path);
#   if defined(_WIN32)
    std::replace(ApplicationPathSTD.begin(), ApplicationPathSTD.end(), '\\', '/');
#   endif
    SDL_free(path);
#endif

    if(isPortable())
        return;

    std::string userDirPath = getPgeUserDirectory();
    if(!userDirPath.empty())
    {
        DirMan appDir(userDirPath);
        if(!appDir.exists() && !appDir.mkpath(userDirPath))
            goto defaultSettingsPath;
#ifdef __APPLE__
        if(!DirMan::exists(ApplicationPathSTD + "/Data directory"))
            symlink((userDirPath).c_str(), (ApplicationPathSTD + "/Data directory").c_str());
#endif
        m_userPath = appDir.absolutePath();
        m_userPath.push_back('/');
        initSettingsPath();
    }
    else
    {
        goto defaultSettingsPath;
    }

    return;
defaultSettingsPath:
    m_userPath = ApplicationPathSTD;
    initSettingsPath();
#ifdef __EMSCRIPTEN__
    printf("== App Path is %s\n", ApplicationPathSTD.c_str());
    printf("== User Path is %s\n", m_userPath.c_str());
    fflush(stdout);
#endif
}

std::string AppPathManager::settingsFileSTD()
{
    return m_settingsPath + "pge_engine.ini";
}

std::string AppPathManager::userAppDirSTD()
{
    return m_userPath;
}

std::string AppPathManager::languagesDir()
{
#ifndef __APPLE__
    return ApplicationPathSTD + "languages";
#else
    CFURLRef appUrlRef;
    appUrlRef = CFBundleCopyResourceURL(CFBundleGetMainBundle(), CFSTR("languages"), NULL, NULL);
    CFStringRef filePathRef = CFURLGetString(appUrlRef);
    char temporaryCString[PATH_MAX];
    bzero(temporaryCString, PATH_MAX);
    CFStringGetCString(filePathRef, temporaryCString, PATH_MAX, kCFStringEncodingUTF8);
    std::string path = PGE_URLDEC(std::string(temporaryCString));
    if(path.compare(0, 7, "file://") == 0)
        path.erase(0, 7);
    return path;
#endif
}

std::string AppPathManager::screenshotsDir()
{
#ifndef __APPLE__
    return m_userPath + "/screenshots";
#else
    std::string path = m_userPath;
    char *base_path = getScreenCaptureDir();
    if(base_path)
    {
        path = base_path;
        SDL_free(base_path);
    }
    return path + "/Moondust Game Screenshots";
#endif
}

void AppPathManager::install()
{
    std::string path = getPgeUserDirectory();

    if(!path.empty())
    {
        DirMan appDir(path);
        if(!appDir.exists())
            appDir.mkpath(path);
    }
}

bool AppPathManager::isPortable()
{
    if(m_settingsPath.empty())
        m_settingsPath = ApplicationPathSTD;

    if(m_userPath.empty())
        m_userPath = ApplicationPathSTD;

    if(!Files::fileExists(settingsFileSTD()))
        return false;

    bool forcePortable = false;

    IniProcessing checkForPort(settingsFileSTD());
    checkForPort.beginGroup("Main");
    forcePortable = checkForPort.value("force-portable", false).toBool();
    checkForPort.endGroup();

    if(forcePortable)
        initSettingsPath();

    return forcePortable;
}

bool AppPathManager::userDirIsAvailable()
{
    return (m_userPath.compare(ApplicationPathSTD) != 0);
}


void AppPathManager::initSettingsPath()
{
    m_settingsPath = m_userPath + "settings/";

    if(Files::fileExists(m_settingsPath))
        Files::deleteFile(m_settingsPath);//Just in case, avoid mad jokes with making same-named file as settings folder

    if(!DirMan::exists(m_settingsPath))
        DirMan::mkAbsPath(m_settingsPath);
}
