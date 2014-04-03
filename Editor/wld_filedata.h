/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#ifndef WLD_FILEDATA_H
#define WLD_FILEDATA_H

//////////////////////World file Data//////////////////////
struct WorldTiles
{
    long x;
    long y;
    unsigned long id;
};

struct WorldScenery
{
    long x;
    long y;
    unsigned long id;
};

struct WorldPaths
{
    long x;
    long y;
    unsigned long id;
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
};

struct WorldMusic
{
    long x;
    long y;
    unsigned long id;
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
    QString autolevel;
    bool noworldmap;
    bool restartlevel;
    unsigned int stars;

    QString author1;
    QString author2;
    QString author3;
    QString author4;
    QString author5;

    QVector<WorldTiles > tiles;
    QVector<WorldScenery > scenery;
    QVector<WorldPaths > paths;
    QVector<WorldLevels > levels;
    QVector<WorldMusic > music;

};

#endif // WLD_FILEDATA_H
