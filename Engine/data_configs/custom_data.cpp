/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#include "custom_data.h"
#include <Utils/files.h>
#include <utility>

CustomDirManager::CustomDirManager(std::string path, std::string name, std::string stuffPath)
{
    setCustomDirs(std::move(path), std::move(name), std::move(stuffPath));
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
        backupName = Files::changeSuffix(backupName, ".png");
        //backupName.replace(backupName.size() - 3, 3, "png");
        //find PNG's first!
        std::string tmp = backupName;
        backupName = srcName;
        srcName = tmp;
    }
    else if(Files::hasSuffix(name, ".png"))
    {
        backupName = srcName;
        backupName = Files::changeSuffix(backupName, ".gif");
        //backupName.replace(backupName.size()-3, 3, "gif");
    }

    std::string target;
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
        if(isDefault)
            *isDefault = false;
    }
    else
    {
        if((!backupName.empty()) && (backupName != srcName))
        {
            srcName = backupName;
            goto tryBackup;
        }
        target = m_mainStuffFullPath + name;
        if(isDefault)
            *isDefault = true;
    }

    if(!Files::fileExists(target))
    {
        target.clear();
        if(isDefault)
            *isDefault = false;
    }

    return target;
}

std::string CustomDirManager::getDefaultFile(std::string name)
{
    std::string target;
    target = m_mainStuffFullPath + name;
    if(!Files::fileExists(target))
        return std::string();
    return target;
}

std::string CustomDirManager::getMaskFallbackFile(std::string name)
{
    if(Files::hasSuffix(name, ".png"))
        return getDefaultFile(name);
    return std::string();
}

void CustomDirManager::setCustomDirs(std::string path, std::string name, std::string stuffPath)
{
    m_dirCustom = path + "/" + name;
    m_dirEpisode = path;
    m_mainStuffFullPath = std::move(stuffPath);
}
