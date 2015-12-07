/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "custom_data.h"
#include <QFile>

CustomDirManager::CustomDirManager()
{}

CustomDirManager::CustomDirManager(QString path, QString name, QString stuffPath)
{
    setCustomDirs(path, name, stuffPath);
}

QString CustomDirManager::getCustomFile(QString name, bool *isDefault)
{
    if(name.isEmpty()) return "";
    QString srcName=name;
    QString backupName;

    //Try to look up for a backup images (if original not found, try to search images in second format)
    if(name.endsWith(".gif", Qt::CaseInsensitive))
    {
       backupName=srcName;
       backupName.replace(backupName.size()-3, 3, "png");
       //find PNG's first!
       QString tmp=backupName;
       backupName=srcName;
       srcName=tmp;
    } else if(name.endsWith(".png", Qt::CaseInsensitive)) {
        backupName=srcName;
        backupName.replace(backupName.size()-3, 3, "gif");
    }

    QString target="";
tryBackup:
    if((QFile::exists(m_dirCustom) ) &&
            (QFile::exists(m_dirCustom+"/" + srcName)) )
    {
        target = m_dirCustom+"/"+srcName;
        if(isDefault) *isDefault = false;
    }
    else
    if(QFile::exists(m_dirEpisode + "/" + srcName) )
    {
        target = m_dirEpisode + "/" + srcName;
        if(isDefault) *isDefault = false;
    }
    else
    {
        if((!backupName.isEmpty()) && (backupName!=srcName))
        {
            srcName=backupName;
            goto tryBackup;
        }
        target = m_mainStuffFullPath + name;
        if(isDefault) *isDefault = true;
    }

    return target;
}

void CustomDirManager::setCustomDirs(QString path, QString name, QString stuffPath)
{
    m_dirCustom = path + "/" + name;
    m_dirEpisode = path;
    m_mainStuffFullPath = stuffPath;
}

