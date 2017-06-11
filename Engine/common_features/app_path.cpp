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
        DWORD path_len = GetEnvironmentVariable(L"UserProfile", pathW, MAX_PATH);
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
    return path.empty() ? std::string() : (path + UserDirName);
}


#ifdef _WIN32

#define PGE_ENGINE_KEY L"Software\\Wohlhabend Networks\\PGE Engine"

/**
 * @brief Retreive the state from Windows System Registry is PGE Engine set to use User Directory instead of application directory
 * @return true if registry key exist and has "true" string value
 */
static bool winReg_isUserDir()
{
    HKEY    pge_key;
    wchar_t enable_user_dir[1024];
    std::wstring output;
    DWORD   DRlen = sizeof(wchar_t) * 1024;

    LONG res = RegOpenKeyExW(HKEY_CURRENT_USER, PGE_ENGINE_KEY, 0, KEY_READ, &pge_key);
    if(res == ERROR_SUCCESS)
    {
        if(RegQueryValueExW(pge_key, L"EnableUserDir", 0, NULL, (LPBYTE)enable_user_dir, &DRlen) == ERROR_SUCCESS)
            output.append(enable_user_dir);
        RegCloseKey(pge_key);
    }
    return (output == L"true");
}

/**
 * @brief Creates the key and sets the "true" value in the Windows System Registry
 * @return true if operation was success
 */
static bool winReg_setUserDir()
{
    HKEY    pge_key;
    bool ret = false;
    DWORD   lpdwDisp;
    LONG res = RegCreateKeyExW(HKEY_CURRENT_USER, PGE_ENGINE_KEY, 0,
                               NULL, REG_OPTION_NON_VOLATILE,
                               KEY_WRITE, NULL, &pge_key, &lpdwDisp);
    if(res == ERROR_SUCCESS)
    {
        if(RegSetValueExW(pge_key, L"EnableUserDir", 0, REG_SZ, (LPBYTE)L"true", sizeof(wchar_t) * 5) == ERROR_SUCCESS)
            ret = true;
        RegCloseKey(pge_key);
    }
    return ret;
}
#endif


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
        }
        //CFRelease(filePathRef);
        CFRelease(appUrlRef);
    }
    #else
    char* path = SDL_GetBasePath();//DirMan(Files::dirname(argv0)).absolutePath();
    ApplicationPathSTD = std::string(path);
    #ifdef _WIN32
    std::replace(ApplicationPathSTD.begin(), ApplicationPathSTD.end(), '\\', '/');
    #endif
    SDL_free(path);
    #endif

//#if defined(__ANDROID__)
//    ApplicationPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/PGE Project Data";
//    QDir appPath(ApplicationPath);

//    if(!appPath.exists())
//        appPath.mkpath(ApplicationPath);

//#endif

    if(isPortable())
        return;

    bool userDir;
#if defined(__ANDROID__) || defined(__APPLE__)
    userDir = true;
#else
    #if defined(__gnu_linux__)
    {
        passwd* pw = getpwuid(getuid());
        std::string path(pw->pw_dir);
        DirMan configDir(path + "/.config/Wohlhabend Networks/");
        if(!configDir.exists())
            configDir.mkpath(".");
        IniProcessing setup(configDir.absolutePath() + "/PGE Engine.conf");
        setup.beginGroup("General");
        userDir = setup.value("EnableUserDir", false).toBool();
        setup.endGroup();
    }
    #elif defined(_WIN32)
    {
        userDir = winReg_isUserDir();
    }
    #else
    userDir = false;
    #endif
#endif

    if(userDir)
    {
        std::string path = getPgeUserDirectory();
        if(!path.empty())
        {
            DirMan appDir(path);
            if(!appDir.exists() && !appDir.mkpath(path))
                goto defaultSettingsPath;
#ifdef __APPLE__
            if(!DirMan::exists(ApplicationPathSTD + "/Data directory"))
                system(fmt::format("ln -s \"{0}\" \"{1}/Data directory\"", path + UserDirName, ApplicationPathSTD).c_str());
#endif
            m_userPath = appDir.absolutePath();
            m_userPath.push_back('/');
            _initSettingsPath();
        }
        else
            goto defaultSettingsPath;
    }
    else
        goto defaultSettingsPath;

    return;
defaultSettingsPath:
    m_userPath = ApplicationPathSTD;
    _initSettingsPath();
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

void AppPathManager::install()
{
    std::string path = getPgeUserDirectory();

    if(!path.empty())
    {
        DirMan appDir(path);
        if(!appDir.exists() && !appDir.mkpath(path))
            return;

        #ifdef _WIN32
        winReg_setUserDir();
        #endif

        #ifdef __gnu_linux__
        DirMan configDir(path + "/.config/Wohlhabend Networks/");
        if(!configDir.exists())
            configDir.mkpath(".");
        IniProcessing setup(configDir.absolutePath() + "/PGE Engine.conf");
        setup.beginGroup("General");
        setup.setValue("EnableUserDir", true);
        setup.endGroup();
        setup.writeIniFile();
        #endif
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
        _initSettingsPath();

    return forcePortable;
}

bool AppPathManager::userDirIsAvailable()
{
    return (m_userPath.compare(ApplicationPathSTD) != 0);
}


void AppPathManager::_initSettingsPath()
{
    m_settingsPath = m_userPath + "settings/";

    if(Files::fileExists(m_settingsPath))
        Files::deleteFile(m_settingsPath);//Just in case, avoid mad jokes with making same-named file as settings folder

    if(!DirMan::exists(m_settingsPath))
        DirMan::mkAbsPath(m_settingsPath);
}
