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

#ifndef WLD_FILEDATA_H
#define WLD_FILEDATA_H

#include <QString>
#include <QVector>

//////////////////////World file Data//////////////////////
struct WorldTiles
{
    long x;
    long y;
    unsigned long id;

    //editing
    unsigned int array_id;
    unsigned int index;
};

struct WorldScenery
{
    long x;
    long y;
    unsigned long id;

    //editing
    unsigned int array_id;
    unsigned int index;
};

struct WorldPaths
{
    long x;
    long y;
    unsigned long id;

    //editing
    unsigned int array_id;
    unsigned int index;
};

struct WorldLevels
{
    long x;
    long y;
    unsigned long id;
    QString lvlfile;
    QString title;
    int top_exit;
    int left_exit;
    int bottom_exit;
    int right_exit;
    unsigned long entertowarp;
    bool alwaysVisible;
    bool pathbg;
    bool gamestart;
    long gotox;
    long gotoy;
    bool bigpathbg;

    //editing
    unsigned int array_id;
    unsigned int index;
};

struct WorldMusic
{
    long x;
    long y;
    unsigned long id;
    QString music_file;

    //editing
    unsigned int array_id;
    unsigned int index;
};

struct WorldData
{
    bool ReadFileValid;

    QString EpisodeTitle;
    bool nocharacter1;
    bool nocharacter2;
    bool nocharacter3;
    bool nocharacter4;
    bool nocharacter5;

    QList<bool > nocharacter;

    QString IntroLevel_file;
    bool HubStyledWorld;
    bool restartlevel;

    unsigned int stars;

    QString authors;
    QString author1;
    QString author2;
    QString author3;
    QString author4;
    QString author5;

    QVector<WorldTiles > tiles;
    unsigned int tile_array_id;
    QVector<WorldScenery > scenery;
    unsigned int scene_array_id;
    QVector<WorldPaths > paths;
    unsigned int path_array_id;
    QVector<WorldLevels > levels;
    unsigned int level_array_id;
    QVector<WorldMusic > music;
    unsigned int musicbox_array_id;

    //editing:
    int CurSection;
    bool playmusic;
    int currentMusic;
    bool modified;
    bool untitled;
    bool smbx64strict;
    QString filename;
    QString path;

};

#endif // WLD_FILEDATA_H
