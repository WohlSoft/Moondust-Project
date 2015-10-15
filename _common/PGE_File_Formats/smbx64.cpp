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

#include "smbx64.h"

namespace smbx64Format
{
    const char *uint_vc = "0123456789";
    const int   uint_vc_len = 10;

    bool isDegit(PGEChar c)
    {
        for(int i=0;i<uint_vc_len;i++)
        {
            if(PGEGetChar(c)==uint_vc[i])
                return true;
        }
        return false;
    }

    bool isValid(PGESTRING &s, const char*valid_chars, const int& valid_chars_len)
    {
        if(s.PGESTRINGisEmpty()) return false;
        int i, j;
        for(i=0;i<(signed)s.size();i++)
        {
            bool found=false;
            for(j=0;j<valid_chars_len;j++) {
                if(PGEGetChar(s[i])==valid_chars[j]) { found=true; break; }
            }
            if(!found) return false;
        }
        return true;
    }
}

// /////////////Validators///////////////
//returns TRUE on wrong data

bool SMBX64::uInt(PGESTRING in) // UNSIGNED INT
{
    using namespace smbx64Format;
    return !isValid(in, uint_vc, uint_vc_len);
}

bool SMBX64::sInt(PGESTRING in) // SIGNED INT
{
    using namespace smbx64Format;
    if(in.PGESTRINGisEmpty()) return true;

    if((in.size()==1)&&(!isDegit(in[0])))          return true;
    if((!isDegit(in[0])) && (PGEGetChar(in[0])!='-')) return true;

    for(int i=1; i<(signed)in.size(); i++)
        if(!isDegit(in[i])) return true;

    return false;
}

bool SMBX64::sFloat(PGESTRING &in) // SIGNED FLOAT
{
    using namespace smbx64Format;
    if(in.PGESTRINGisEmpty()) return true;

    if((in.size()==1)&&(!isDegit(in[0])))          return true;
    if((!isDegit(in[0])) && (PGEGetChar(in[0])!='-')&&(PGEGetChar(in[0])!='.')&&(PGEGetChar(in[0])!=',')) return true;

    bool decimal=false;
    bool pow10  =false;
    bool sign   =false;
    for(int i=((PGEGetChar(in[0])=='-')?1:0); i<(signed)in.size(); i++)
    {
        if((!decimal) &&(!pow10))
        {
            if((PGEGetChar(in[i])=='.')||(PGEGetChar(in[i])==','))
            {
                in[i]='.';//replace comma with a dot
                decimal=true;
                if(i==((signed)in.size()-1)) return true;
                continue;
            }
        }
        if(!pow10)
        {
            if((PGEGetChar(in[i])=='E')||(PGEGetChar(in[i])=='e'))
            {
                pow10=true;
                if(i==((signed)in.size()-1)) return true;
                continue;
            }
        }
        else
        {
            if(!sign)
            {
                sign=true;
                if((PGEGetChar(in[i])=='+')||(PGEGetChar(in[i])=='-'))
                {
                    if(i==((signed)in.size()-1)) return true;
                    continue;
                }
            }
        }
        if(!isDegit(in[i])) return true;
    }
    return false;
}

bool SMBX64::qStr(PGESTRING in) // QUOTED STRING
{
    //This is INVERTED validator. If false - good, true - bad.
    #define QStrGOOD false
    #define QStrBAD true
    int i=0;
    for(i=0; i<(signed)in.size();i++)
    {
        if(i==0)
        {
            if(in[i]!='"') return QStrBAD;
        } else if(i==(signed)in.size()-1) {
            if(in[i]!='"') return QStrBAD;
        } else if(in[i]=='"') return QStrBAD;
        else if(in[i]=='"') return QStrBAD;
    }
    if(i==0) return QStrBAD; //This is INVERTED validator. If false - good, true - bad.
    return QStrGOOD;
}

bool SMBX64::wBool(PGESTRING in) //Worded BOOL
{
    return !( (in=="#TRUE#")||(in=="#FALSE#") );
}

bool SMBX64::dBool(PGESTRING in) //Digital BOOL
{
    if((in.size()!=1) || (in.PGESTRINGisEmpty()) )
        return true;
    return !((PGEGetChar(in[0])=='1')||(PGEGetChar(in[0])=='0'));
}

//Convert from string to internal data
bool SMBX64::wBoolR(PGESTRING in)
{
    return ((in=="#TRUE#")?true:false);
}

PGESTRING SMBX64::StrToStr(PGESTRING in)
{
    PGESTRING target = in;
    if(target.PGESTRINGisEmpty())
        return target;
    if(target[0]==PGEChar('\"'))
        target.PGE_RemSRng(0,1);
    if((!target.PGESTRINGisEmpty()) && (target[target.size()-1]==PGEChar('\"')))
        target.PGE_RemSRng(target.size()-1, 1);

    target=PGE_ReplSTR(target, "\"", "\'");//Correct damaged by SMBX line
    return target;
}


//SMBX64 parameter string generators
PGESTRING SMBX64::IntS(long input)
{  return fromNum(input)+"\n"; }

PGESTRING SMBX64::FloatS(float input)
{  return fromNum(input)+"\n"; }

PGESTRING SMBX64::BoolS(bool input)
{  return PGESTRING( (input)?"#TRUE#":"#FALSE#" )+"\n"; }

PGESTRING SMBX64::qStrS(PGESTRING input)
{
    input = PGE_RemSSTR(input, "\n");
    input = PGE_RemSSTR(input, "\r");
    input = PGE_RemSSTR(input, "\t");
    input = PGE_RemSSTR(input, "\"");
    return PGESTRING("\"")+input+PGESTRING("\"\n");
}

PGESTRING SMBX64::qStrS_multiline(PGESTRING input)
{
    input = PGE_RemSSTR(input, "\t");
    input = PGE_RemSSTR(input, "\"");
    return PGESTRING("\"")+input+PGESTRING("\"\n");
}
