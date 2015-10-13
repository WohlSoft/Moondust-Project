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
#include <QMutex>
#include <QMutexLocker>
#else
#include <regex>
#endif

#include "pge_x.h"
#include "file_strlist.h"

namespace PGEExtendedFormat
{
    const char *section_title_valid_chars    = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
    const int   section_title_valid_chars_len= 38;

    const char *heximal_valid_chars    = "0123456789ABCDEFabcdef";
    const int   heximal_valid_chars_len= 22;

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


#ifdef PGE_FILES_QT
PGEFile::PGEFile(QObject *parent)
    : QObject(parent)
#else
PGEFile::PGEFile()
#endif
{
    _lastError = "";
    rawData = "";
}

#ifdef PGE_FILES_QT
PGEFile::PGEFile(PGEFile &pgeFile, QObject *parent)
    : QObject(parent)
#else
PGEFile::PGEFile(PGEFile &pgeFile)
#endif
{
    rawData = pgeFile.rawData;
    rawDataTree = pgeFile.rawDataTree;
    _lastError = pgeFile._lastError;
}

PGEFile::PGEFile(PGESTRING _rawData)
{
    rawData = _rawData;
    _lastError = "";
}

PGESTRING PGEFile::removeQuotes(PGESTRING str)
{
    PGESTRING target = str.remove(0,1);
    target = target.remove(target.size()-1,1);
    return target;
}

void PGEFile::setRawData(PGESTRING _rawData)
{
    rawData = _rawData;
}

bool PGEFile::buildTreeFromRaw()
{
    PGEXSct PGEXsection;

    FileStringList in;
    in.addData( rawData );

    //Read raw data sections
    bool sectionOpened=false;
    while(!in.atEnd())
    {
        PGEXsection.first = in.readLine();
        PGEXsection.second.clear();

        if(PGESTRING(PGEXsection.first).remove(' ').isEmpty()) continue; //Skip empty strings

        sectionOpened=true;
        PGESTRING data;
        while(!in.atEnd())
        {
            data = in.readLine();
            if(data==PGEXsection.first+"_END") {sectionOpened=false; break;} // Close Section
            PGEXsection.second.push_back(data);
        }
        rawDataTree.push_back(PGEXsection);
    }

    if(sectionOpened)
    {
        _lastError=PGESTRING("Section [%1] is not closed").arg(PGEXsection.first);
        return false;
    }

    //Building tree

    for(int z=0; z<(signed)rawDataTree.size(); z++)
    {
        bool valid=true;
        PGEX_Entry subTree = buildTree( rawDataTree[z].second, &valid );
        if(valid)
        {   //Store like subtree
            subTree.type = PGEX_Struct;
            subTree.name = rawDataTree[z].first;
            dataTree.push_back( subTree );
        }
        else
        {   //Store like plain text
            PGEX_Item dataItem;
            dataItem.type = PGEX_PlainText;
            subTree.data.clear();
            subTree.subTree.clear();
            PGEX_Val dataValue;
                dataValue.marker = "PlainText";
                for(int i=0;i<rawDataTree[z].second.size();i++) dataValue.value += rawDataTree[z].second[i]+"\n";
            dataItem.values.push_back(dataValue);
            subTree.name = rawDataTree[z].first;
            subTree.type = PGEX_PlainText;
            subTree.data.push_back(dataItem);
            dataTree.push_back( subTree );
            valid = true;
        }
    }

    return true;
}


PGEFile::PGEX_Entry PGEFile::buildTree(PGESTRINGList &src_data, bool *_valid)
{
    PGEX_Entry entryData;

    bool valid=true;
    for(int q=0; q<src_data.size(); q++)
    {
        if( IsSectionTitle(PGESTRING(src_data[q]).remove(' ') ) )
        {//Build and store subTree
            PGESTRING nameOfTree = PGESTRING(src_data[q]).remove(' ');
            PGESTRINGList rawSubTree;
            q++;
            for(; q<src_data.size() && src_data[q] != nameOfTree+"_END" ;q++)
            {
                rawSubTree.push_back( src_data[q] );
            }
            PGEX_Entry subTree = buildTree( rawSubTree, &valid );
            if(valid)
            {   //Store like subtree
                subTree.name = nameOfTree;
                entryData.subTree.push_back( subTree );
                entryData.type = PGEX_Struct;
            }
            else
            {   //Store like plain text
                subTree.name = nameOfTree;
                subTree.subTree.clear();
                subTree.type = PGEX_PlainText;
                subTree.data.clear();

                PGEX_Item dataItem; PGEX_Val dataValue;
                dataItem.type = PGEX_PlainText;

                dataValue.marker = nameOfTree;
                //foreach(PGESTRING x, rawSubTree) dataValue.value += x+"\n";
                for(int i=0;i<rawSubTree.size();i++) dataValue.value += rawSubTree[i]+"\n";
                dataItem.values.push_back(dataValue);
                subTree.data.push_back(dataItem);
                entryData.subTree.push_back( subTree );
                entryData.type = PGEX_Struct;
                valid = true;
            }

        }
        else
        {
            PGESTRINGList fields = encodeEscape(src_data[q]).split(';');
            PGEX_Item dataItem;
            dataItem.type = PGEX_Struct;
            for(int i=0;i<fields.size();i++)
            {
                if(PGESTRING(fields[i]).remove(' ').isEmpty()) continue;

                //Store data into list
                PGESTRINGList value = fields[i].split(':');

                if(value.size()!=2)
                {
                    valid=false; break;
                }

                PGEX_Val dataValue;
                    dataValue.marker = value[0];
                    dataValue.value = value[1];
                dataItem.values.push_back(dataValue);
            }
            entryData.type = PGEX_Struct;
            entryData.data.push_back(dataItem);
        }
        if(!valid) break;
    }

    if(_valid) *_valid = valid;
    return entryData;
}



PGESTRING PGEFile::lastError()
{
    return _lastError;
}


bool PGEFile::IsSectionTitle(PGESTRING in)
{
    using namespace PGEExtendedFormat;
    return isValid(in, section_title_valid_chars, section_title_valid_chars_len);
}


//validatos
bool PGEFile::IsQStr(PGESTRING in) // QUOTED STRING
{
    using namespace PGEExtendedFormat;
    #ifdef PGE_FILES_QT
    return QRegExp("^\"(?:[^\"\\\\]|\\\\.)*\"$").exactMatch(in);
    #else
    std::regex rex("^\"(?:[^\"\\\\]|\\\\.)*\"$");
    return std::regex_match(in, rex);
    #endif
}

bool PGEFile::IsHex(PGESTRING in) // Heximal string
{
    using namespace PGEExtendedFormat;
    return isValid(in, heximal_valid_chars, heximal_valid_chars_len);
}

bool PGEFile::IsBool(PGESTRING in) // Boolean
{
    if((in.size()!=1) || (in.isEmpty()) )
        return false;
    return ((in[0].toLatin1()=='1')||(in[0].toLatin1()=='0'));
}

bool PGEFile::IsIntU(PGESTRING in) // Unsigned Int
{
    using namespace PGEExtendedFormat;
    return isValid(in, uint_vc, uint_vc_len);
}

bool PGEFile::IsIntS(PGESTRING in) // Signed Int
{
    using namespace PGEExtendedFormat;

    if(in.isEmpty()) return false;

    if((in.size()==1)&&(!isDegit(in[0])))          return false;
    if((!isDegit(in[0])) && (in[0].toLatin1()!='-')) return false;

    for(int i=1; i<in.size(); i++)
        if(!isDegit(in[i])) return false;

    return true;
}

bool PGEFile::IsFloat(PGESTRING &in) // Float Point numeric
{
    using namespace PGEExtendedFormat;

    if(in.isEmpty()) return false;

    if((in.size()==1)&&(!isDegit(in[0])))          return false;
    if((!isDegit(in[0])) && (in[0].toLatin1()!='-')&&(in[0].toLatin1()!='.')&&(in[0].toLatin1()!=',')) return false;

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
                if(i==(in.size()-1)) return false;
                continue;
            }
        }
        if(!pow10)
        {
            if((in[i].toLatin1()=='E')||(in[i].toLatin1()=='e'))
            {
                pow10=true;
                if(i==(in.size()-1)) return false;
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
                    if(i==(in.size()-1)) return false;
                    continue;
                }
            }
        }
        if(!isDegit(in[i])) return false;
    }

    return true;
}

