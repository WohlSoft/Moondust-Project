/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef _WIN32
#   include <unistd.h>
#else
#   include <cstdlib>
#   include <cstring>
#   include <windows.h>
#   include <QFileInfo>
#   include <common_features/logger.h>
#endif

#include <QDir>
#include <QString>


#ifndef _WIN32
static inline int symLink(const QString &from, const QString &to)
{
    return symlink(from.toUtf8().data(), to.toUtf8().data());
}

static inline void removeAllLinks(QDir &d)
{
    d.removeRecursively();
}
#endif // !_WIN32


#ifdef _WIN32

static QString GetLastErrorAsString()
{
    //Get the error message, if any.
    DWORD errorMessageID = GetLastError();
    if(errorMessageID == 0)
        return QString(); //No error message has been recorded

    LPWSTR messageBuffer = nullptr;
    size_t size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&messageBuffer, 0, NULL);

    std::wstring message(messageBuffer, size);

    //Free the buffer.
    LocalFree(messageBuffer);

    return QString::fromStdWString(message);
}

#define REPARSE_MOUNTPOINT_HEADER_SIZE  8

typedef struct
{
    DWORD ReparseTag;
    DWORD ReparseDataLength;
    WORD Reserved;
    WORD ReparseTargetLength;
    WORD ReparseTargetMaximumLength;
    WORD Reserved1;
    WCHAR ReparseTarget[MAX_PATH];
} REPARSE_MOUNTPOINT_DATA_BUFFER, *PREPARSE_MOUNTPOINT_DATA_BUFFER;

static HANDLE OpenDirectory(LPCWSTR pszPath, BOOL bReadWrite)
{
    // Obtain restore privilege in case we don't have it
    HANDLE hToken;
    TOKEN_PRIVILEGES tp;
    OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken);
    LookupPrivilegeValue(NULL,
                        (bReadWrite ? SE_RESTORE_NAME : SE_BACKUP_NAME),
                        &tp.Privileges[0].Luid);

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
    CloseHandle(hToken);

    // Open the directory
    DWORD dwAccess = bReadWrite ? (GENERIC_READ | GENERIC_WRITE) : GENERIC_READ;
    HANDLE hDir = CreateFileW(pszPath, dwAccess, 0, NULL, OPEN_EXISTING,
                              FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS, NULL);
    return hDir;
}

#define DIR_ATTR  (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)

static BOOL IsDirectoryJunction(const std::wstring &pszDir)
{
    DWORD dwAttr = GetFileAttributesW(pszDir.c_str());
    if(dwAttr == INVALID_FILE_ATTRIBUTES)
    {
        LogDebug("IsJunction: not exist");
        return FALSE;  // Not exists
    }

    if((dwAttr & DIR_ATTR) != DIR_ATTR)
    {
        LogDebug("IsJunction: not a dir or no reparse point");
        return FALSE;  // Not dir or no reparse point
    }

    HANDLE hDir = OpenDirectory(pszDir.c_str(), FALSE);
    if(hDir == INVALID_HANDLE_VALUE)
    {
        LogDebug("IsJunction: fail to open a directory: " + GetLastErrorAsString());
        return FALSE;  // Failed to open directory
    }

    BYTE buf[MAXIMUM_REPARSE_DATA_BUFFER_SIZE];
    REPARSE_MOUNTPOINT_DATA_BUFFER& ReparseBuffer = (REPARSE_MOUNTPOINT_DATA_BUFFER&)buf;
    DWORD dwRet;
    BOOL br = DeviceIoControl(hDir, FSCTL_GET_REPARSE_POINT, NULL, 0, &ReparseBuffer,
                              MAXIMUM_REPARSE_DATA_BUFFER_SIZE, &dwRet, NULL);
    CloseHandle(hDir);

    return br ? (ReparseBuffer.ReparseTag == IO_REPARSE_TAG_MOUNT_POINT) : FALSE;
}

