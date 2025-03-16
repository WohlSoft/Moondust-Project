/*
 * XTConvert - a tool to package asset packs and episodes for TheXTech game engine.
 * Copyright (c) 2024 ds-sloth
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

#ifdef _WIN32
#   include <io.h>
#   include <fileapi.h>
#else
#   include <sys/stat.h>
#endif

#include <QString>

#include "misc.h"


time_t getFileTime(const QFile &f)
{
    time_t fileTime = 0;

#ifdef _WIN32 // Windows
    HANDLE hFile = reinterpret_cast<HANDLE >(_get_osfhandle(f.handle()));
    FILETIME modTime;

    const int64_t UNIX_TIME_START = 0x019DB1DED53E8000;
    const int64_t TICKS_PER_SECOND = 10000000;

    if(GetFileTime(hFile, NULL, NULL, &modTime))
    {
        LARGE_INTEGER li;
        li.LowPart  = modTime.dwLowDateTime;
        li.HighPart = modTime.dwHighDateTime;
        fileTime = (li.QuadPart - UNIX_TIME_START) / TICKS_PER_SECOND;
    }

#else // UNIX-like
    struct stat attr;
    QString fName = f.fileName();
    stat(fName.toStdString().c_str(), &attr);
    fileTime = attr.st_mtime;
#endif

    return fileTime;
}
