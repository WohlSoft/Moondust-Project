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

#ifndef FILE_FORMATS_H
#define FILE_FORMATS_H

#include <QRegExp>
#include <QString>

//SMBX64 standard data
namespace SMBX64 {

    QRegExp isint("\\d+");     //Check "Is Numeric"
    QRegExp boolwords("^(#TRUE#|#FALSE#)$");
    QRegExp issint("^[\\-0]?\\d*$");     //Check "Is signed Numeric"
    QRegExp issfloat("^[\\-]?(\\d*)?[\\(.|,)]?\\d*[Ee]?[\\-\\+]?\\d*$");     //Check "Is signed Float Numeric"
    QRegExp booldeg("^(1|0)$");
    QRegExp qstr("^\"(?:[^\"\\\\]|\\\\.)*\"$");

    QString Quotes1 = "^\"(?:[^\"\\\\]|\\\\.)*\"$";
    QString Quotes2 = "^(?:[^\"\\\\]|\\\\.)*$";

    // /////////////Validators///////////////
    //returns TRUE on wrong data
    bool Int(QString in) // UNSIGNED INT
    {  return !isint.exactMatch(in); }

    bool sInt(QString in) // SIGNED INT
    {  return !issint.exactMatch(in); }

    bool sFloat(QString in) // SIGNED FLOAT
    {  return !issfloat.exactMatch(in); }

    bool qStr(QString in) // QUOTED STRING
    {  return !qstr.exactMatch(in); }

    bool wBool(QString in) //Worded BOOL
    {  return !boolwords.exactMatch(in); }

    bool dBool(QString in) //Worded BOOL
    {  return !booldeg.exactMatch(in); }


    //SMBX64 parameter string generators
    QString IntS(long input)
    {  return QString::number(input)+"\n"; }

    QString BoolS(bool input)
    {  return QString( (input)?"#TRUE#":"#FALSE#" )+"\n"; }

    QString qStrS(QString input)
    { return QString("\"%1\"\n").arg(input); }


}

#endif // FILE_FORMATS_H
