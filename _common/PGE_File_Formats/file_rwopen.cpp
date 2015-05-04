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
#ifdef PGE_FILES_USE_MESSAGEBOXES
#include <QMessageBox>
#endif


#include "file_formats.h"


LevelData FileFormats::OpenLevelFile(QString filePath)
{
    errorString.clear();
    QFile file(filePath);
    LevelData data;

    if (!file.open(QIODevice::ReadOnly))
    {
        //qDebug() << "Failed to open file: " << filePath;
        #ifdef PGE_FILES_USE_MESSAGEBOXES
        QMessageBox::critical(NULL, QTranslator::tr("File open error"),
                QTranslator::tr("Can't open the file."), QMessageBox::Ok);
        #endif
        data.ReadFileValid = false;
        return data;
    }

    QTextStream in(&file);   //Read File

    QFileInfo in_1(filePath);

    if(in_1.suffix().toLower() == "lvl")
        {   //Read SMBX LVL File
            in.setAutoDetectUnicode(true);
            in.setLocale(QLocale::system());
            in.setCodec(QTextCodec::codecForLocale());
            data = ReadSMBX64LvlFile( in.readAll(), filePath );
        }
    else
        {   //Read PGE LVLX File
            in.setCodec("UTF-8");
            data = ReadExtendedLvlFile( in.readAll(), filePath );
        }

    return data;
}

LevelData FileFormats::OpenLevelFileHeader(QString filePath)
{
    errorString.clear();
    LevelData data;
    QFileInfo in_1(filePath);

    if(in_1.suffix().toLower() == "lvl")
        {   //Read SMBX LVL File
            data = ReadSMBX64LvlFileHeader( filePath );
        }
    else
        {   //Read PGE LVLX File
            data = ReadExtendedLvlFileHeader( filePath );
        }
    return data;
}









WorldData FileFormats::OpenWorldFile(QString filePath)
{
    errorString.clear();
    QFile file(filePath);
    WorldData data;

    if (!file.open(QIODevice::ReadOnly))
    {
        #ifdef PGE_FILES_USE_MESSAGEBOXES
        QMessageBox::critical(NULL, QTranslator::tr("File open error"),
                QTranslator::tr("Can't open the file."), QMessageBox::Ok);
        data.ReadFileValid = false;
        #endif
        return data;
    }

    QTextStream in(&file);   //Read File

    QFileInfo in_1(filePath);

    if(in_1.suffix().toLower() == "wld")
        {   //Read SMBX WLD File
            in.setAutoDetectUnicode(true);
            in.setLocale(QLocale::system());
            in.setCodec(QTextCodec::codecForLocale());
            data = ReadSMBX64WldFile( in.readAll(), filePath );
        }
    else
        {   //Read PGE WLDX File
            in.setCodec("UTF-8");
            data = ReadExtendedWldFile( in.readAll(), filePath );
        }

    return data;
}

WorldData FileFormats::OpenWorldFileHeader(QString filePath)
{
    errorString.clear();
    WorldData data;
    QFileInfo in_1(filePath);

    if(in_1.suffix().toLower() == "wld")
        {   //Read SMBX LVL File
            data = ReadSMBX64WldFileHeader( filePath );
        }
    else
        {   //Read PGE LVLX File
            data = ReadExtendedWldFileHeader( filePath );
        }
    return data;
}


