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

#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>

#include <QDir>
#include <QUrl>
#include <QStandardPaths>

#include "../pge_app_path.h"
#include "pge_app_path_private.h"


void AppPathManagerPrivate::initAppPathPrivate(const char *argv0)
{
    Q_UNUSED(argv0);
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

void AppPathManagerPrivate::initDataPath()
{
    m_dataPath = ApplicationPath;
}

void AppPathManagerPrivate::initLibExecPath()
{
    m_libExecPath = m_dataPath;
}

void AppPathManagerPrivate::initLangsDir()
{
    CFURLRef appUrlRef;
    appUrlRef = CFBundleCopyResourceURL(CFBundleGetMainBundle(), CFSTR("languages"), NULL, NULL);
    CFStringRef filePathRef = CFURLGetString(appUrlRef);
    QString path = QUrl(QString::fromCFString(filePathRef)).toLocalFile();
    CFRelease(appUrlRef);
    m_languagesPath = path;
}

void AppPathManagerPrivate::initLogsDir()
{
    m_logsPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/Library/Logs/PGE Project";
}
