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

#include <QFile>
#include <QDir>

#include "custom_data.h"

CustomDirManager::CustomDirManager()
{}

CustomDirManager::CustomDirManager(QString path, QString name)
{
    setCustomDirs(path, name);
}

QString CustomDirManager::getCustomFile(QString name)
{
    if(name.isEmpty()) return "";
    QString target="";
    if((QFile::exists(dirCustom) ) &&
            (QFile::exists(dirCustom+"/" + name)) )
    {
        target = dirCustom+"/"+name;
    }
    else
    if(QFile::exists(dirEpisode + "/" + name) )
    {
        target = dirEpisode + "/" + name;
    }

    return target;
}

void CustomDirManager::setCustomDirs(QString path, QString name)
{
    dirCustom = path + "/" + name;
    dirEpisode = path;
}

void CustomDirManager::createDirIfNotExsist()
{
    if(!QFile::exists(dirCustom)){
        QDir tarDir(dirCustom);
        tarDir.mkpath(".");
    }
}

void CustomDirManager::import(QStringList &files, bool local)
{
    QString targetDir = (local ? dirCustom : dirEpisode);
    targetDir = (!targetDir.endsWith("/") ? targetDir.append('/') : targetDir);
    foreach (QString targetFile, files) {
        QFile sourceFile(targetFile);
        sourceFile.copy(targetDir + targetFile.section("/", -1));
    }
}