bool PGEFile::IsBoolArray(PGESTRING in) // Boolean array
{
    using namespace PGEExtendedFormat;
    return isValid(in, "01", 2);
}

bool PGEFile::IsIntArray(PGESTRING in) // Boolean array
{
    using namespace PGEExtendedFormat;
    #ifdef PGE_FILES_QT
    return QRegExp("^\\[(\\-?\\d+,?)*\\]$").exactMatch(in);
    #else
    std::regex rex("^\\[(\\-?\\d+,?)*\\]$");
    return std::regex_match(in, rex);
    #endif
}

bool PGEFile::IsStringArray(PGESTRING in) // String array
{
    bool valid=true;
    int i=0, depth=0, comma=0;
    while(i<in.size())
    {
        switch(depth)
        {
        case 0://outside array
                if(in[i]=='[') {depth=1;comma++;}
                else valid=false;
             break;
        case 1://between entries
            switch(comma)
            {
                case 0:
                         if(in[i]==']') depth=0;//Close array
                    else if(in[i]==',') comma++;//Close array
                    else valid=false;
                    break;
                case 1:
                    if(in[i]=='"')     depth=2;//Open value
                    else valid=false;
            }
            break;
        case 2://Inside entry
                 if(in[i]=='"'){ depth=1;comma=0;}    //Close value
            else if(in[i]=='['){ valid=false;}
            else if(in[i]==']'){ valid=false;}
            else if(in[i]==','){ valid=false;}
            break;
        }
        if(!valid) break;//Stop parsing on invalid
        i++;
    }
    return valid;
}


