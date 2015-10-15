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

LevelData FileFormats::OpenLevelFile(PGESTRING filePath, bool silent)
{
    errorString.clear();
    PGE_FileFormats_misc::TextFileInput file;
    LevelData data;
    silentMode = silent;
    if(!file.open(filePath))
    {
        //qDebug() << "Failed to open file: " << filePath;
        #ifdef PGE_FILES_USE_MESSAGEBOXES
        if(!silentMode)
        {
            QMessageBox::critical(NULL, QTranslator::tr("File open error"),
                    QTranslator::tr("Can't open the file."), QMessageBox::Ok);
        }
        #endif
        data.ReadFileValid = false;
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









WorldData FileFormats::OpenWorldFile(PGESTRING filePath, bool silent)
{
    errorString.clear();
    PGE_FileFormats_misc::TextFileInput file;
    WorldData data;
    silentMode=silent;

    if(!file.open(filePath))
    {
        #ifdef PGE_FILES_USE_MESSAGEBOXES
        QMessageBox::critical(NULL, QTranslator::tr("File open error"),
                QTranslator::tr("Can't open the file."), QMessageBox::Ok);
        data.ReadFileValid = false;
        #endif
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


