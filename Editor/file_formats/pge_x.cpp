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

namespace PGEExtendedFormat
{
    QRegExp qstr = QRegExp("^\"(?:[^\"\\\\]|\\\\.)*\"$");
    QRegExp heximal = QRegExp("^[0-9a-fA-F]+$");

    QRegExp boolean = QRegExp("^(1|0)$");

    QRegExp usig_int = QRegExp("\\d+");     //Check "Is Numeric"
    QRegExp sig_int = QRegExp("^[\\-0]?\\d*$");     //Check "Is signed Numeric"

    QRegExp floatptr = QRegExp("^[\\-]?(\\d*)?[\\(.|,)]?\\d*[Ee]?[\\-\\+]?\\d*$");     //Check "Is signed Float Numeric"

    //Arrays
    QRegExp boolArray = QRegExp("^[1|0]+$");
    QRegExp intArray = QRegExp("^\\[(\\-?\\d+,?)*\\]$"); // ^\[(\-?\d+,?)*\]$

}


//validatos
bool PGEFile::IsQStr(QString in) // QUOTED STRING
{
    using namespace PGEExtendedFormat;
    return qstr.exactMatch(in);
}

bool PGEFile::IsHex(QString in) // Heximal string
{
    using namespace PGEExtendedFormat;
    return heximal.exactMatch(in);
}

bool PGEFile::IsBool(QString in) // Boolean
{
    using namespace PGEExtendedFormat;
    return boolean.exactMatch(in);
}

bool PGEFile::IsIntU(QString in) // Unsigned Int
{
    using namespace PGEExtendedFormat;
    return usig_int.exactMatch(in);
}

bool PGEFile::IsIntS(QString in) // Signed Int
{
    using namespace PGEExtendedFormat;
    return sig_int.exactMatch(in);
}

bool PGEFile::IsFloat(QString in) // Float Point numeric
{
    using namespace PGEExtendedFormat;
    return floatptr.exactMatch(in);
}


bool PGEFile::IsBoolArray(QString in) // Boolean array
{
    using namespace PGEExtendedFormat;
    return boolArray.exactMatch(in);
}

bool PGEFile::IsIntArray(QString in) // Boolean array
{
    using namespace PGEExtendedFormat;
    return intArray.exactMatch(in);
}

bool PGEFile::IsStringArray(QString in) // String array
{
    using namespace PGEExtendedFormat;
    bool valid=true;
    int i=0, depth=0, comma=0;
    while(i<=in.size())
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





QList<QStringList > PGEFile::splitDataLine(QString src_data, bool *valid)
{
    QList<QStringList > entryData;

    bool wrong=false;

    QStringList fields = encodeEscape(src_data).split(';');
    for(int i=0;i<fields.size();i++)
    {
        if(QString(fields[i]).remove(' ').isEmpty()) continue;

        QStringList value = fields[i].split(':');

        if(value.size()!=2) {wrong=true; break;}
        entryData.push_back(value);
    }

    if(valid) * valid = (!wrong);
    return entryData;
}



QString PGEFile::IntS(long input)
{
    return QString::number(input);
}

QString PGEFile::BoolS(bool input)
{
    return QString((input)?"1":"0");
}

QString PGEFile::FloatS(double input)
{
    return QString::number(input);
}

QString PGEFile::qStrS(QString input)
{
    return "\""+escapeStr(input)+"\"";
}

QString PGEFile::hStrS(QString input)
{
    return input.toLatin1();
}

QString PGEFile::strArrayS(QStringList input)
{
    QString output;

    if(input.isEmpty()) return QString("");

    output.append("[");

    for(int i=0; i< input.size(); i++)
    {
        output.append(qStrS(input[i])+(i<input.size()-1?",":""));
    }

    output.append("]");
    return output;
}

QString PGEFile::intArrayS(QList<int> input)
{
    QString output;
    if(input.isEmpty()) return QString("");
    output.append("[");
    for(int i=0; i< input.size(); i++)
    {
        output.append(QString::number(input[i])+(i<input.size()-1?",":""));
    }
    output.append("]");
    return output;
}

QString PGEFile::BoolArrayS(QList<bool> input)
{
    QString output;
    foreach(bool b, input)
    {
        output.append(b?"1":"0");
    }
    return output;
}

QString PGEFile::encodeEscape(QString input)
{
    QString output = input;
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

QString PGEFile::decodeEscape(QString input)
{
    QString output = input;
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

QString PGEFile::X2STR(QString input)
{
    return restoreStr(
                decodeEscape(
                    FileFormats::removeQuotes(input)
                )
            );
}

QString PGEFile::restoreStr(QString input)
{
    QString output = input;
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

QString PGEFile::escapeStr(QString input)
{
    QString output = input;
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

QString PGEFile::value(QString marker, QString data)
{
    QString out;
    out +=marker+":"+data+";";
    return out;
}
