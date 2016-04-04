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
#endif
#ifdef PGE_FILES_USE_MESSAGEBOXES
#include <QMessageBox>
#endif

#include "file_formats.h"
#include "pge_file_lib_globs.h"

bool FileFormats::OpenLevelFile(PGESTRING filePath, LevelData &FileData)
{
    errorString.clear();
    PGE_FileFormats_misc::TextFileInput file;
    PGESTRING firstLine;
    if(!file.open(filePath))
    {
        FileData.ReadFileValid = false;
        FileData.ERROR_info="Can't open file";
        FileData.ERROR_linedata="";
        FileData.ERROR_linenum=-1;
        errorString = FileData.ERROR_info;
        return false;
    }
    firstLine = file.read(8);
    file.close();

    if( PGE_StartsWith( firstLine, "SMBXFile" ) )
    {
        //Read SMBX65-38A LVL File
        if(!ReadSMBX38ALvlFileF( filePath, FileData ))
        {
            errorString = FileData.ERROR_info;
            return false;
        }
    }
    else if( PGE_DetectSMBXFile( firstLine ) )
    {
        //Read SMBX LVL File
        if(!ReadSMBX64LvlFileF( filePath, FileData ))
        {
            errorString = FileData.ERROR_info;
            return false;
        }
    }
    else
    {   //Read PGE LVLX File
        if(!ReadExtendedLvlFileF( filePath, FileData ))
        {
            errorString = FileData.ERROR_info;
            return false;
        }
    }

    if(PGE_FileFormats_misc::TextFileInput::exists(filePath+".meta"))
    {
        #ifdef PGE_EDITOR
        if(FileData.metaData.script)
            FileData.metaData.script.reset();
        #endif
        if( ! ReadNonSMBX64MetaDataF( filePath+".meta", FileData.metaData ) )
            errorString = "Can't open meta-file";
    }

    return true;
}

bool FileFormats::OpenLevelFileHeader(PGESTRING filePath, LevelData& data)
{
    errorString.clear();

    PGE_FileFormats_misc::TextFileInput file;
    PGESTRING firstLine;
    if(!file.open(filePath))
    {
        data.ReadFileValid = false;
        data.ERROR_info="Can't open file";
        data.ERROR_linedata="";
        data.ERROR_linenum=-1;
        return false;
    }
    firstLine = file.readLine();
    file.close();

    if( PGE_StartsWith(firstLine, "SMBXFile") )
    {
        //Read SMBX65-38A LVL File
        return ReadSMBX38ALvlFileHeader( filePath, data );
    }
    else if( PGE_DetectSMBXFile(firstLine) )
    {
        //Read SMBX LVL File
        return ReadSMBX64LvlFileHeader( filePath, data );
    }
    else
    {   //Read PGE LVLX File
        return ReadExtendedLvlFileHeader( filePath, data );
    }
    return false;
}


bool FileFormats::SaveLevelFile(LevelData &FileData, PGESTRING filePath, LevelFileFormat format, unsigned int FormatVersion)
{
    errorString.clear();
    switch(format)
    {
    case LVL_PGEX:
        {
            smbx64CountStars(FileData);
            if(!FileFormats::WriteExtendedLvlFileF(filePath, FileData))
            {
                errorString="Cannot save file "+filePath+".";
                return false;
            }
            return true;
        }
        break;
    case LVL_SMBX64:
        {
            //Apply SMBX64-specific things to entire array
            smbx64LevelPrepare(FileData);

            if(!FileFormats::WriteSMBX64LvlFileF( filePath, FileData, FormatVersion))
            {
                errorString="Cannot save file "+filePath+".";
                return false;
            }

            //save additional meta data
            if(
                (!FileData.metaData.bookmarks.empty())
            #ifdef PGE_EDITOR
                ||((FileData.metaData.script)&&(!FileData.metaData.script->events().isEmpty()))
            #endif
                )
            {
                if(!FileFormats::WriteNonSMBX64MetaDataF(filePath+".meta", FileData.metaData))
                {
                    errorString="Cannot save file "+filePath+".meta.";
                    return false;
                }
            }
            return true;
        }
        break;
    case LVL_SMBX38A:
        {
            if(!FileFormats::WriteSMBX38ALvlFileF(filePath, FileData))
            {
                errorString="Cannot save file "+filePath+".";
                return false;
            }
            return true;
        }
        break;
    }
    errorString = "Unsupported file type";
    return false;
}

