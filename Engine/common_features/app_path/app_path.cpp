/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2026 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <DirManager/dirman.h>
#include <Utils/files.h>
#include <IniProcessor/ini_processing.h>
#define FMT_NOEXCEPT
#include <fmt/fmt_format.h>

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <PGE_File_Formats/pge_file_lib_private.h>//It's only exception for macOS here to get URL-Decode. Never include this!
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

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

static bool loadingLocked = false;

extern "C" void unlockLoadingCustomState()
{
    loadingLocked = false;
}

static void loadCustomState()
{
    loadingLocked = true;
    EM_ASM(
        FS.mkdir('/settings');
        FS.mount(IDBFS, {}, '/settings');

        // sync from persisted state into memory and then
        // run the 'test' function
        FS.syncfs(true, function (err) {
            assert(!err);
            ccall('unlockLoadingCustomState', 'v');
        });
    );

    while(loadingLocked)
        emscripten_sleep(10);
}

static void saveCustomState()
{
    loadingLocked = true;
    EM_ASM(
        FS.syncfs(function (err) {
            assert(!err);
            ccall('unlockLoadingCustomState', 'v');
        });
    );

    while(loadingLocked)
        emscripten_sleep(10);
}
#endif

#ifdef __ANDROID__
#   include <unistd.h>
#   include <jni.h>
#   if 1
#       undef JNIEXPORT
#       undef JNICALL
#       define JNIEXPORT extern "C"
#       define JNICALL
#   endif
#endif

#include "app_path.h"
#include "../../version.h"

#include <SDL2/SDL.h>

std::string  ApplicationPathSTD;

bool EnginePathMan::m_isPortable = false;
std::string EnginePathMan::m_settingsPath;
std::string EnginePathMan::m_userPath;
std::string EnginePathMan::m_dataPath;

#define SHARE_DATA_DIR "moondust-project"
#if defined(__ANDROID__) || defined(__APPLE__) || defined(__HAIKU__)
#define UserDirName "/PGE Project"
#else
#define UserDirName "/.PGE_Project"
#endif

#ifdef __ANDROID__
//! Default path to the internal sotrage directory
static std::string m_androidSdCardPath = "/storage/emulated/0";

JNIEXPORT void JNICALL
Java_ru_wohlsoft_moondust_moondustActivity_setSdCardPath(
    JNIEnv *env,
    jclass type,
    jstring sdcardPath_j
)
{
    const char *sdcardPath;
    (void)type;
    sdcardPath = env->GetStringUTFChars(sdcardPath_j, nullptr);
    m_androidSdCardPath = sdcardPath;
    env->ReleaseStringUTFChars(sdcardPath_j, sdcardPath);
}
#endif

static void appendSlash(std::string &path)
{
#if defined(__EMSCRIPTEN__)
    // fix emscripten bug of duplicated worlds
    if(path.empty() || path.back() != '/')
        path.push_back('/');
#else
    if(!path.empty() && path.back() != '/')
        path.push_back('/');
#endif
}

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
    path = m_androidSdCardPath;
#elif defined(__HAIKU__)
    {
        const char *home = SDL_getenv("HOME");
        path.append(home);
    }
#elif defined(__gnu_linux__)
    {
        passwd *pw = getpwuid(getuid());
        path.append(pw->pw_dir);
    }
#endif

    return path.empty() ? std::string(".") : (path + UserDirName);
}


void EnginePathMan::initAppPath()
{
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
        CFRelease(appUrlRef);
    }
#else //__APPLE__
    char *path = SDL_GetBasePath();
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

    appendSlash(ApplicationPathSTD);

#ifdef __EMSCRIPTEN__
    loadCustomState();
#endif

    initIsPortable();

    if(m_isPortable)
        return;

    std::string userDirPath = getPgeUserDirectory();
    if(!userDirPath.empty())
    {
        DirMan appDir(userDirPath);
        if(!appDir.exists())
        {
            appDir.mkpath(userDirPath);
            if(!appDir.exists()) // Re-check the existance
                goto defaultSettingsPath;
        }
#ifdef __APPLE__
        if(!DirMan::exists(ApplicationPathSTD + "/Data directory"))
            symlink((userDirPath).c_str(), (ApplicationPathSTD + "/Data directory").c_str());
#endif

#ifdef __ANDROID__
        std::string noMediaFile = userDirPath + "/.nomedia";
        if(!Files::fileExists(noMediaFile))
        {
            SDL_RWops *noMediaRWops = SDL_RWFromFile(noMediaFile.c_str(), "wb");
            if(noMediaRWops)
                SDL_RWclose(noMediaRWops);
        }
#endif
        m_userPath = appDir.absolutePath();
        appendSlash(m_userPath);
        initSettingsPath();
        initDataPath();
    }
    else
    {
        goto defaultSettingsPath;
    }

    return;
