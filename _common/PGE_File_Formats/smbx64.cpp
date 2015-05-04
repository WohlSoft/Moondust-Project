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

#include <QRegExp>
#include "smbx64.h"

#include <QMutex>
#include <QMutexLocker>

namespace smbx64Format
{
    QRegExp isint = QRegExp("^\\d+$");     //Check "Is Numeric"
    QMutex isint_mutex;
    QRegExp issint = QRegExp("^[\\-0]?\\d*$");     //Check "Is signed Numeric"
    QMutex issint_mutex;
    QRegExp issfloat = QRegExp("^[\\-]?(\\d*)?[\\(.|,)]?\\d*[Ee]?[\\-\\+]?\\d*$");     //Check "Is signed Float Numeric"
    QMutex issfloat_mutex;
    QRegExp qstr = QRegExp("^\"(?:[^\\\\]|\\\\.)*\"$");
    QMutex qstr_mutex;
    QRegExp boolwords = QRegExp("^(#TRUE#|#FALSE#)$");
    QMutex boolwords_mutex;
    QRegExp booldeg = QRegExp("^(1|0)$");
    QMutex booldeg_mutex;
}

// /////////////Validators///////////////
//returns TRUE on wrong data

bool SMBX64::Int(QString in) // UNSIGNED INT
{
    using namespace smbx64Format;
    QMutexLocker locker(&isint_mutex);
    return !isint.exactMatch(in);
}

bool SMBX64::sInt(QString in) // SIGNED INT
{
    using namespace smbx64Format;
    QMutexLocker locker(&issint_mutex);
    return !issint.exactMatch(in);
}

bool SMBX64::sFloat(QString in) // SIGNED FLOAT
{
    using namespace smbx64Format;
    QMutexLocker locker(&issfloat_mutex);
    return !issfloat.exactMatch(in);
}

bool SMBX64::qStr(QString in) // QUOTED STRING
{
    using namespace smbx64Format;
    QMutexLocker locker(&qstr_mutex);
    return !qstr.exactMatch(in);
}

bool SMBX64::wBool(QString in) //Worded BOOL
{
    using namespace smbx64Format;
    QMutexLocker locker(&boolwords_mutex);
    return !boolwords.exactMatch(in);
}

bool SMBX64::dBool(QString in) //Digital BOOL
{
    using namespace smbx64Format;
    QMutexLocker locker(&booldeg_mutex);
    return !booldeg.exactMatch(in);
}


//Convert from string to internal data
bool SMBX64::wBoolR(QString in)
{
    using namespace smbx64Format;
    return ((in=="#TRUE#")?true:false);
}

QString SMBX64::StrToStr(QString in)
{
    QString target = in;
    if(target.isEmpty())
        return target;
    if(target[0]==QChar('\"'))
        target.remove(0,1);
    if((!target.isEmpty()) && (target[target.size()-1]==QChar('\"')))
        target.remove(target.size()-1,1);

    target.replace("\"", "\'");//Correct damaged by SMBX line
    return target;
}


//SMBX64 parameter string generators
QString SMBX64::IntS(long input)
{  return QString::number(input)+"\n"; }

QString SMBX64::FloatS(float input)
{  return QString::number(input)+"\n"; }

QString SMBX64::BoolS(bool input)
{  return QString( (input)?"#TRUE#":"#FALSE#" )+"\n"; }

QString SMBX64::qStrS(QString input)
{ return QString("\"%1\"\n").arg(input.remove('\n').remove('\r').remove('\t').remove('\"') ); }

QString SMBX64::qStrS_multiline(QString input)
{ return QString("\"%1\"\n").arg(input.remove('\t').remove('\"') ); }
