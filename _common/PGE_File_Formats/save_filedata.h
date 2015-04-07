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

#include <QVector>
#include <QString>
#include <QPair>

typedef QPair<unsigned int, bool > visibleItem; //!< ArrayID, Is Visible
typedef QPair<QString, int > starOnLevel; //!< Level file, section of level

struct saveCharacterState
{
    unsigned int state;
    unsigned long itemID;
    unsigned int mountType;
    unsigned int mountID;
    unsigned int health;
};

struct GamesaveData
{
    bool ReadFileValid;

    int version;

    unsigned int lives;      //!< Number of lives
    unsigned int coins;      //!< Number of coins
    unsigned int points;     //!< Number of points
    unsigned int totalStars; //!< Total stars

    long worldPosX; //!< Last world map position X
    long worldPosY; //!< Last world map position Y

    long last_hub_warp; //!< Last entered/exited warp Array-ID on the HUB-based episodes.

    unsigned int musicID; //!< Current world music ID
    QString musicFile;    //!< Current world music file (custom music)

    bool gameCompleted;   //!< Is episode was completed in last time

    QVector<saveCharacterState > characterStates;

    //Visible state of world map items
    QVector<visibleItem > visibleLevels;
    QVector<visibleItem > visiblePaths;
    QVector<visibleItem > visibleScenery;
    QVector<starOnLevel > gottenStars;

    //editing:
    bool modified;
    bool untitled;
    bool smbx64strict;
    QString filename;
    QString path;
};

#endif // SAVE_FILEDATA_H

