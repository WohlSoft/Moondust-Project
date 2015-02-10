/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

QString AppPathManager::_settingsPath;

void AppPathManager::initAppPath()
{
    QApplication::setOrganizationName(_COMPANY);
    QApplication::setOrganizationDomain(_PGE_URL);
    QApplication::setApplicationName("PGE Editor");

    ApplicationPath = QApplication::applicationDirPath();
    ApplicationPath_x = QApplication::applicationDirPath();

    #ifdef __APPLE__
    //Application path relative bundle folder of application
    QString osX_bundle = QApplication::applicationName()+".app/Contents/MacOS";
    if(ApplicationPath.endsWith(osX_bundle, Qt::CaseInsensitive))
        ApplicationPath.remove(ApplicationPath.length()-osX_bundle.length()-1, osX_bundle.length()+1);
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
    userDir = setup.value("EnableUserDir", false).toBool();
//openUserDir:

    if(userDir)
    {
        QString path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        if(!path.isEmpty())
        {
            QDir appDir(path+"/.PGE_Project");
            if(!appDir.exists())
                if(!appDir.mkpath(path+"/.PGE_Project"))
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
    return _settingsPath+"/pge_editor.ini";
}

QString AppPathManager::userAppDir()
{
    return _settingsPath;
}

void AppPathManager::install()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    if(!path.isEmpty())
    {
        QDir appDir(path+"/.PGE_Project");
        if(!appDir.exists())
            if(!appDir.mkpath(path+"/.PGE_Project"))
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


