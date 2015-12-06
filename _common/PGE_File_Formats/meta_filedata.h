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

/*!
 * \brief Position bookmark entry structure
 */
struct Bookmark
{
    PGESTRING bookmarkName; //!< Name of bookmark
    double x;               //!< Bookmarked X position of the camera
    double y;               //!< Bookmarked Y position of the camera
};

#ifdef PGE_EDITOR
/*!
 * \brief Contains backup of helpful techincal data used by PGE Editor
 */
class CrashData
{
public:
    /*!
     * \brief Constructor
     */
    explicit CrashData();
    /*!
     * \brief Copy constructor
     * \param _cd another CrashData object
     */
    CrashData(const CrashData &_cd);
    /*!
     * \brief Copy constructor
     * \param _cd another CrashData object
     */
    CrashData(CrashData &_cd);
    /*!
     * \brief Sets default preferences
     */
    void reset();
    //! Is crash data was used by editor (if false, LVLX writer will not record crash data into file)
    bool used;
    //! Is this level was untitled since crash occopued?
    bool untitled;
    //! Is this level was modified before crash occouped?
    bool modifyed;
    //! Full original file path file which was opened before crash occouped
    PGESTRING fullPath;
    //! Full episode path of file which was opened before crash occouped
    PGESTRING path;
    //! Base file name of file which was opened before crash occouped
    PGESTRING filename;
};
#endif

/*!
 * \brief Contains additional helpful meda-data used by PGE Applications
 */
struct MetaData
{
    //! Array of all position bookmarks presented in the opened file
    PGEVECTOR<Bookmark> bookmarks;

    /* For Editor application only*/
    #ifdef PGE_EDITOR
    //! Crash backup of Editor's special data
    CrashData crash;
    //only for level
    //! (For levels only) If pointer is not null, level contains a script
    ScriptHolder* script;
    #endif

    //! [Used by file parses] Contains a result of file parsing: if true - file was parsed successfully,
    //! if false - parsing was aborted with errors
    bool      ReadFileValid;
    //! [Used by file parses] Contains description of error
    PGESTRING ERROR_info;
    //! [Used by file parses] Contains data of line in the file where error was occopued
    PGESTRING ERROR_linedata;
    //! [Used by file parses] Contains number of line in the file where error was occouped
    int       ERROR_linenum;
};

#endif // META_FILEDATA_H