PGESTRINGList PGEFile::X2STRArr(PGESTRING src)
{
    PGESTRINGList strArr;
    src.remove("[").remove("]");
    strArr = src.split(',');
    for(int i=0; i<strArr.size(); i++)
    {
        strArr[i] = X2STR(strArr[i]);
    }

    return strArr;
}

QList<bool > PGEFile::X2BollArr(PGESTRING src)
{
    QList<bool > arr;

    for(int i=0;i<src.size();i++)
        arr.push_back(src[i]=='1');

    return arr;
}

PGELIST<PGESTRINGList > PGEFile::splitDataLine(PGESTRING src_data, bool *valid)
{
    PGELIST<PGESTRINGList > entryData;

    bool wrong=false;

    PGESTRINGList fields = encodeEscape(src_data).split(';');
    for(int i=0;i<fields.size();i++)
    {
        if(PGESTRING(fields[i]).remove(' ').isEmpty()) continue;

        PGESTRINGList value = fields[i].split(':');

        if(value.size()!=2) {wrong=true; break;}
        entryData.push_back(value);
    }

    if(valid) * valid = (!wrong);
    return entryData;
}


PGESTRING PGEFile::IntS(long input)
{
    return fromNum(input);
}

PGESTRING PGEFile::BoolS(bool input)
{
    return PGESTRING((input)?"1":"0");
}

PGESTRING PGEFile::FloatS(double input)
{
    return PGESTRING::number(input, 'g', 10);
}

PGESTRING PGEFile::qStrS(PGESTRING input)
{
    return "\""+escapeStr(input)+"\"";
}

PGESTRING PGEFile::hStrS(PGESTRING input)
{
    return input.toLatin1();
}

PGESTRING PGEFile::strArrayS(PGESTRINGList input)
{
    PGESTRING output;

    if(input.isEmpty()) return PGESTRING("");

    output.append("[");

    for(int i=0; i< input.size(); i++)
    {
        output.append(qStrS(input[i])+(i<input.size()-1?",":""));
    }

    output.append("]");
    return output;
}

PGESTRING PGEFile::intArrayS(QList<int> input)
{
    PGESTRING output;
    if(input.isEmpty()) return PGESTRING("");
    output.append("[");
    for(int i=0; i< input.size(); i++)
    {
        output.append(fromNum(input[i])+(i<input.size()-1?",":""));
    }
    output.append("]");
    return output;
}

PGESTRING PGEFile::BoolArrayS(PGELIST<bool> input)
{
    PGESTRING output;
    for(int i=0;i<input.size(); i++)
    {
        output.append(input[i]?"1":"0");
    }
    return output;
}

PGESTRING PGEFile::encodeEscape(PGESTRING input)
{
    PGESTRING output = input;
    output.replace("\\n", "==n==");
    output.replace("\\\"", "==q==");
    output.replace("\\\\", "==sl==");
    output.replace("\\;", "==Sc==");
    output.replace("\\:", "==Cl==");
    output.replace("\\[", "==Os==");
    output.replace("\\]", "==Cs==");
    output.replace("\\,", "==Cm==");
    output.replace("\\%", "==Pc==");
    return output;
}

PGESTRING PGEFile::decodeEscape(PGESTRING input)
{
    PGESTRING output = input;
    output.replace("==n==", "\\n");
    output.replace("==q==", "\\\"");
    output.replace("==sl==","\\\\");
    output.replace("==Sc==", "\\;");
    output.replace("==Cl==", "\\:");
    output.replace("==Os==", "\\[");
    output.replace("==Cs==", "\\]");
    output.replace("==Cm==", "\\,");
    output.replace("==Pc==", "\\%");
    return output;
}

PGESTRING PGEFile::X2STR(PGESTRING input)
{
    return restoreStr(
                decodeEscape(
                    removeQuotes(input)
                )
            );
}

PGESTRING PGEFile::restoreStr(PGESTRING input)
{
    PGESTRING output = input;
    output.replace("\\n", "\n");
    output.replace("\\\"", "\"");
    output.replace("\\;", ";");
    output.replace("\\:", ":");
    output.replace("\\[", "[");
    output.replace("\\]", "]");
    output.replace("\\,", ",");
    output.replace("\\%", "%");
    output.replace("\\\\", "\\");
    return output;
}

PGESTRING PGEFile::escapeStr(PGESTRING input)
{
    PGESTRING output = input;
    output.replace("\\", "\\\\");
    output.replace("\n", "\\n");
    output.replace("\"", "\\\"");
    output.replace(";", "\\;");
    output.replace(":", "\\:");
    output.replace("[", "\\[");
    output.replace("]", "\\]");
    output.replace(",", "\\,");
    output.replace("%", "\\%");
    return output;
}

PGESTRING PGEFile::value(PGESTRING marker, PGESTRING data)
{
    PGESTRING out;
    out +=marker+":"+data+";";
    return out;
}
