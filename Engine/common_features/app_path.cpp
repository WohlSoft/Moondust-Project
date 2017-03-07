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

#include "pge_qt_application.h"
#include <DirManager/dirman.h>
#include <Utils/files.h>
#include <IniProcessor/ini_processing.h>
#define FMT_NOEXCEPT
#include <fmt/fmt_format.h>

#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#include <QUrl>
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

void AppPathManager::initAppPath(const char* argv0)
{
    //PGE_Application::setOrganizationName(_COMPANY);
    //PGE_Application::setOrganizationDomain(_PGE_URL);
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
        //const char* filePath = CFStringGetCStringPtr(filePathRef, kCFStringEncodingUTF8);
        ApplicationPath = QUrl(QString::fromCFString(filePathRef)).toLocalFile();
        {
            int i = ApplicationPath.lastIndexOf(".app");
            i = ApplicationPath.lastIndexOf('/', i);
            ApplicationPath.remove(i, ApplicationPath.size() - i);
        }
        //CFRelease(filePathRef);
        CFRelease(appUrlRef);
    }
    #else
    ApplicationPathSTD = DirMan(Files::dirname(argv0)).absolutePath();
    #endif

#if defined(__ANDROID__)
    ApplicationPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/PGE Project Data";
    QDir appPath(ApplicationPath);

    if(!appPath.exists())
        appPath.mkpath(ApplicationPath);

#endif

    if(isPortable())
        return;

    bool userDir;
#if defined(__ANDROID__) || defined(__APPLE__)
    userDir = true;
#else
    QSettings setup;
    userDir = setup.value("EnableUserDir", false).toBool();
#endif
    //openUserDir:

    if(userDir)
    {
#if defined(__ANDROID__) || defined(__APPLE__)
        std::string path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation).toStdString();
#else
        std::string path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation).toStdString();
#endif

        if(!path.empty())
        {
            DirMan appDir(path + UserDirName);

            if(!appDir.exists())
                if(!appDir.mkpath(path + UserDirName))
                    goto defaultSettingsPath;

#ifdef __APPLE__
            if(!DirMan::exists(ApplicationPathSTD + "/Data directory"))
                system(fmt::format("ln -s \"{0}\" \"{1}/Data directory\"", path + UserDirName, ApplicationPathSTD).c_str());
#endif

            m_userPath = appDir.absolutePath();
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
    return m_settingsPath + "/pge_engine.ini";
}

std::string AppPathManager::userAppDirSTD()
{
    return m_userPath;
}



void AppPathManager::install()
{
#if defined(__ANDROID__) || defined(__APPLE__)
    QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
#else
    QString path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#endif

    if(!path.isEmpty())
    {
        QDir appDir(path + UserDirName);

        if(!appDir.exists())
            if(!appDir.mkpath(path + UserDirName))
                return;

        QSettings setup;
        setup.setValue("EnableUserDir", true);
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
    m_settingsPath = m_userPath + "/settings";

    if(Files::fileExists(m_settingsPath))
        Files::deleteFile(m_settingsPath);//Just in case, avoid mad jokes with making same-named file as settings folder

    if(!DirMan::exists(m_settingsPath))
        DirMan::mkAbsPath(m_settingsPath);
}