static inline int symLink(const QString &from, const QString &to)
{
    auto f = from.toStdWString();
    auto t = to.toStdWString();

    QFileInfo frInfo(from);
    BOOL ret = FALSE;
    if(frInfo.isDir())
    {
        // make a directory juction
        CreateDirectoryW(t.c_str(), NULL);
        HANDLE hDir = OpenDirectory(t.c_str(), TRUE);
        if(hDir)
        {
            auto ff = from;
            ff.replace("//", "/");
            ff.replace('/', '\\');
            ff.push_front("\\??\\");
            ff.push_back("\\");
//            LogDebug(QString("Creating a directory junction to '%1'").arg(ff));
            f = ff.toStdWString();

            // Take note that buf and ReparseBuffer occupy the same space
            BYTE buf[sizeof(REPARSE_MOUNTPOINT_DATA_BUFFER) + MAX_PATH * sizeof(WCHAR)];
            REPARSE_MOUNTPOINT_DATA_BUFFER& ReparseBuffer = (REPARSE_MOUNTPOINT_DATA_BUFFER&)buf;

            // Prepare reparse point data
            std::memset(buf, 0, sizeof(buf));
            ReparseBuffer.ReparseTag = IO_REPARSE_TAG_MOUNT_POINT;
            wcsncpy(ReparseBuffer.ReparseTarget, f.c_str(), MAX_PATH);
            ReparseBuffer.ReparseTargetMaximumLength = f.size() * sizeof(WCHAR);
            ReparseBuffer.ReparseTargetLength = (f.size() - 1) * sizeof(WCHAR);
            ReparseBuffer.ReparseDataLength = ReparseBuffer.ReparseTargetLength + 12;

            // Attach reparse point
            DWORD dwRet;
            DeviceIoControl(hDir, FSCTL_SET_REPARSE_POINT, &ReparseBuffer,
                            ReparseBuffer.ReparseDataLength + REPARSE_MOUNTPOINT_HEADER_SIZE,
                            NULL, 0, &dwRet, NULL);
            CloseHandle(hDir);
        }
    }
    else
    {
        ret = CreateHardLinkW(t.c_str(), f.c_str(), NULL);
        if(ret == FALSE)
        {
            // Can't create a hard link? copy file!
            ret = CopyFileW(f.c_str(), t.c_str(), FALSE);
        }
    }

    return (int)ret;
}

static inline void removeAllLinks(QDir &d)
{
    auto entries = d.entryList(QDir::NoDotAndDotDot|QDir::Dirs|QDir::Files);
    bool noErrors = true;

    for(auto &i : entries)
    {
        auto f = d.absolutePath() + "/" + i;
        QFileInfo e(f);
        f.replace('/', '\\');
        auto ew = f.toStdWString();

        if(e.isDir())
        {
            if(IsDirectoryJunction(ew))
            {
//                LogDebug(QString("Remove a directory junction to '%1'").arg(f));
                HANDLE hDir = OpenDirectory(ew.c_str(), TRUE);

                BYTE buf[REPARSE_MOUNTPOINT_HEADER_SIZE];
                REPARSE_MOUNTPOINT_DATA_BUFFER& ReparseBuffer = (REPARSE_MOUNTPOINT_DATA_BUFFER&)buf;
                DWORD dwRet;

                std::memset(buf, 0, sizeof(buf));
                ReparseBuffer.ReparseTag = IO_REPARSE_TAG_MOUNT_POINT;

                if(!DeviceIoControl(hDir, FSCTL_DELETE_REPARSE_POINT, &ReparseBuffer, REPARSE_MOUNTPOINT_HEADER_SIZE, NULL, 0, &dwRet, NULL))
                {
                    LogWarning("Fail to remove a directory junction!");
                    noErrors = false;
                }

                CloseHandle(hDir);
                RemoveDirectoryW(ew.c_str());
            }
            else
            {
                noErrors = false;
                LogDebug(QString("'%1' is not a junction").arg(f));
                RemoveDirectoryW(ew.c_str());
            }
        }
        else
        {
//            LogDebug(QString("Remove a hard link to '%1'").arg(f));
            DeleteFileW(ew.c_str());
        }
    }

    if(noErrors)
        d.removeRecursively();
}

#endif // _WIN32
