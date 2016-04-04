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

#include "file_formats.h"
#include "file_strlist.h"
#include "save_filedata.h"
#include "smbx64.h"
#include "smbx64_macro.h"

#ifdef PGE_FILES_QT
#include <QFileInfo>
#include <QDir>
#endif

//*********************************************************
//****************READ FILE FORMAT*************************
//*********************************************************
bool FileFormats::ReadSMBX64ConfigFileF(PGESTRING  filePath, SMBX64_ConfigFile &FileData)
{
    PGE_FileFormats_misc::TextFileInput file(filePath, false);
    return ReadSMBX64ConfigFile(file, FileData);
}

bool FileFormats::ReadSMBX64ConfigFileRaw(PGESTRING &rawdata, PGESTRING  filePath,  SMBX64_ConfigFile &FileData)
{
    PGE_FileFormats_misc::RawTextInput file(&rawdata, filePath);
    return ReadSMBX64ConfigFile(file, FileData);
}

SMBX64_ConfigFile FileFormats::ReadSMBX64ConfigFile(PGESTRING RawData)
{
    SMBX64_ConfigFile FileData;
    PGE_FileFormats_misc::RawTextInput file(&RawData, "");
    ReadSMBX64ConfigFile(file, FileData);
    return FileData;
}

//SMBX64_ConfigFile FileFormats::ReadSMBX64ConfigFile(PGESTRING RawData)
bool FileFormats::ReadSMBX64ConfigFile(PGE_FileFormats_misc::TextInput &in, SMBX64_ConfigFile &FileData)
{
    SMBX64_FileBegin();
    errorString.clear();
    //SMBX64_File( RawData );

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
    return true;

    badfile:    //If file format is not correct
    if(file_format>0)
        FileData.ERROR_info="Detected file format: SMBX-"+fromNum(file_format)+" is invalid";
    else
        FileData.ERROR_info="It is not an SMBX game settings file";
    FileData.ERROR_linenum=in.getCurrentLineNumber();
    FileData.ERROR_linedata=line;
    FileData.ReadFileValid=false;
    return false;
}

//*********************************************************
//****************WRITE FILE FORMAT************************
//*********************************************************
bool FileFormats::WriteSMBX64ConfigFileF(PGESTRING filePath, SMBX64_ConfigFile &FileData, int file_format)
{
    PGE_FileFormats_misc::TextFileOutput file;
    if(!file.open(filePath, false, true, PGE_FileFormats_misc::TextOutput::truncate))
        return false;
    return WriteSMBX64ConfigFile(file, FileData, file_format);
}

bool FileFormats::WriteSMBX64ConfigFileRaw(SMBX64_ConfigFile &FileData, PGESTRING &rawdata, int file_format)
{
    PGE_FileFormats_misc::RawTextOutput file;
    if(!file.open(&rawdata, PGE_FileFormats_misc::TextOutput::truncate))
        return false;
    return WriteSMBX64ConfigFile(file, FileData, file_format);
}

PGESTRING FileFormats::WriteSMBX64ConfigFile(SMBX64_ConfigFile &FileData, int file_format)
{
    PGESTRING raw;
    WriteSMBX64ConfigFileRaw(FileData, raw, file_format);
    return raw;
}

bool FileFormats::WriteSMBX64ConfigFile(PGE_FileFormats_misc::TextOutput &out, SMBX64_ConfigFile &FileData, int file_format)
{
    int i=0;
    //Prevent out of range: 0....64
    if(file_format<0) file_format = 0;
    else
    if(file_format>64) file_format = 64;

    out << SMBX64::IntS(file_format);   //Format version
    if(file_format>=16) out << SMBX64::BoolS(FileData.fullScreen);

    while(FileData.players.size()>2)
    {
        SMBX64_ConfigPlayer plr;
        FileData.players.push_back(plr);
    }

    for(i=0;i<((signed)FileData.players.size()); i++)
    {
        out << SMBX64::IntS(FileData.players[i].controllerType);
        out << SMBX64::IntS(FileData.players[i].k_up);
        out << SMBX64::IntS(FileData.players[i].k_down);
        out << SMBX64::IntS(FileData.players[i].k_left);
        out << SMBX64::IntS(FileData.players[i].k_right);
        out << SMBX64::IntS(FileData.players[i].k_run);
        out << SMBX64::IntS(FileData.players[i].k_jump);
        out << SMBX64::IntS(FileData.players[i].k_drop);
        out << SMBX64::IntS(FileData.players[i].k_pause);
        if(file_format>=19)
        {
            out << SMBX64::IntS(FileData.players[i].k_altjump);
            out << SMBX64::IntS(FileData.players[i].k_altrun);
        }
        out << SMBX64::IntS(FileData.players[i].j_run);
        out << SMBX64::IntS(FileData.players[i].j_jump);
        out << SMBX64::IntS(FileData.players[i].j_drop);
        out << SMBX64::IntS(FileData.players[i].j_pause);
        if(file_format>=19)
        {
            out << SMBX64::IntS(FileData.players[i].j_altjump);
            out << SMBX64::IntS(FileData.players[i].j_altrun);
        }
    }
    return true;
}
