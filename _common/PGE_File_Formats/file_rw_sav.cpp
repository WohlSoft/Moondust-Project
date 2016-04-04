/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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
#else
#include <stdlib.h>
#include <limits.h> /* PATH_MAX */
#endif

#include "file_formats.h"
#include "file_strlist.h"
#include "save_filedata.h"
#include "smbx64.h"
#include "smbx64_macro.h"

//*********************************************************
//****************READ FILE FORMAT*************************
//*********************************************************

bool FileFormats::ReadSMBX64SavFileF(PGESTRING  filePath, GamesaveData &FileData)
{
    PGE_FileFormats_misc::TextFileInput file(filePath, false);
    return ReadSMBX64SavFile(file, FileData);
}

bool FileFormats::ReadSMBX64SavFileRaw(PGESTRING &rawdata, PGESTRING  filePath,  GamesaveData &FileData)
{
    PGE_FileFormats_misc::RawTextInput file(&rawdata, filePath);
    return ReadSMBX64SavFile(file, FileData);
}

bool FileFormats::ReadSMBX64SavFile(PGE_FileFormats_misc::TextInput &in, GamesaveData &FileData)
{
    SMBX64_FileBegin();
    PGESTRING filePath = in.getFilePath();
    errorString.clear();
    //SMBX64_File( RawData );

    int i;                  //counters
    int arrayIdCounter=0;
    //GamesaveData FileData;
    FileData = CreateGameSaveData();

    FileData.untitled = false;

    //Add path data
    if(!IsEmpty(filePath))
    {
        PGE_FileFormats_misc::FileInfo in_1(filePath);
        FileData.filename = in_1.basename();
        FileData.path = in_1.dirpath();
    }

    //Enable strict mode for SMBX LVL file format
    FileData.smbx64strict = true;

    ///////////////////////////////////////Begin file///////////////////////////////////////
    nextLine(); UIntVar(file_format, line);//File format number
    FileData.version = file_format;
    nextLine(); UIntVar(FileData.lives, line); //Number of lives
    nextLine(); UIntVar(FileData.coins, line); //Number of coins
    nextLine(); SIntVar(FileData.worldPosX, line);  //World map pos X
    nextLine(); SIntVar(FileData.worldPosY, line);  //World map pos Y

    for(i=0; i< (ge(56)? 5 : 2) ;i++)
    {
        saveCharState charState;
        charState = CreateSavCharacterState();
        nextLine(); UIntVar(charState.state, line);//Character's power up state
        nextLine(); UIntVar(charState.itemID, line) //ID of item in the slot
        if(ge(10)) { nextLine();UIntVar(charState.mountType, line); } //Type of mount
        nextLine(); UIntVar(charState.mountID, line); //ID of mount
        if(lt(10)) { if(charState.mountID>0) charState.mountType=1; }
        if(ge(56)) { nextLine(); UIntVar(charState.health, line); } //ID of mount
        FileData.characterStates.push_back(charState);
    }

    nextLine(); UIntVar(FileData.musicID, line);//ID of music
    nextLine();
    if(line=="" || in.eof()) goto successful;

    if(ge(56)) { wBoolVar(FileData.gameCompleted, line);}//Game was complited

    arrayIdCounter=1;

    nextLine();
    while((line!="next")&&(!in.eof()))
    {
        visibleItem level;
        level.first=arrayIdCounter;
        level.second=false;
        wBoolVar(level.second, line); //Is level shown

        FileData.visibleLevels.push_back(level);
        arrayIdCounter++;
        nextLine();
    }

    arrayIdCounter=1;
    nextLine();
    while((line!="next")&&(!in.eof()))
    {
        visibleItem level;
        level.first=arrayIdCounter;
        level.second=false;
        wBoolVar(level.second, line); //Is path shown

        FileData.visiblePaths.push_back(level);
        arrayIdCounter++;
        nextLine();
    }

    arrayIdCounter=1;
    nextLine();
    while((line!="next")&&(!in.eof()))
    {
        visibleItem level;
        level.first=arrayIdCounter;
        level.second=false;
        wBoolVar(level.second, line); //Is Scenery shown

        FileData.visibleScenery.push_back(level);
        arrayIdCounter++;
        nextLine();
    }

    if(ge(7))
    {
        nextLine();
        while((line!="next")&&(!IsNULL(line)))
        {
            starOnLevel gottenStar;
            gottenStar.first="";
            gottenStar.second=0;

            strVar(gottenStar.first, line);//Level file
            if(ge(16)) { nextLine(); UIntVar(gottenStar.second, line); } //Section ID

            FileData.gottenStars.push_back(gottenStar);
            nextLine();
        }
    }

    if(ge(21))
    {
        nextLine();
        if(line=="" || in.eof()) goto successful;
        UIntVar(FileData.totalStars, line);//Total Number of stars
    }

    successful:

    ///////////////////////////////////////EndFile///////////////////////////////////////
    FileData.ReadFileValid=true;
    return true;

    badfile:    //If file format is not correct
    if(file_format>0)
        FileData.ERROR_info="Detected file format: SMBX-"+fromNum(file_format)+" is invalid";
    else
        FileData.ERROR_info="It is not an SMBX game save file";
    FileData.ERROR_linenum=in.getCurrentLineNumber();
    FileData.ERROR_linedata=line;
    FileData.ReadFileValid=false;
    return false;
}


