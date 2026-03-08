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

#include <QQueue>
#include <QPair>
#include <QDir>
#include <QFile>
#include <QStandardPaths>

#include "../pge_app_path.h"
#include "pge_app_path_private.h"


static void copyWholeDir(const QString& sourceFolder,const QString& destFolder)
{
    QQueue< QPair<QString, QString> > queue;

    queue.enqueue(qMakePair(sourceFolder, destFolder));

    while (!queue.isEmpty())
    {
        QPair<QString, QString> pair = queue.dequeue();
        QDir sourceDir(pair.first);
        QDir destDir(pair.second);

        if(!sourceDir.exists())
        continue;

        if(!destDir.exists())
        destDir.mkpath(pair.second);

        QStringList files = sourceDir.entryList(QDir::Files);
        for(int i = 0; i < files.count(); i++)
        {
            QString srcName = pair.first + "/" + files.at(i);
            QString destName = pair.second + "/" + files.at(i);
            QFile::copy(srcName, destName);
        }

        QStringList dirs = sourceDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
        for(int i = 0; i < dirs.count(); i++)
        {
            QString srcName = pair.first + "/" + dirs.at(i);
            QString destName = pair.second + "/" + dirs.at(i);
            queue.enqueue(qMakePair(srcName, destName));
        }
    }
}


void AppPathManagerPrivate::initAppPathPrivate(const char *argv0)
{
    Q_UNUSED(argv0)

    ApplicationPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/PGE Project Data";

    QDir appPath(ApplicationPath);
    if(!appPath.exists())
        appPath.mkpath(ApplicationPath);

    QDir languagesFolder(ApplicationPath + "/languages");
    if(!languagesFolder.exists())
    {
        languagesFolder.mkpath(ApplicationPath + "/languages");
        copyWholeDir("assets:/languages", languagesFolder.absolutePath());
    }

#   ifdef PGE_APP_PATH_HAS_THEMES
    QDir themesFolder(ApplicationPath + "/themes");
    if(!themesFolder.exists())
    {
        themesFolder.mkpath(ApplicationPath + "/themes");
        DirCopy::copy("assets:/themes", themesFolder.absolutePath());
    }
#   endif
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
    m_languagesPath = m_dataPath + "/languages";
}

void AppPathManagerPrivate::initLogsDir()
{
    m_logsPath = m_userPath + "/logs";
}
