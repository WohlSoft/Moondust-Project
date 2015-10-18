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

#ifdef PGE_FILES_QT
#include <QFileInfo>
#include <QDir>
#endif
#ifdef PGE_FILES_USE_MESSAGEBOXES
#include <QMessageBox>
#include <QTranslator>
#endif

#include "file_formats.h"
#include "pge_file_lib_globs.h"

LevelData FileFormats::OpenLevelFile(PGESTRING filePath)
{
    errorString.clear();
    PGE_FileFormats_misc::TextFileInput file;
    LevelData data;
    if(!file.open(filePath))
    {
        data.ReadFileValid = false;
        data.ERROR_info="Can't open file";
        data.ERROR_linedata="";
        data.ERROR_linenum=-1;
        return data;
    }
    file.close();

    PGE_FileFormats_misc::FileInfo in_1(filePath);

    if(in_1.suffix() == "lvl")
        {   //Read SMBX LVL File
            file.open(filePath, false);
            data = ReadSMBX64LvlFile( file.readAll(), filePath );
        }
    else
        {   //Read PGE LVLX File
            file.open(filePath, true);
            data = ReadExtendedLvlFile( file.readAll(), filePath );
        }
    return data;
}

LevelData FileFormats::OpenLevelFileHeader(PGESTRING filePath)
{
    errorString.clear();
    LevelData data;

    PGE_FileFormats_misc::FileInfo in_1(filePath);
    if(in_1.suffix() == "lvl")
        {   //Read SMBX LVL File
            data = ReadSMBX64LvlFileHeader( filePath );
        }
    else
        {   //Read PGE LVLX File
            data = ReadExtendedLvlFileHeader( filePath );
        }
    return data;
}









WorldData FileFormats::OpenWorldFile(PGESTRING filePath)
{
    errorString.clear();
    PGE_FileFormats_misc::TextFileInput file;
    WorldData data;

    if(!file.open(filePath))
    {
        data.ERROR_info="Can't open file";
        data.ERROR_linedata="";
        data.ERROR_linenum=-1;
        data.ReadFileValid = false;
        return data;
    }
    file.close();

    PGE_FileFormats_misc::FileInfo in_1(filePath);

    if(in_1.suffix() == "wld")
        {   //Read SMBX WLD File
            file.open(filePath, false);
            data = ReadSMBX64WldFile( file.readAll(), filePath );
        }
    else
        {   //Read PGE WLDX File
            file.open(filePath, true);
            data = ReadExtendedWldFile( file.readAll(), filePath );
        }

    return data;
}

WorldData FileFormats::OpenWorldFileHeader(PGESTRING filePath)
{
    errorString.clear();
    WorldData data;
    PGE_FileFormats_misc::FileInfo in_1(filePath);

    if(in_1.suffix() == "wld")
        {   //Read SMBX LVL File
            data = ReadSMBX64WldFileHeader( filePath );
        }
    else
        {   //Read PGE LVLX File
            data = ReadExtendedWldFileHeader( filePath );
        }
    return data;
}


