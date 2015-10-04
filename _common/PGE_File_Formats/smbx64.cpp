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
#include <QRegExp>
#include <QMutex>
#include <QMutexLocker>
#endif
#include "smbx64.h"

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

    const char *uint_vc = "0123456789";
    const int   uint_vc_len = 10;

    bool isDegit(QChar c)
    {
        for(int i=0;i<uint_vc_len;i++)
        {
            if(c.toLatin1()==uint_vc[i])
                return true;
        }
        return false;
    }

    bool isValid(QString &s, const char*valid_chars, const int& valid_chars_len)
    {
        if(s.isEmpty()) return false;
        int i, j;
        for(i=0;i<s.size();i++)
        {
            bool found=false;
            for(j=0;j<valid_chars_len;j++) {
                if(s[i].toLatin1()==valid_chars[j]) { found=true; break; }
            }
            if(!found) return false;
        }
        return true;
    }
}

// /////////////Validators///////////////
//returns TRUE on wrong data

bool SMBX64::uInt(QString in) // UNSIGNED INT
{
    using namespace smbx64Format;
    return !isValid(in, uint_vc, uint_vc_len);
}

bool SMBX64::sInt(QString in) // SIGNED INT
{
    using namespace smbx64Format;
    if(in.isEmpty()) return true;

    if((in.size()==1)&&(!isDegit(in[0])))          return true;
    if((!isDegit(in[0])) && (in[0].toLatin1()!='-')) return true;

    for(int i=1; i<in.size(); i++)
        if(!isDegit(in[i])) return true;

    return false;
}

bool SMBX64::sFloat(QString &in) // SIGNED FLOAT
{
    using namespace smbx64Format;
    if(in.isEmpty()) return true;

    if((in.size()==1)&&(!isDegit(in[0])))          return true;
    if((!isDegit(in[0])) && (in[0].toLatin1()!='-')&&(in[0].toLatin1()!='.')&&(in[0].toLatin1()!=',')) return true;

    bool decimal=false;
    bool pow10  =false;
    bool sign   =false;
    for(int i=((in[0].toLatin1()=='-')?1:0); i<in.size(); i++)
    {
        if((!decimal) &&(!pow10))
        {
            if((in[i].toLatin1()=='.')||(in[i].toLatin1()==','))
            {
                in[i]='.';//replace comma with a dot
                decimal=true;
                if(i==(in.size()-1)) return true;
                continue;
            }
        }
        if(!pow10)
        {
            if((in[i].toLatin1()=='E')||(in[i].toLatin1()=='e'))
            {
                pow10=true;
                if(i==(in.size()-1)) return true;
                continue;
            }
        }
        else
        {
            if(!sign)
            {
                sign=true;
                if((in[i].toLatin1()=='+')||(in[i].toLatin1()=='-'))
                {
                    if(i==(in.size()-1)) return true;
                    continue;
                }
            }
        }
        if(!isDegit(in[i])) return true;
    }
    return false;
}

bool SMBX64::qStr(QString in) // QUOTED STRING
{
    using namespace smbx64Format;
    #ifdef PGE_FILES_QT
    QMutexLocker locker(&qstr_mutex);
    #endif
    return !qstr.exactMatch(in);
}

bool SMBX64::wBool(QString in) //Worded BOOL
{
    return !( (in=="#TRUE#")||(in=="#FALSE#") );
}

bool SMBX64::dBool(QString in) //Digital BOOL
{
    if((in.size()!=1) || (in.isEmpty()) )
        return true;
    return !((in[0].toLatin1()=='1')||(in[0].toLatin1()=='0'));
}

//Convert from string to internal data
bool SMBX64::wBoolR(QString in)
{
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