defaultSettingsPath:
    m_userPath = ApplicationPathSTD;
    appendSlash(m_userPath);
    initSettingsPath();
    initDataPath();
#ifdef __EMSCRIPTEN__
    printf("== App Path is %s\n", ApplicationPathSTD.c_str());
    printf("== User Path is %s\n", m_userPath.c_str());
    fflush(stdout);
#endif
}

std::string EnginePathMan::settingsFileSTD()
{
    return m_settingsPath + "pge_engine.ini";
}

std::string EnginePathMan::userAppDirSTD()
{
    return m_userPath;
}

std::string EnginePathMan::dataDir()
{
    return m_dataPath;
}

std::string EnginePathMan::languagesDir()
{
#if defined(__APPLE__)
    CFURLRef appUrlRef;
    appUrlRef = CFBundleCopyResourceURL(CFBundleGetMainBundle(), CFSTR("languages"), NULL, NULL);
    CFStringRef filePathRef = CFURLGetString(appUrlRef);
    char temporaryCString[PATH_MAX];
    bzero(temporaryCString, PATH_MAX);
    CFStringGetCString(filePathRef, temporaryCString, PATH_MAX, kCFStringEncodingUTF8);

    std::string path = PGE_URLDEC(std::string(temporaryCString));

    if(path.compare(0, 7, "file://") == 0)
        path.erase(0, 7);

    appendSlash(path);
    return path;
#elif defined(__ANDROID__)
    return "languages/";
#else
    return m_dataPath + "languages/";
#endif
}

std::string EnginePathMan::logsDir()
{
    return m_userPath + "logs/";
}

std::string EnginePathMan::screenshotsDir()
{
#ifndef __APPLE__
    return m_userPath + "screenshots/";
#else
    std::string path = m_userPath;
    char *base_path = getScreenCaptureDir();
    if(base_path)
    {
        path = base_path;
        SDL_free(base_path);
    }
    return path + "/Moondust Game Screenshots/";
#endif
}

std::string EnginePathMan::gameSaveRootDir()
{
    return m_settingsPath + "gamesaves/";
}

void EnginePathMan::install()
{
    std::string path = getPgeUserDirectory();

    if(!path.empty())
    {
        DirMan appDir(path);
        if(!appDir.exists())
            appDir.mkpath(path);
    }
}

bool EnginePathMan::isPortable()
{
    return m_isPortable;
}

bool EnginePathMan::userDirIsAvailable()
{
    return !m_isPortable;
}

#ifdef __EMSCRIPTEN__
void EnginePathMan::syncFs()
{
    saveCustomState();
}
#endif


void EnginePathMan::initSettingsPath()
{
    m_settingsPath = m_userPath + "settings/";

    if(Files::fileExists(m_settingsPath))
        Files::deleteFile(m_settingsPath);//Just in case, avoid mad jokes with making same-named file as settings folder

    if(!DirMan::exists(m_settingsPath))
        DirMan::mkAbsPath(m_settingsPath);

    // Also make the gamesaves root folder to be exist
    if(!DirMan::exists(gameSaveRootDir()))
        DirMan::mkAbsPath(gameSaveRootDir());

    appendSlash(m_settingsPath);
}

void EnginePathMan::initDataPath()
{
#ifndef __APPLE__
    m_dataPath = ApplicationPathSTD;
    std::string dataPath = ApplicationPathSTD + "../share/" + SHARE_DATA_DIR;

    if(!DirMan::exists(ApplicationPathSTD + "../share/"))
        return;

    if(DirMan::exists(dataPath))
        m_dataPath = dataPath;

    appendSlash(m_dataPath);

#else
    m_dataPath = ApplicationPathSTD;
#endif
}

void EnginePathMan::initIsPortable()
{
    m_isPortable = false;

    if(m_settingsPath.empty())
        m_settingsPath = ApplicationPathSTD;

    if(m_userPath.empty())
        m_userPath = ApplicationPathSTD;

    if(m_dataPath.empty())
        m_dataPath = ApplicationPathSTD;

    if(!Files::fileExists(settingsFileSTD()))
        return;

    bool forcePortable = false;

    IniProcessing checkForPort(settingsFileSTD());
    checkForPort.beginGroup("Main");
    forcePortable = checkForPort.value("force-portable", false).toBool();
    checkForPort.endGroup();

    if(forcePortable)
        initSettingsPath();

    m_isPortable = forcePortable;
}
