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

#include "pge_file_lib_globs.h"
#include "file_strlist.h"

FileStringList::FileStringList()
{
    lineID=0;
}

FileStringList::FileStringList(PGESTRING fileData)
{
    addData(fileData);
}

FileStringList::~FileStringList()
{
    buffer.clear();
}

void FileStringList::addData(PGESTRING fileData)
{
    buffer.clear();
    #ifdef PGE_FILES_QT
    buffer = fileData.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
    #else
    PGE_SPLITSTR(buffer, fileData, "\n");
    #endif
    lineID=0;
}

PGESTRING FileStringList::readLine()
{
    PGESTRING sent;

    if(!isEOF())
    {
        sent = buffer[lineID];
        lineID++;
    }
    return sent;
}

bool FileStringList::isEOF()
{
    return (lineID >= (signed)buffer.size());
}

bool FileStringList::atEnd()
{
    return isEOF();
}

