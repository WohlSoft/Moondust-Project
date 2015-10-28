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

#include "file_formats.h"
#include "file_strlist.h"
#include "save_filedata.h"
#include "smbx64.h"
#include "smbx64_macro.h"

#ifdef PGE_FILES_QT
#include <QFileInfo>
#include <QDir>
#endif


SMBX64_ConfigFile FileFormats::ReadSMBX64ConfigFile(PGESTRING RawData)
{
    errorString.clear();
    SMBX64_File( RawData );

    SMBX64_ConfigFile FileData;

    ///////////////////////////////////////Begin file///////////////////////////////////////
    nextLine(); UIntVar(file_format, line);//File format number

    if(ge(16)) { nextLine(); wBoolVar(FileData.fullScreen, line);}//Full screen mode

    for(int i=0; i<2; i++)
    {
        SMBX64_ConfigPlayer plr;
        nextLine(); UIntVar(plr.controllerType, line);
        nextLine(); UIntVar(plr.k_up, line);
        nextLine(); UIntVar(plr.k_down, line);
        nextLine(); UIntVar(plr.k_left, line);
        nextLine(); UIntVar(plr.k_right, line);
        nextLine(); UIntVar(plr.k_run, line);
        nextLine(); UIntVar(plr.k_jump, line);
        nextLine(); UIntVar(plr.k_drop, line);
        nextLine(); UIntVar(plr.k_pause, line);
        if(ge(19))
        {
            nextLine(); UIntVar(plr.k_altjump, line);
            nextLine(); UIntVar(plr.k_altrun, line);
        }
        nextLine(); UIntVar(plr.j_run, line);
        nextLine(); UIntVar(plr.j_jump, line);
        nextLine(); UIntVar(plr.j_drop, line);
        nextLine(); UIntVar(plr.j_pause, line);
        if(ge(19))
        {
            nextLine(); UIntVar(plr.j_altjump, line);
            nextLine(); UIntVar(plr.j_altrun, line);
        }
        plr.id=i+1;
        FileData.players.push_back(plr);
    }
    ///////////////////////////////////////EndFile///////////////////////////////////////

    FileData.ReadFileValid=true;
    return FileData;

    badfile:    //If file format is not correct
    if(file_format>0)
        FileData.ERROR_info="Detected file format: SMBX-"+fromNum(file_format)+" is invalid";
    else
        FileData.ERROR_info="It is not an SMBX game settings file";
    FileData.ERROR_linenum=str_count;
    FileData.ERROR_linedata=line;
    FileData.ReadFileValid=false;
    return FileData;
}



PGESTRING FileFormats::WriteSMBX64ConfigFile(SMBX64_ConfigFile &FileData, int file_format)
{
    PGESTRING TextData;
    int i=0;
    //Prevent out of range: 0....64
    if(file_format<0) file_format = 0;
    else
    if(file_format>64) file_format = 64;

    TextData += SMBX64::IntS(file_format);              //Format version
    if(file_format>=16) TextData += SMBX64::BoolS(file_format);              //Format version

    while(FileData.players.size()>2)
    {
        SMBX64_ConfigPlayer plr;
        FileData.players.push_back(plr);
    }

    for(i=0;i<((signed)FileData.players.size()); i++)
    {
        TextData += SMBX64::IntS(FileData.players[i].controllerType);
        TextData += SMBX64::IntS(FileData.players[i].k_up);
        TextData += SMBX64::IntS(FileData.players[i].k_down);
        TextData += SMBX64::IntS(FileData.players[i].k_left);
        TextData += SMBX64::IntS(FileData.players[i].k_right);
        TextData += SMBX64::IntS(FileData.players[i].k_run);
        TextData += SMBX64::IntS(FileData.players[i].k_jump);
        TextData += SMBX64::IntS(FileData.players[i].k_drop);
        TextData += SMBX64::IntS(FileData.players[i].k_pause);
        if(file_format>=19)
        {
            TextData += SMBX64::IntS(FileData.players[i].k_altjump);
            TextData += SMBX64::IntS(FileData.players[i].k_altrun);
        }
        TextData += SMBX64::IntS(FileData.players[i].j_run);
        TextData += SMBX64::IntS(FileData.players[i].j_jump);
        TextData += SMBX64::IntS(FileData.players[i].j_drop);
        TextData += SMBX64::IntS(FileData.players[i].j_pause);
        if(file_format>=19)
        {
            TextData += SMBX64::IntS(FileData.players[i].j_altjump);
            TextData += SMBX64::IntS(FileData.players[i].j_altrun);
        }
    }

    return TextData;
}
