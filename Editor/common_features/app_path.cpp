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

#include <QApplication>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>
#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#include <QUrl>
#endif

#include "dir_copy.h"
#include "app_path.h"
#include "../version.h"

QString ApplicationPath;
QString ApplicationPath_x;

QString AppPathManager::m_settingsPath;
QString AppPathManager::m_userPath;

#if defined(__ANDROID__) || defined(__APPLE__)
#define UserDirName "/PGE Project"
#else
#define UserDirName "/.PGE_Project"
#endif

void AppPathManager::initAppPath(const char* argv0)
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
    }
    #else
    ApplicationPath = QFileInfo(QString::fromUtf8(argv0)).absoluteDir().absolutePath();
    #endif
    ApplicationPath_x = ApplicationPath;

    QApplication::addLibraryPath(".");
    QApplication::addLibraryPath(ApplicationPath);
    QApplication::addLibraryPath(QFileInfo(QString::fromLocal8Bit(argv0)).absoluteDir().absolutePath());

    QApplication::setOrganizationName(_COMPANY);
    QApplication::setOrganizationDomain(_PGE_URL);
    QApplication::setApplicationName("PGE Editor");

    #ifdef __ANDROID__
    ApplicationPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)+"/PGE Project Data";
    QDir appPath(ApplicationPath);
    if(!appPath.exists())
        appPath.mkpath(ApplicationPath);

    QDir languagesFolder(ApplicationPath+"/languages");
    if(!languagesFolder.exists())
    {
        languagesFolder.mkpath(ApplicationPath+"/languages");
        DirCopy::copy("assets:/languages", languagesFolder.absolutePath());
    }

    QDir themesFolder(ApplicationPath+"/themes");
    if(!themesFolder.exists())
    {
        themesFolder.mkpath(ApplicationPath+"/themes");
        DirCopy::copy("assets:/themes", themesFolder.absolutePath());
    }
    #endif

    if(isPortable())
        return;

    QSettings setup;
    bool userDir;
    #if defined(__ANDROID__) || defined(__APPLE__)
    userDir = true;
    #else
    userDir = setup.value("EnableUserDir", false).toBool();
    #endif
//openUserDir:

    if(userDir)
    {
        #if defined(__ANDROID__) || defined(__APPLE__)
        QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
        #else
        QString path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        #endif
        if(!path.isEmpty())
        {
            QDir appDir(path+UserDirName);
            if(!appDir.exists())
                if(!appDir.mkpath(path+UserDirName))
                    goto defaultSettingsPath;
            #ifdef __APPLE__
                if(!QDir(ApplicationPath+"/Data directory").exists()) {
                    system(QString("ln -s \"%1\" \"%2/Data directory\"").arg(path+UserDirName).arg(ApplicationPath).toLocal8Bit().data());
                }
            #endif
            m_userPath = appDir.absolutePath();
            _initSettingsPath();
        }
        else
        {
            goto defaultSettingsPath;
        }
    }
    else
    {
        goto defaultSettingsPath;
    }

    return;
defaultSettingsPath:
    m_userPath = ApplicationPath;
    _initSettingsPath();
}

QString AppPathManager::settingsFile()
{
    return m_settingsPath + "/pge_editor.ini";
}

QString AppPathManager::userAppDir()
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
    if(m_settingsPath.isNull())
        m_settingsPath = ApplicationPath;
    if(m_userPath.isNull())
        m_userPath = ApplicationPath;
    if(!QFile(settingsFile()).exists()) return false;
    bool forcePortable=false;
    QSettings checkForPort(settingsFile(), QSettings::IniFormat);

    checkForPort.beginGroup("Main");
        forcePortable= checkForPort.value("force-portable", false).toBool();
    checkForPort.endGroup();

    if(forcePortable)
        _initSettingsPath();

    return forcePortable;
}

bool AppPathManager::userDirIsAvailable()
{
    return (m_userPath != ApplicationPath);
}

void AppPathManager::_initSettingsPath()
{
    m_settingsPath = m_userPath + "/settings";
    if(QFileInfo(m_settingsPath).isFile())
        QFile::remove(m_settingsPath);//Just in case, avoid mad jokes with making same-named file as settings folder
    QDir st(m_settingsPath);
    if(!st.exists())
        st.mkpath(m_settingsPath);
}

