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
#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#include <QUrl>
#endif

#include "app_path.h"
#include "../version.h"


QString ApplicationPath;
QString ApplicationPath_x;

QString AppPathManager::_settingsPath;

#if __ANDROID__ || __APPLE__
#define UserDirName "/PGE Project"
#else
#define UserDirName "/.PGE_Project"
#endif

void AppPathManager::initAppPath()
{
    QApplication::setOrganizationName(_COMPANY);
    QApplication::setOrganizationDomain(_PGE_URL);
    QApplication::setApplicationName("PGE Maintainer");

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
    ApplicationPath = QApplication::applicationDirPath();
    #endif
    ApplicationPath_x = ApplicationPath;

    #ifdef __ANDROID__
    ApplicationPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)+"/PGE Project Data";
//    QDir appPath(ApplicationPath);
//    if(!appPath.exists())
//        appPath.mkpath(ApplicationPath);

//    QDir languagesFolder(ApplicationPath+"/languages");
//    if(!languagesFolder.exists())
//    {
//        languagesFolder.mkpath(ApplicationPath+"/languages");
//        DirCopy::copy("assets:/languages", languagesFolder.absolutePath());
//    }
    #endif

    if(isPortable())
        return;

    bool userDir;
    #if __ANDROID__ || __APPLE__
    userDir = true;
    #else
    QSettings setup;
    userDir = setup.value("EnableUserDir", false).toBool();
    #endif
//openUserDir:

    if(userDir)
    {
        #if __ANDROID__||__APPLE__
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

            _settingsPath = appDir.absolutePath();
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
    _settingsPath = ApplicationPath;
}

QString AppPathManager::settingsFile()
{
    return _settingsPath+"/pge_maintainer.ini";
}

QString AppPathManager::userAppDir()
{
    return _settingsPath;
}

void AppPathManager::install()
{
    #if __ANDROID__||__APPLE__
    QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    #else
    QString path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    #endif
    if(!path.isEmpty())
    {
        QDir appDir(path+UserDirName);
        if(!appDir.exists())
            if(!appDir.mkpath(path+UserDirName))
                return;

        QSettings setup;
        setup.setValue("EnableUserDir", true);
    }
}

bool AppPathManager::isPortable()
{
    if(_settingsPath.isNull())
        _settingsPath = ApplicationPath;
    if(!QFile(settingsFile()).exists()) return false;
    bool forcePortable=false;
    QSettings checkForPort(settingsFile(), QSettings::IniFormat);
    checkForPort.beginGroup("Main");
        forcePortable= checkForPort.value("force-portable", false).toBool();
    checkForPort.endGroup();

    return forcePortable;
}

bool AppPathManager::userDirIsAvailable()
{
    return (_settingsPath != ApplicationPath);
}

