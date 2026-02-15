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

#include "../pge_app_path.h"
#include "pge_app_path_private.h"
#include PGE_APP_PATH_VERSION_HEADER

QString ApplicationPath;
QString ApplicationPath_x;

bool AppPathManagerPrivate::m_isPortable = false;

QString AppPathManagerPrivate::m_userPath;
QString AppPathManagerPrivate::m_dataPath;
QString AppPathManagerPrivate::m_libExecPath;

QString AppPathManagerPrivate::m_settingsPath;
QString AppPathManagerPrivate::m_languagesPath;
QString AppPathManagerPrivate::m_logsPath;


void AppPathManager::initAppPath(const char *argv0)
{
    using namespace AppPathManagerPrivate;

    QApplication::addLibraryPath(".");
    QApplication::addLibraryPath(QFileInfo(QString::fromLocal8Bit(argv0)).absoluteDir().absolutePath());

    QApplication::setOrganizationName(V_COMPANY);
    QApplication::setOrganizationDomain(V_PGE_URL);
    QApplication::setApplicationName(PGE_APP_PATH_MODULE_NAME);

    initAppPathPrivate(argv0);

    ApplicationPath_x = ApplicationPath;
    QApplication::addLibraryPath(ApplicationPath);

    initIsPortable();

    if(m_isPortable)
    {
        m_dataPath = ApplicationPath;
        m_libExecPath = m_dataPath;
        initLangsDir();
        initLogsDir();
        return;
    }

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
        initLangsDir();
        initLogsDir();
    }
    else
        goto defaultSettingsPath;

    return;

defaultSettingsPath:
    m_userPath = ApplicationPath;
    initSettingsPath();
    initDataPath();
    initLibExecPath();
    initLangsDir();
    initLogsDir();
}

QString AppPathManager::settingsFile()
{
    return AppPathManagerPrivate::m_settingsPath + "/" + PGE_APP_PATH_SETUP_FILE_NAME;
}

QString AppPathManager::settingsPath()
{
    return AppPathManagerPrivate::m_settingsPath;
}

QString AppPathManager::userAppDir()
{
    return AppPathManagerPrivate::m_userPath;
}

QString AppPathManager::dataDir()
{
    return AppPathManagerPrivate::m_dataPath;
}

QString AppPathManager::libExecDir()
{
    return AppPathManagerPrivate::m_libExecPath;
}


QString AppPathManager::languagesDir()
{
    return AppPathManagerPrivate::m_languagesPath;
}

QString AppPathManager::logsDir()
{
    return AppPathManagerPrivate::m_logsPath;
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
    return AppPathManagerPrivate::m_isPortable;
}

bool AppPathManager::userDirIsAvailable()
{
    return !AppPathManagerPrivate::m_isPortable;
}

void AppPathManagerPrivate::initSettingsPath()
{
    m_settingsPath = m_userPath + "/settings";

    if(QFileInfo(m_settingsPath).isFile())
        QFile::remove(m_settingsPath);  // Just in case, avoid mad jokes with making same-named file as settings folder

    QDir st(m_settingsPath);

    if(!st.exists())
        st.mkpath(m_settingsPath);
}

void AppPathManagerPrivate::initIsPortable()
{
    if(m_settingsPath.isNull())
        m_settingsPath = ApplicationPath;

    if(m_userPath.isNull())
        m_userPath = ApplicationPath;

    if(m_dataPath.isNull())
        m_dataPath = ApplicationPath;

    if(m_libExecPath.isNull())
        m_libExecPath = ApplicationPath;

    if(!QFile::exists(AppPathManager::settingsFile()))
    {
        m_isPortable = false;
        return;
    }

    bool forcePortable = false;
    QSettings checkForPort(AppPathManager::settingsFile(), QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0) // In Qt6, INI files are always UTF-8
    checkForPort.setIniCodec("UTF-8");
#endif

    checkForPort.beginGroup("Main");
    forcePortable = checkForPort.value("force-portable", false).toBool();
    checkForPort.endGroup();

    if(forcePortable)
        initSettingsPath();

    m_isPortable = forcePortable;
}
