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


QString PGEFile::IntS(long input)
{
    return QString::number(input);
}

QString PGEFile::BoolS(bool input)
{
    return QString((input)?"1":"0");
}

QString PGEFile::FloatS(float input)
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
        output.append(qStrS(input[i])+(i<input.size()?",":""));
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
        output.append(QString::number(input[i])+(i<input.size()?",":""));
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

QString PGEFile::escapeStr(QString input)
{
    QString output = input;
    output.replace("\n", "\\n");
    output.replace("\"", "\\\"");
    output.replace("\\", "\\\\");
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
