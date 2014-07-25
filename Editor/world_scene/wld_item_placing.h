/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef WLD_ITEM_PLACING_H
#define WLD_ITEM_PLACING_H

#include <QPoint>

#include "../file_formats/wld_filedata.h"
class WldPlacingItems
{
public:
    static WorldTiles TileSet;
    static WorldScenery SceneSet;
    static WorldPaths PathSet;
    static WorldLevels LevelSet;
    static WorldMusic MusicSet;

    static bool fillingMode;

    static int c_offset_x;
    static int c_offset_y;

    static int itemW;
    static int itemH;

    static int gridSz;
    static QPoint gridOffset;

};

#endif // ITEM_PLACING_H
