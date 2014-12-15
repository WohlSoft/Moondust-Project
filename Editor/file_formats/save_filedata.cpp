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

#include "file_formats.h"

//*********************************************************
//*******************Dummy arrays**************************
//*********************************************************

saveCharacterState FileFormats::dummySavCharacterState()
{
    saveCharacterState newData;

    newData.health      = 0;
    newData.itemID      = 0;
    newData.mountID     = 0;
    newData.mountType   = 0;
    newData.state       = 0;

    return newData;
}


GamesaveData FileFormats::dummySaveDataArray()
{
    GamesaveData newData;

    newData.ReadFileValid = true;

    newData.version = 0;

    newData.lives = 3;
    newData.coins = 0;
    newData.points = 0;
    newData.totalStars = 0;

    newData.musicID = 0;
    newData.musicFile = "";

    newData.worldPosX = 0;
    newData.worldPosY = 0;
    newData.gameCompleted = false;

    newData.modified = false;
    newData.untitled = true;
    newData.smbx64strict = false;
    newData.filename = "";
    newData.path = "";

    return newData;
}
