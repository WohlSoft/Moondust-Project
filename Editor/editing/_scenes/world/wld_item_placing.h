/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2024 Vitaly Novichkov <admin@wohlnet.ru>
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
#ifndef WLD_ITEM_PLACING_H
#define WLD_ITEM_PLACING_H

#include <QPoint>
#include <QPair>
#include <QVariant>

#include <PGE_File_Formats/wld_filedata.h>

class WldPlacingItems
{
public:
    static WorldTerrainTile terrainSet;
    static WorldScenery sceneSet;
    static WorldPathTile pathSet;
    static WorldLevelTile levelSet;
    static WorldMusicBox musicSet;

    enum PlaceMode
    {
        PMODE_Brush = 0,
        PMODE_Rect,
        PMODE_Circle,
        PMODE_Line,
        PMODE_FloodFill
    };
    static PlaceMode placingMode;

    static bool overwriteMode;

    static int c_offset_x;
    static int c_offset_y;

    static int itemW;
    static int itemH;

    static int gridSz;
    static QPoint gridOffset;

    static QList<QPair<int, QVariant > > flags;
};

typedef QPair<int, QVariant > dataFlag_w;

#endif // ITEM_PLACING_H