bool FileFormats::SaveLevelData(LevelData &FileData, PGESTRING &RawData, LevelFileFormat format, unsigned int FormatVersion)
{
    errorString.clear();
    switch(format)
    {
    case LVL_PGEX:
        {
            smbx64CountStars(FileData);
            WriteExtendedLvlFileRaw(FileData, RawData);
            return true;
        }
        break;
    case LVL_SMBX64:
        {
            smbx64LevelPrepare(FileData);
            WriteSMBX64LvlFileRaw(FileData, RawData, FormatVersion);
            return true;
        }
        break;
    case LVL_SMBX38A:
        {
            FileFormats::WriteSMBX38ALvlFileRaw(FileData, RawData);
            return true;
        }
        break;
    }
    errorString = "Unsupported file type";
    return false;
}



bool FileFormats::OpenWorldFile(PGESTRING filePath, WorldData &data)
{
    errorString.clear();
    PGE_FileFormats_misc::TextFileInput file;

    PGESTRING firstLine;
    if(!file.open(filePath))
    {
        data.ERROR_info="Can't open file";
        data.ERROR_linedata="";
        data.ERROR_linenum=-1;
        data.ReadFileValid = false;
        return false;
    }

    firstLine = file.read(8);
    file.close();

    if( PGE_DetectSMBXFile(firstLine) )
    {
        //Read SMBX WLD File
        if(!ReadSMBX64WldFileF( filePath, data ))
        {
            errorString = data.ERROR_info;
            return false;
        }
    }
    else
    {
        //Read PGE WLDX File
        if(!ReadExtendedWldFileF( filePath, data ))
        {
            errorString = data.ERROR_info;
            return false;
        }
    }

    if( PGE_FileFormats_misc::TextFileInput::exists(filePath+".meta") )
    {
        if( ! ReadNonSMBX64MetaDataF( filePath+".meta", data.metaData ) )
            errorString = "Can't open meta-file";
    }

    return true;
}

bool FileFormats::OpenWorldFileHeader(PGESTRING filePath, WorldData& data)
{
    errorString.clear();
    PGE_FileFormats_misc::TextFileInput file;
    if(!file.open(filePath))
    {
        data.ERROR_info="Can't open file";
        data.ERROR_linedata="";
        data.ERROR_linenum=-1;
        data.ReadFileValid = false;
        return false;
    }
    PGESTRING firstLine;
    firstLine = file.readLine();
    file.close();

    if( PGE_DetectSMBXFile(firstLine) )
    {   //Read SMBX WLD File
        return ReadSMBX64WldFileHeader( filePath, data );
    }
    else
    {   //Read PGE WLDX File
        return ReadExtendedWldFileHeader( filePath, data);
    }
    return data.ReadFileValid;
}

bool FileFormats::SaveWorldFile(WorldData &FileData, PGESTRING filePath, FileFormats::WorldFileFormat format, unsigned int FormatVersion)
{
    errorString.clear();
    switch(format)
    {
    case WLD_PGEX:
        {
            if(!FileFormats::WriteExtendedWldFileF(filePath, FileData))
            {
                errorString="Cannot save file "+filePath+".";
                return false;
            }
            return true;
        }
        break;
    case WLD_SMBX64:
        {
            if(!FileFormats::WriteSMBX64WldFileF( filePath, FileData, FormatVersion))
            {
                errorString="Cannot save file "+filePath+".";
                return false;
            }

            //save additional meta data
            if( !FileData.metaData.bookmarks.empty() )
            {
                if(!FileFormats::WriteNonSMBX64MetaDataF(filePath+".meta", FileData.metaData))
                {
                    errorString="Cannot save file "+filePath+".meta.";
                    return false;
                }
            }
            return true;
        }
        break;
    case WLD_SMBX38A:
        break;
    }
    errorString = "Unsupported file type";
    return false;
}

bool FileFormats::SaveWorldData(WorldData &FileData, PGESTRING &RawData, FileFormats::WorldFileFormat format, unsigned int FormatVersion)
{
    errorString.clear();
    switch(format)
    {
    case WLD_PGEX:
        {
            WriteExtendedWldFileRaw(FileData, RawData);
            return true;
        }
        break;
    case WLD_SMBX64:
        {
            WriteSMBX64WldFileRaw(FileData, RawData, FormatVersion);
            return true;
        }
        break;
    case WLD_SMBX38A:
        break;
    }
    errorString = "Unsupported file type";
    return false;
}


