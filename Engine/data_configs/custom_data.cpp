/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <Utils/files.h>

CustomDirManager::CustomDirManager()
{}

CustomDirManager::CustomDirManager(QString path, QString name, QString stuffPath)
{
    setCustomDirs(path, name, stuffPath);
}

CustomDirManager::CustomDirManager(std::string path, std::string name, std::string stuffPath)
{
    setCustomDirs(path, name, stuffPath);
}

QString CustomDirManager::getCustomFile(QString name, bool *isDefault)
{
    return QString::fromStdString(getCustomFile(name.toStdString(), isDefault));
}

std::string CustomDirManager::getCustomFile(std::string name, bool *isDefault)
{
    if(name.empty())
        return "";
    std::string srcName = name;
    std::string backupName;

    //Try to look up for a backup images (if original not found, try to search images in second format)
    if(Files::hasSuffix(name, ".gif"))
    {
        backupName = srcName;
        Files::changeSuffix(backupName, ".png");
        backupName.replace(backupName.size() - 3, 3, "png");
        //find PNG's first!
        std::string tmp = backupName;
        backupName = srcName;
        srcName = tmp;
    }
    else if(Files::hasSuffix(name, ".png"))
    {
        backupName = srcName;
        Files::changeSuffix(backupName, ".gif");
        //backupName.replace(backupName.size()-3, 3, "gif");
    }

    std::string target = "";
tryBackup:
    if((Files::fileExists(m_dirCustom)) &&
       (Files::fileExists(m_dirCustom + "/" + srcName)))
    {
        target = m_dirCustom + "/" + srcName;
        if(isDefault)
            *isDefault = false;
    }
    else if(Files::fileExists(m_dirEpisode + "/" + srcName))
    {
        target = m_dirEpisode + "/" + srcName;
        if(isDefault) *isDefault = false;
    }
    else
    {
        if((!backupName.empty()) && (backupName != srcName))
        {
            srcName = backupName;
            goto tryBackup;
        }
        target = m_mainStuffFullPath + name;
        if(isDefault) *isDefault = true;
    }

    return target;
}

void CustomDirManager::setCustomDirs(QString path, QString name, QString stuffPath)
{
    setCustomDirs(path.toStdString(), name.toStdString(), stuffPath.toStdString());
}

void CustomDirManager::setCustomDirs(std::string path, std::string name, std::string stuffPath)
{
    m_dirCustom = path + "/" + name;
    m_dirEpisode = path;
    m_mainStuffFullPath = stuffPath;
}
