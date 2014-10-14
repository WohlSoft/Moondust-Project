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

#include "file_formats.h"

#include <QTranslator>

/*
if(myString.startsWith("\"") myString.remove(0,1);
if(myString.endsWith("\"") myString.remove(myString.size()-1,1);
*/

/*
//Regs
QRegExp isint("\\d+");     //Check "Is Numeric"
QRegExp boolwords("^(#TRUE#|#FALSE#)$");
QRegExp issint("^[\\-0]?\\d*$");     //Check "Is signed Numeric"
QRegExp issfloat("^[\\-]?(\\d*)?[\\.]?\\d*[Ee]?[\\-\\+]?\\d*$");     //Check "Is signed Float Numeric"
QRegExp booldeg("^(1|0)$");
QRegExp qstr("^\"(?:[^\"\\\\]|\\\\.)*\"$");
QString Quotes1 = "^\"(?:[^\"\\\\]|\\\\.)*\"$";
QString Quotes2 = "^(?:[^\"\\\\]|\\\\.)*$";
*/

//Errror for file parsing


void FileFormats::BadFileMsg(QString fileName_DATA, int str_count, QString line)
{
    #ifndef PGE_ENGINE
    QMessageBox * box = new QMessageBox();
    box->setWindowTitle( QTranslator::tr("Bad File") );
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
