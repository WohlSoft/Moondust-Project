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

#include "pge_file_lib_sys.h"
#include "file_formats.h"
#include "file_strlist.h"
#include "smbx64.h"
#include "smbx64_macro.h"

LevelData FileFormats::ReadSMBX65by38ALvlFileHeader(PGESTRING filePath)
{
    errorString.clear();
    LevelData FileData;
    FileData = CreateLevelData();

    PGE_FileFormats_misc::TextFileInput inf;
    if(!inf.open(filePath, false))
    {
        FileData.ReadFileValid=false;
        return FileData;
    }
    PGE_FileFormats_misc::FileInfo in_1(filePath);
    FileData.filename = in_1.basename();
    FileData.path = in_1.dirpath();

    inf.seek(0, PGE_FileFormats_misc::TextFileInput::begin);
    SMBX64_FileBegin();
    PGESTRINGList currentLine;

    #define nextLineH() str_count++;line = inf.readLine();

    nextLineH();   //Read first line
    if( !PGE_StartsWith(line, "SMBXFile") ) //File format number
        goto badfile;

    else file_format=toInt(line);
readLineAgain:
    nextLineH();   //Read second Line
    PGE_SPLITSTR(currentLine, line, "|");

    if(currentLine.size()==0)
    {
        if(!inf.eof())
            goto readLineAgain;
    } else {
        if(currentLine[0]=="A")
        {
            if(currentLine.size()>1) { //Number of stars
                if( SMBX64::uInt(currentLine[1]) )
                    goto badfile;
                else FileData.stars=toInt(currentLine[1]);   //Number of stars
            }
            if(currentLine.size()>2) { //Level title (URL Encoded!)
                FileData.LevelName = PGE_URLDEC(currentLine[2]);
            }
        } else goto readLineAgain;
    }

    FileData.CurSection=0;
    FileData.playmusic=0;

    FileData.ReadFileValid=true;

    return FileData;
badfile:
    FileData.ReadFileValid=false;
    FileData.ERROR_info="Invalid file format, detected file SMBX-"+fromNum(file_format)+"format";
    FileData.ERROR_linenum=str_count;
    FileData.ERROR_linedata=line;
    return FileData;
}

LevelData FileFormats::ReadSMBX65by38ALvlFile(PGESTRING RawData, PGESTRING filePath)
{
    return CreateLevelData();
}


PGESTRING WriteSMBX65by38ALvlFile(LevelData FileData, int file_format)
{
    return "";
}
