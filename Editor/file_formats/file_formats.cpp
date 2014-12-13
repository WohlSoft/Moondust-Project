/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QTranslator>
#include <QRegExp>
#include <QFileInfo>
#include <QtDebug>

#include "file_formats.h"
#ifdef PGE_EDITOR
#include <common_features/themes.h>
#endif

FileStringList::FileStringList()
{
    lineID=0;
}

FileStringList::FileStringList(QString fileData)
{
    addData(fileData);
}

FileStringList::~FileStringList()
{
    buffer.clear();
}

void FileStringList::addData(QString fileData)
{
    buffer.clear();
    buffer = fileData.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
    lineID=0;
}

QString FileStringList::readLine()
{
    QString sent;

    if(!isEOF())
    {
        sent = buffer[lineID];
        lineID++;
    }
    return sent;
}

bool FileStringList::isEOF()
{
    return (lineID >= buffer.size());
}

bool FileStringList::atEnd()
{
    return isEOF();
}

LevelData FileFormats::OpenLevelFile(QString filePath)
{
    QFile file(filePath);
    LevelData data;

    if (!file.open(QIODevice::ReadOnly))
    {
        #ifndef PGE_ENGINE
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


WorldData FileFormats::OpenWorldFile(QString filePath)
{
    QFile file(filePath);
    WorldData data;

    if (!file.open(QIODevice::ReadOnly))
    {
        #ifndef PGE_ENGINE
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



void FileFormats::BadFileMsg(QString fileName_DATA, int str_count, QString line)
{
    #ifdef PGE_EDITOR
    QMessageBox * box;
    box= new QMessageBox();
    box->setWindowTitle( QTranslator::tr("Bad File") );
    box->setWindowIcon( Themes::icon(Themes::debugger) );
    box->setText(
                QString( QTranslator::tr("Bad file format\nFile: %1\n").arg(fileName_DATA)  //Print Bad data string
                          +QTranslator::tr("Line Number: %1\n").arg(str_count)         //Print Line With error
                           +QTranslator::tr("Line Data: %1").arg(line))
                );
    box->setStandardButtons(QMessageBox::Ok);
    box->setIcon(QMessageBox::Warning);
    box->exec();
    #else
    Q_UNUSED(fileName_DATA);
    Q_UNUSED(str_count);
    Q_UNUSED(line);
    #endif
}


QString FileFormats::removeQuotes(QString str)
{
    QString target = str.remove(0,1);
    target = target.remove(target.size()-1,1);
    return target;
}
