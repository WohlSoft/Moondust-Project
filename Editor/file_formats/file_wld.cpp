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

#include "../mainwindow.h"
#include "wld_filedata.h"

#include "file_formats.h"

/*
if(myString.startsWith("\"") myString.remove(0,1);
if(myString.endsWith("\"") myString.remove(myString.size()-1,1);
*/


//*********************************************************
//****************READ FILE FORMAT*************************
//*********************************************************

//World file Read
WorldData FileFormats::ReadWorldFile(QFile &inf)
{
    //Regs
    QRegExp isint("\\d+");     //Check "Is Numeric"
    //QRegExp boolwords("^(#TRUE#|#FALSE#)$");
    //QRegExp issint("^[\\-0]?\\d*$");     //Check "Is signed Numeric"
    //QRegExp issfloat("^[\\-]?(\\d*)?[\\(.|,)]?\\d*[Ee]?[\\-\\+]?\\d*$");     //Check "Is signed Float Numeric"
    //QRegExp booldeg("^(1|0)$");
    //QRegExp qstr("^\"(?:[^\"\\\\]|\\\\.)*\"$");
    //QString Quotes1 = "^\"(?:[^\"\\\\]|\\\\.)*\"$";
    //QString Quotes2 = "^(?:[^\"\\\\]|\\\\.)*$";


    int str_count=0;        //Line Counter
    //int i;                  //counters
    int file_format;        //File format number
    QString line;           //Current Line data
    QTextStream in(&inf);   //Read File

    in.setAutoDetectUnicode(true); //Test Fix for MacOS
    in.setLocale(QLocale::system());   //Test Fix for MacOS
    in.setCodec(QTextCodec::codecForLocale()); //Test Fix for MacOS


    WorldData FileData;

    str_count++;line = in.readLine();   //Read first Line
    if(!isint.exactMatch(line)) //File format number
        goto badfile;
    else file_format=line.toInt();

    QString::number(file_format);//Пустышка

FileData.ReadFileValid=true;

return FileData;
badfile:    //If file format not corrects
BadFileMsg(inf.fileName(), str_count, line);
FileData.ReadFileValid=false;
return FileData;
}

