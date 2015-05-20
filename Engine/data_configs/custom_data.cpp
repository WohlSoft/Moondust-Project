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
    QString target="";
    if((QFile::exists(dirCustom) ) &&
            (QFile::exists(dirCustom+"/" + name)) )
    {
        target = dirCustom+"/"+name;
        if(isDefault) *isDefault = false;
    }
    else
    if(QFile::exists(dirEpisode + "/" + name) )
    {
        target = dirEpisode + "/" + name;
        if(isDefault) *isDefault = false;
    }
//    else
//    if(QFile::exists(mainStuffFullPath + "/" + name) )
//    {
//        target = mainStuffFullPath + name;
//        if(isDefault) *isDefault = true;
//    }
    else
    {
        target = mainStuffFullPath + name;
        if(isDefault) *isDefault = true;
    }

    return target;
}

void CustomDirManager::setCustomDirs(QString path, QString name, QString stuffPath)
{
    dirCustom = path + "/" + name;
    dirEpisode = path;
    mainStuffFullPath = stuffPath;
}

