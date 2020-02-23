/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QFile>
#include <QDir>
#include <utility>

#include "custom_data.h"

CustomDirManager::CustomDirManager(QString path, QString name)
{
    setCustomDirs(std::move(path), std::move(name));
}

QString CustomDirManager::getCustomFile(QString name, bool ignoreDefaultDirectory)
{
    if(name.isEmpty())
        return "";
    QString backupName;

    //Try to look up for a backup images (if original not found, try to search images in second format)
    if(name.endsWith(".gif", Qt::CaseInsensitive))
    {
        backupName = name;
        backupName.replace(backupName.size() - 3, 3, "png");
        //find PNG's first!
        QString tmp = backupName;
        backupName = name;
        name = tmp;
    }
    else if(name.endsWith(".png", Qt::CaseInsensitive))
    {
        backupName = name;
        backupName.replace(backupName.size() - 3, 3, "gif");
    }

    QString target;

    while(1)
    {
        if(!dirCustom.isEmpty()) // Don't try to search in custom directory when it wasn't specified
        {
            const QFileInfo dCustom(dirCustom);
            const QFileInfo dCustomFile(dirCustom + "/" + name);
            const QFileInfo dEpisode(dirEpisode);
            const QFileInfo dEpisodeFile(dirEpisode + "/" + name);

            if(dCustom.exists() && dCustom.isDir() && dCustomFile.exists())
                target = dCustomFile.absoluteFilePath();
            else if(dEpisode.exists() && dEpisode.isDir() && dEpisodeFile.exists())
                target = dEpisodeFile.absoluteFilePath();
            else
                target = findFileInExtraDirs(name);
        }

        if(!ignoreDefaultDirectory && target.isEmpty() && !defaultDirectory.isEmpty())
        {
            const QFileInfo dDefault(defaultDirectory);
            const QFileInfo dDefaultFile(defaultDirectory + "/" + name);

            if(dDefault.exists() && dDefault.isDir() && dDefaultFile.exists())
                target = dDefaultFile.absoluteFilePath();
        }

        if((target.isEmpty()) && (!backupName.isEmpty()) && (backupName != name))
        {
            name = backupName;
            continue;
        }

        break;
    }

    return target;
}

QStringList CustomDirManager::getCustomFiles(QString nameBase, QStringList extensions, bool ignoreDefaultDirectory)
{
    QStringList ret;
    for(QString &e : extensions)
    {
        QString file = getCustomFile(nameBase + e, ignoreDefaultDirectory);
        if(!file.isEmpty())
            ret.push_back(file);
    }
    return ret;
}

void CustomDirManager::setCustomDirs(QString path, QString name)
{
    if(path.isEmpty())
        return; // Don't even try to search here if path is blank
    dirCustom = path + "/" + name;
    dirEpisode = path;
}

void CustomDirManager::setDefaultDir(QString dPath)
{
    defaultDirectory = dPath;
}

void CustomDirManager::addExtraDir(QString dPath)
{
    dirsExtra.push_back(dPath);
}

void CustomDirManager::clearExtraDirs()
{
    dirsExtra.clear();
}

QString CustomDirManager::findFileInExtraDirs(QString fPath)
{
    for(const QString &d : dirsExtra)
    {
        QString f = d + "/" + fPath;
        if(QFile::exists(f))
            return f;
    }
    return QString();
}

void CustomDirManager::createDirIfNotExsist()
{
    if(!QFile::exists(dirCustom))
    {
        QDir tarDir(dirCustom);
        tarDir.mkpath(".");
    }
}

void CustomDirManager::import(QStringList &files, bool local)
{
    QString targetDir = (local ? dirCustom : dirEpisode);
    targetDir = (!targetDir.endsWith("/") ? targetDir.append('/') : targetDir);
    for(QString &targetFile : files)
    {
        QFile sourceFile(targetFile);
        sourceFile.copy(targetDir + targetFile.section("/", -1));
    }
}
