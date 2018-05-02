/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#pragma once
#ifndef OBJ_TILESETS_H
#define OBJ_TILESETS_H

#include <QString>

#include "../defines.h"

struct SimpleTilesetItem{
    unsigned int row,col,id;
};

struct SimpleTileset{
    unsigned int rows, cols;
    QString tileSetName;
    QString fileName;
    int type;
    bool customDir;
    QList<SimpleTilesetItem> items;
};

struct SimpleTilesetGroup{
    QString groupName;
    QString groupCat;
    int     groupWeight;
    QStringList tilesets;
    bool operator<(const SimpleTilesetGroup& other) const
    {
        if(groupCat != other.groupCat)
            return groupCat.compare(other.groupCat, Qt::CaseInsensitive) < 0;
        if((groupWeight < 0) || (other.groupWeight < 0) || (groupWeight == other.groupWeight))
            return groupName.compare(other.groupName, Qt::CaseInsensitive) < 0;
        return groupWeight < other.groupWeight;
    }
    bool operator>(const SimpleTilesetGroup& other) const
    {
        if(groupCat != other.groupCat)
            return groupCat.compare(other.groupCat, Qt::CaseInsensitive) > 0;
        if((groupWeight < 0) || (other.groupWeight < 0) || (groupWeight == other.groupWeight))
            return groupName.compare(other.groupName, Qt::CaseInsensitive) > 0;
        return groupWeight > other.groupWeight;
    }
};

#endif // OBJ_TILESETS_H
