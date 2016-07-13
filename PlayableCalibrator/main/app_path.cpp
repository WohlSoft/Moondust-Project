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

#include "app_path.h"
#include "../version.h"

QString ApplicationPath;
QString ApplicationPath_x;

QString AppPathManager::m_settingsPath;
QString AppPathManager::m_userPath;

#if __ANDROID__ || __APPLE__
#define UserDirName "/PGE Project"
#else
#define UserDirName "/.PGE_Project"
#endif

void AppPathManager::initAppPath()
{
    QApplication::setOrganizationName(_COMPANY);
    QApplication::setOrganizationDomain(_PGE_URL);
    QApplication::setApplicationName("Playable Character Calibrator");

    ApplicationPath = QApplication::applicationDirPath();
    ApplicationPath_x = QApplication::applicationDirPath();

    #ifdef __APPLE__
    //Application path relative bundle folder of application
    ApplicationPath = QFileInfo(ApplicationPath_x+"../../..").absoluteDir().absolutePath();
    #endif

    /*
    QString osX_bundle = QApplication::applicationName()+".app/Contents/MacOS";
    QString test="/home/vasya/pge/"+osX_bundle;
    qDebug() << test << " <- before";
    if(test.endsWith(osX_bundle, Qt::CaseInsensitive))
        test.remove(test.length()-osX_bundle.length()-1, osX_bundle.length()+1);
    qDebug() << test << " <- after";
    */

    if(isPortable())
        return;

    QSettings setup;
    bool userDir;
    #if __ANDROID__ || __APPLE__
    userDir = true;
    #else
    userDir = setup.value("EnableUserDir", false).toBool();
    #endif

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
    return m_settingsPath+"/pge_calibrator.ini";
}

QString AppPathManager::userAppDir()
{
    return m_userPath;
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
    if(m_settingsPath.isNull())
        m_settingsPath = ApplicationPath;
    if(m_userPath.isNull())
        m_userPath = ApplicationPath;
    if(!QFile(settingsFile()).exists()) return false;
    bool forcePortable=false;
    QSettings checkForPort(settingsFile(), QSettings::IniFormat);
    checkForPort.beginGroup("Main");
        forcePortable=checkForPort.value("force-portable", false).toBool();
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

