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

#ifndef SAVE_FILEDATA_H
#define SAVE_FILEDATA_H

#include "pge_file_lib_globs.h"

typedef PGEPAIR<unsigned int, bool > visibleItem; //!< ArrayID, Is Visible
typedef PGEPAIR<PGESTRING, int > starOnLevel; //!< Level file, section of level

struct saveCharacterState
{
    int id;
    unsigned int state;
    unsigned long itemID;
    unsigned int mountType;
    unsigned int mountID;
    unsigned int health;
};

struct savePlayerState
{
    int characterID;
};

struct GamesaveData
{
    bool ReadFileValid;

    int version;

    int lives;               //!< Number of lives
    unsigned int coins;      //!< Number of coins
    unsigned int points;     //!< Number of points
    unsigned int totalStars; //!< Total stars

    long worldPosX; //!< Last world map position X
    long worldPosY; //!< Last world map position Y

    long last_hub_warp; //!< Last entered/exited warp Array-ID on the HUB-based episodes.

    unsigned int musicID; //!< Current world music ID
    PGESTRING musicFile;    //!< Current world music file (custom music)

    bool gameCompleted;   //!< Is episode was completed in last time

    PGEVECTOR<saveCharacterState > characterStates;
    PGELIST<int > currentCharacter;

    //Visible state of world map items
    PGEVECTOR<visibleItem > visibleLevels;
    PGEVECTOR<visibleItem > visiblePaths;
    PGEVECTOR<visibleItem > visibleScenery;
    PGEVECTOR<starOnLevel > gottenStars;

    //editing:
    bool modified;
    bool untitled;
    bool smbx64strict;
    PGESTRING filename;
    PGESTRING path;
};

#endif // SAVE_FILEDATA_H

