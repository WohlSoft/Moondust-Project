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
#ifndef META_FILEDATA_H
#define META_FILEDATA_H

#include "pge_file_lib_globs.h"

//Editor's headers shouldn't appears in the engine
#ifdef PGE_EDITOR
#include <script/scriptholder.h>
#endif

struct Bookmark
{
    PGESTRING bookmarkName;
    double x;
    double y;
};

#ifdef PGE_EDITOR
class CrashData
{
public:
    explicit CrashData();
    CrashData(const CrashData &_cd);
    CrashData(CrashData &_cd);
    void reset();
    bool used;
    bool untitled;
    bool modifyed;
    PGESTRING fullPath;
    PGESTRING path;
    PGESTRING filename;
};
#endif

struct MetaData
{
    PGEVECTOR<Bookmark> bookmarks;

    //For Editor application only
    #ifdef PGE_EDITOR
    //Crash backup of special data
    CrashData crash;
    //only for level
    ScriptHolder* script;
    #endif

    bool      ReadFileValid;
    PGESTRING ERROR_info;
    PGESTRING ERROR_linedata;
    int       ERROR_linenum;
};

#endif // META_FILEDATA_H
