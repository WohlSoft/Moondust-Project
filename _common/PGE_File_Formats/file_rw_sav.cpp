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

#include <QFileInfo>
#include <QDir>

#include "file_formats.h"
#include "file_strlist.h"
#include "save_filedata.h"
#include "smbx64.h"

//*********************************************************
//****************READ FILE FORMAT*************************
//*********************************************************
GamesaveData FileFormats::ReadSMBX64SavFile(QString RawData, QString filePath)
{
    errorString.clear();
    FileStringList in;
    in.addData( RawData );

    int str_count=0;        //Line Counter
    int i;                  //counters
    int file_format=0;      //File format number
    int arrayIdCounter=0;
    QString line;           //Current Line data
    GamesaveData FileData;
    FileData = dummySaveDataArray();

    FileData.untitled = false;

    //Add path data
    if(!filePath.isEmpty())
    {
        QFileInfo in_1(filePath);
        FileData.filename = in_1.baseName();
        FileData.path = in_1.absoluteDir().absolutePath();
    }

    //Enable strict mode for SMBX LVL file format
    FileData.smbx64strict = true;

    ///////////////////////////////////////Begin file///////////////////////////////////////
    str_count++;line = in.readLine();   //Read first line
    if( SMBX64::Int(line) ) //File format number
        goto badfile;

    else file_format=line.toInt();

    FileData.version = file_format;

    str_count++;line = in.readLine();   //Read second Line
    if( SMBX64::Int(line) ) //Number of lives
        goto badfile;
    else FileData.lives=line.toInt();

    str_count++;line = in.readLine();
    if( SMBX64::Int(line) ) //Number of coins
        goto badfile;
    else FileData.coins=line.toInt();

    str_count++;line = in.readLine();
    if( SMBX64::sInt(line) ) //World map pos X
        goto badfile;
    else FileData.worldPosX=line.toInt();

    str_count++;line = in.readLine();
    if( SMBX64::sInt(line) ) //World map pos Y
        goto badfile;
    else FileData.worldPosY=line.toInt();

    for(i=0; i< (file_format>=56? 5 : 2) ;i++)
    {
        saveCharacterState charState;
        charState = dummySavCharacterState();

        str_count++;line = in.readLine();
        if( SMBX64::Int(line) ) //Character's power up state
            goto badfile;
        else charState.state=line.toInt();

        str_count++;line = in.readLine();
        if( SMBX64::Int(line) ) //ID of item in the slot
            goto badfile;
        else charState.itemID=line.toInt();

        if(file_format>=10)
        {
            str_count++;line = in.readLine();
            if( SMBX64::Int(line) ) //Type of mount
                goto badfile;
            else charState.mountType=line.toInt();
        }

        str_count++;line = in.readLine();
        if( SMBX64::Int(line) ) //ID of mount
            goto badfile;
        else charState.mountID=line.toInt();

        if(file_format<10)
        {
            if(charState.mountID>0)
                charState.mountType=1;
        }

        if(file_format>=56)
        {
            str_count++;line = in.readLine();
            if( SMBX64::Int(line) ) //ID of mount
                goto badfile;
            else charState.health=line.toInt();
        }
        FileData.characterStates.push_back(charState);
    }

    str_count++;line = in.readLine();
    if( SMBX64::Int(line) ) //ID of music
        goto badfile;
    else FileData.musicID=line.toInt();

    str_count++;line = in.readLine();
    if(line=="" || in.isEOF()) goto successful;

    if(file_format>=56)
    {
        if( SMBX64::wBool(line) ) //ID of music
        goto badfile;
        else FileData.gameCompleted=SMBX64::wBoolR(line);
    }

    arrayIdCounter=1;

    str_count++;line = in.readLine();
    while((line!="\"next\"")&&(!line.isNull()))
    {
        visibleItem level;
        level.first=arrayIdCounter;
        level.second=false;

        if( SMBX64::wBool(line) ) //Is level shown
            goto badfile;
        else level.second = SMBX64::wBoolR(line);

        FileData.visibleLevels.push_back(level);
        arrayIdCounter++;
        str_count++;line = in.readLine();
    }

    arrayIdCounter=1;
    str_count++;line = in.readLine();
    while((line!="\"next\"")&&(!line.isNull()))
    {
        visibleItem level;
        level.first=arrayIdCounter;
        level.second=false;

        if( SMBX64::wBool(line) ) //Is path shown
            goto badfile;
        else level.second = SMBX64::wBoolR(line);

        FileData.visiblePaths.push_back(level);
        arrayIdCounter++;
        str_count++;line = in.readLine();
    }

    arrayIdCounter=1;
    str_count++;line = in.readLine();
    while((line!="\"next\"")&&(!line.isNull()))
    {
        visibleItem level;
        level.first=arrayIdCounter;
        level.second=false;

        if( SMBX64::wBool(line) ) //Is path shown
            goto badfile;
        else level.second = SMBX64::wBoolR(line);

        FileData.visibleScenery.push_back(level);
        arrayIdCounter++;
        str_count++;line = in.readLine();
    }

    if(file_format>=7)
    {
        str_count++;line = in.readLine();
        while((line!="\"next\"")&&(!line.isNull()))
        {
            starOnLevel gottenStar;
            gottenStar.first="";
            gottenStar.second=0;

            if( SMBX64::qStr(line) ) //Level file
                goto badfile;
            else gottenStar.first= removeQuotes(line);

            if(file_format>=16)
            {
                str_count++;line = in.readLine();
                if( SMBX64::Int(line) ) //Section ID
                    goto badfile;
                else gottenStar.second= line.toInt();
            }

            FileData.gottenStars.push_back(gottenStar);
            str_count++;line = in.readLine();
        }
    }

    if(file_format>=21)
    {
        str_count++;line = in.readLine();
        if(line=="" || in.isEOF()) goto successful;
        if( SMBX64::Int(line) ) //Total Number of stars
            goto badfile;
        else FileData.totalStars = line.toInt();
    }

    successful:

    ///////////////////////////////////////EndFile///////////////////////////////////////
    FileData.ReadFileValid=true;
    return FileData;

    badfile:    //If file format is not correct
    BadFileMsg(filePath+"\nFile format "+QString::number(file_format), str_count, line);
    FileData.ReadFileValid=false;
    return FileData;
}


