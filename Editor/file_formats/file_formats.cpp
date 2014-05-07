/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include "../mainwindow.h"
#include "file_formats.h"

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
void BadFileMsg(MainWindow *window, QString fileName_DATA, int str_count, QString line)
{
    QMessageBox::warning(window, MainWindow::tr("Bad File"),
    MainWindow::tr(QString("Bad file format\nFile: %1\n"
               "Line Number: %3\n"         //Print Line With error
               "Line Data: %2").arg(fileName_DATA.toStdString().c_str(),line)  //Print Bad data string
               .arg(str_count)
               .toStdString().c_str()),
        QMessageBox::Ok);
}
