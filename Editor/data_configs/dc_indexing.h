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

#ifndef DC_INDEXING_H
#define DC_INDEXING_H

//////////////Indexing objects////////////////
struct blocksIndexes
{
    unsigned long i; //Target array index
    unsigned long ai; //Animator array index
    unsigned int type;//0 - internal GFX, 1 - user defined GFX
};

struct bgoIndexes
{
    unsigned long i; //Target array index
    unsigned long ai; //Animator array index
    unsigned int type;//0 - internal GFX, 1 - user defined GFX
    long smbx64_sp;//smbx64-sort-priority, array sorting priority, need for compatible with SMBX 1.3
};

struct npcIndexes
{
    unsigned long i; //Target array index
    unsigned long ai; //Animator array index
    unsigned long gi; //Target array index by GlobalConfig
    unsigned int type;//0 - internal GFX, 1 - user defined GFX
};

struct wTileIndexes
{
    unsigned long i; //Target array index
    unsigned long ai; //Animator array index
    unsigned int type;//0 - internal GFX, 1 - user defined GFX
};

struct wSceneIndexes
{
    unsigned long i; //Target array index
    unsigned long ai; //Animator array index
    unsigned int type;//0 - internal GFX, 1 - user defined GFX
};

struct wLevelIndexes
{
    unsigned long i; //Target array index
    unsigned long ai; //Animator array index
    unsigned int type;//0 - internal GFX, 1 - user defined GFX
};
struct wPathIndexes
{
    unsigned long i; //Target array index
    unsigned long ai; //Animator array index
    unsigned int type;//0 - internal GFX, 1 - user defined GFX
};


#endif // DC_INDEXING_H
