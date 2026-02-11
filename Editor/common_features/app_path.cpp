/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2026 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QApplication>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>
#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <QUrl>
#endif

#ifdef __ANDROID__
#include "dir_copy.h"
#endif
#include "app_path.h"
#include "../version.h"

QString ApplicationPath;
QString ApplicationPath_x;

bool AppPathManager::m_isPortable = false;
QString AppPathManager::m_settingsPath;
QString AppPathManager::m_userPath;
QString AppPathManager::m_dataPath;
QString AppPathManager::m_libExecPath;

#define SHARE_DATA_DIR "moondust-project"
#if defined(__ANDROID__) || defined(__APPLE__) || defined(__HAIKU__)
#   define UserDirName "/PGE Project"
#else
#   define UserDirName "/.PGE_Project"
#endif

void AppPathManager::initAppPath(const char *argv0)
{
#ifdef __APPLE__
    Q_UNUSED(argv0);
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

        //! If it's a path randomizer
        if(ApplicationPath.startsWith("/private/var/"))
        {
            QString realAppPath("/Applications/Moondust Project");
            if(QDir(realAppPath).exists())
                ApplicationPath = realAppPath;
        }
    }
#else
    ApplicationPath = QFileInfo(QString::fromUtf8(argv0)).absoluteDir().absolutePath();
#endif

    ApplicationPath_x = ApplicationPath;

    QApplication::addLibraryPath(".");
    QApplication::addLibraryPath(ApplicationPath);
    QApplication::addLibraryPath(QFileInfo(QString::fromLocal8Bit(argv0)).absoluteDir().absolutePath());

    QApplication::setOrganizationName(V_COMPANY);
    QApplication::setOrganizationDomain(V_PGE_URL);
    QApplication::setApplicationName("PGE Editor");

#ifdef __ANDROID__
    ApplicationPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/PGE Project Data";
    QDir appPath(ApplicationPath);
    if(!appPath.exists())
        appPath.mkpath(ApplicationPath);

    QDir languagesFolder(ApplicationPath + "/languages");
    if(!languagesFolder.exists())
    {
        languagesFolder.mkpath(ApplicationPath + "/languages");
        DirCopy::copy("assets:/languages", languagesFolder.absolutePath());
    }

    QDir themesFolder(ApplicationPath + "/themes");
    if(!themesFolder.exists())
    {
        themesFolder.mkpath(ApplicationPath + "/themes");
        DirCopy::copy("assets:/themes", themesFolder.absolutePath());
    }
#endif

    initIsPortable();

    if(m_isPortable)
        return;

#if defined(__ANDROID__) || defined(__APPLE__)
    QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
#else
    QString path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
#endif
    if(!path.isEmpty())
    {
        QDir appDir(path + UserDirName);
        if(!appDir.exists())
        {
            appDir.mkpath(path + UserDirName);
            if(!appDir.exists()) // Re-check the existance
                goto defaultSettingsPath;
        }
#ifdef __APPLE__
        if(!QDir(ApplicationPath + "/Data directory").exists())
            symlink((path + UserDirName).toUtf8().data(), (ApplicationPath + "/Data directory").toUtf8().data());
#endif
        m_userPath = appDir.absolutePath();
        initSettingsPath();
        initDataPath();
        initLibExecPath();
    }
    else
        goto defaultSettingsPath;

    return;

defaultSettingsPath:
    m_userPath = ApplicationPath;
    initSettingsPath();
    initDataPath();
    initLibExecPath();
}

QString AppPathManager::settingsFile()
{
    return m_settingsPath + "/pge_editor.ini";
}

QString AppPathManager::settingsPath()
{
    return m_settingsPath;
}

QString AppPathManager::userAppDir()
{
    return m_userPath;
}

QString AppPathManager::dataDir()
{
    return m_dataPath;
}

QString AppPathManager::libExecDir()
{
    return m_libExecPath;
}


QString AppPathManager::languagesDir()
{
#ifndef Q_OS_MAC
    return m_dataPath + "/languages";
#else
    CFURLRef appUrlRef;
    appUrlRef = CFBundleCopyResourceURL(CFBundleGetMainBundle(), CFSTR("languages"), NULL, NULL);
    CFStringRef filePathRef = CFURLGetString(appUrlRef);
    QString path = QUrl(QString::fromCFString(filePathRef)).toLocalFile();
    CFRelease(appUrlRef);
    return path;
#endif
}

QString AppPathManager::logsDir()
{
#ifndef Q_OS_MAC
    return m_userPath + "/logs";
#else
    return QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/Library/Logs/PGE Project";
#endif
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
            appDir.mkpath(path + UserDirName);
    }
}

bool AppPathManager::isPortable()
{
    return m_isPortable;
}

bool AppPathManager::userDirIsAvailable()
{
    return !m_isPortable;
}

void AppPathManager::initSettingsPath()
{
    m_settingsPath = m_userPath + "/settings";

    if(QFileInfo(m_settingsPath).isFile())
        QFile::remove(m_settingsPath);//Just in case, avoid mad jokes with making same-named file as settings folder

    QDir st(m_settingsPath);

    if(!st.exists())
        st.mkpath(m_settingsPath);
}

void AppPathManager::initDataPath()
{
#ifndef __APPLE__
    QDir dataPath(ApplicationPath);
    m_dataPath = ApplicationPath;

    dataPath.cdUp();

    if(!dataPath.exists("share"))
        return;

    dataPath.cd("share");

    if(dataPath.exists(SHARE_DATA_DIR))
        m_dataPath = dataPath.absoluteFilePath(SHARE_DATA_DIR);

#else
    m_dataPath = ApplicationPath;
#endif
}

void AppPathManager::initLibExecPath()
{
#ifndef __APPLE__
    QDir dataPath(ApplicationPath);
    m_libExecPath = m_dataPath;

    dataPath.cdUp();

    if(!dataPath.exists("libexec"))
        return;

    dataPath.cd("libexec");

    if(dataPath.exists(SHARE_DATA_DIR))
        m_libExecPath = dataPath.absoluteFilePath(SHARE_DATA_DIR);

#else
    m_libExecPath = m_dataPath;
#endif
}

void AppPathManager::initIsPortable()
{
    if(m_settingsPath.isNull())
        m_settingsPath = ApplicationPath;

    if(m_userPath.isNull())
        m_userPath = ApplicationPath;

    if(m_dataPath.isNull())
        m_dataPath = ApplicationPath;

    if(m_libExecPath.isNull())
        m_libExecPath = ApplicationPath;

    if(!QFile::exists(settingsFile()))
    {
        m_isPortable = false;
        return;
    }

    bool forcePortable = false;
    QSettings checkForPort(settingsFile(), QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    checkForPort.setIniCodec("UTF-8");
#endif

    checkForPort.beginGroup("Main");
    forcePortable = checkForPort.value("force-portable", false).toBool();
    checkForPort.endGroup();

    if(forcePortable)
        initSettingsPath();

    m_isPortable = forcePortable;
}
