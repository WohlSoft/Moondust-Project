/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "file_mapper.h"

#include "util.h"

#ifdef __unix__
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#elif _WIN32
#include <windows.h>
#endif

PGE_FileMapper::PGE_FileMapper() :
    #ifdef _WIN32
        m_File(NULL),m_Map(NULL),m_Address(NULL),
    #endif
        data(NULL),size(0)
{}

PGE_FileMapper::PGE_FileMapper(std::string file)
{
    open_file(file);
}

PGE_FileMapper::PGE_FileMapper(const PGE_FileMapper &fm) :
    #ifdef _WIN32
        m_File(NULL),m_Map(NULL),m_Address(NULL),
    #endif
        data(NULL),size(0)
{
    if(fm.data)
    {
        open_file(fm.m_path);
    }
    m_error=fm.m_error;
}

bool PGE_FileMapper::open_file(std::string path)
{
#ifdef __unix__
    struct  stat sb;
    int     m_fd;
    m_fd = open(path.c_str(), O_RDONLY);

    if(m_fd == -1 )
    {
        return false;
    }

    if(fstat(m_fd, &sb) == -1)
    {
        m_error="Failed to take state of file "+path;
        return false;
    }

    if(!S_ISREG (sb.st_mode))
    {
        m_error=path+" is not a file";
        return false;
    }

    data = mmap(0, sb.st_size, PROT_READ, MAP_SHARED, m_fd, 0);

    if(data == MAP_FAILED)
    {
        m_error="Failed to map file "+path;
        return false;
    }

    if(close(m_fd) ==-1 )
    {
        m_error="fd is not closed validely "+path;
        return false;
    }
    size = sb.st_size;
#elif _WIN32
    std::wstring wpath;
    charsets_utils::UTF8Str_To_WStr(wpath, path);
    m_File = CreateFileW(wpath.c_str(), GENERIC_READ, 1, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (m_File == INVALID_HANDLE_VALUE)
    {
        m_error="Failed to open file "+path;
        return false;
    }

    size = GetFileSize(m_File, NULL);

    m_Map = CreateFileMappingW(m_File, NULL, PAGE_READONLY, 0, 0, NULL);
    if( m_Map == NULL )
    {
        CloseHandle(m_File);
        m_error="Failed to map file "+path;
        return false;
    }

    m_Address = MapViewOfFile(m_Map, FILE_MAP_READ, 0, 0, size);
    if(m_Address == NULL)
    {
        CloseHandle(m_Map);
        CloseHandle(m_File);
        m_error="Failed to take map address for a file "+path;
        return false;
    }

    data = m_Address;
#endif

    m_path=path;
    return true;
}

bool PGE_FileMapper::close_file()
{
    if(data)
    {
        #ifdef __unix__
        if(munmap(data, size)==-1)
        {
            m_error="Fail to unmap";
            return false;
        }
        #elif _WIN32
        if (m_Address != NULL)
        {
            try{ UnmapViewOfFile(m_Address); } catch(void */*e*/) {}
            m_Address=NULL;
        }

        if (m_Map != NULL)
        {
            try{ CloseHandle(m_Map); } catch(void */*e*/) {}
            m_Map=NULL;
        }

        if (m_File != INVALID_HANDLE_VALUE)
        {
            try{ CloseHandle(m_File);} catch(void*/*e*/) {}
            m_File = NULL;
        }
        #endif
        m_path.clear();
        data = NULL;
    }
    return true;
}

PGE_FileMapper::~PGE_FileMapper()
{
    if(data)
    {
        close_file();
    }
}

std::string PGE_FileMapper::error()
{
    return m_error;
}
