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

#include "file_keeper.h"

#include <QFileInfo>
#include <QDir>
#include <QTimer>

#ifndef __WIN32

#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

#else

#include <windows.h>
#include <common_features/logger.h>

static QString errorToString()
{
    // Retrieve the system error message for the last-error code
    LPVOID lpMsgBuf;
    DWORD dw = GetLastError();

    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   nullptr, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, nullptr);

    QString ret = QString::fromWCharArray((LPCWSTR)lpMsgBuf);

    LocalFree(lpMsgBuf);

    return ret;
}

#endif // __WIN32


static QString getRandomString()
{
    const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
    const int randomStringLength = 12; // assuming you want random strings of 12 characters

    QString randomString;
    for(int i = 0; i < randomStringLength; ++i)
    {
        int index = qrand() % possibleCharacters.length();
        QChar nextChar = possibleCharacters.at(index);
        randomString.append(nextChar);
    }
    return randomString;
}

FileKeeper::FileKeeper(const QString &path) :
    m_origPath(path),
    m_backupPath(path + QStringLiteral(".bak"))
{
    if(!QFile::exists(path))
    {
        m_tempPath = m_origPath;
        return; //Do nothing when file is not exists (aka, new-made)
    }

    QFileInfo info = QFileInfo(path);
    QString dirPath = info.absoluteDir().absolutePath() + "/";
    QString baseName = info.baseName();
    QString suffix = info.suffix();

    QString targetPath;
    do
    {
        targetPath = dirPath + baseName + "." + getRandomString() + "." + suffix;
    } while(QFile::exists(targetPath));

    m_tempPath = targetPath;
}

FileKeeper::~FileKeeper()
{
    remove();
}

QString FileKeeper::tempPath()
{
    return m_tempPath;
}

void FileKeeper::remove()
{
    if(m_tempPath == m_origPath)
        return;
    if(!m_tempPath.isEmpty() && QFile::exists(m_tempPath))
    {
        QFile::remove(m_tempPath);
        m_tempPath.clear();
    }
}

void FileKeeper::restore()
{
    if(m_tempPath == m_origPath)
        return;

    if(!m_origPath.isEmpty() && !m_tempPath.isEmpty() && QFile::exists(m_tempPath))
    {
#ifndef __WIN32
        int fd = open(m_tempPath.toUtf8().data(), O_FSYNC | O_APPEND, 0660);
        if(fd)
        {
            fsync(fd);
            close(fd);
        }

        if(QFile::exists(m_origPath))
        {
            if(QFile::exists(m_backupPath))
                QFile::remove(m_backupPath);
            QFile::rename(m_origPath, m_backupPath);
        }
        QFile::rename(m_tempPath, m_origPath);

#else
        std::wstring srcPath = m_tempPath.toStdWString();
        std::wstring dstPath = m_origPath.toStdWString();
        std::wstring backupPath = m_backupPath.toStdWString();
        LogDebug("Attempt to override " + m_origPath + " file with " + m_tempPath + " file and making a backup " + m_backupPath + " backup path...");
        BOOL ret = ReplaceFileW(dstPath.c_str(),
                                srcPath.c_str(),
                                backupPath.c_str(),
                                0, nullptr, nullptr);
        if(ret == FALSE)
        {
            ret = GetLastError();
            LogWarning("Failed to override " + m_origPath + " file with " + m_tempPath + "! ["+ errorToString() + "]");
            if(ret == ERROR_UNABLE_TO_MOVE_REPLACEMENT)
                LogWarning("The replacement file could not be renamed.");
            else if(ret == ERROR_UNABLE_TO_MOVE_REPLACEMENT_2)
                LogWarning("The replacement file could not be moved.");
            else if(ret == ERROR_UNABLE_TO_REMOVE_REPLACED)
                LogWarning("The replaced file could not be deleted.");

            if(QFile::exists(m_origPath))
            {
                if(QFile::exists(m_backupPath))
                    QFile::remove(m_backupPath);
                QFile::rename(m_origPath, m_backupPath);
            }
            QFile::rename(m_tempPath, m_origPath);
        }
#endif

        QString backupPathToRemove = m_backupPath;
        QTimer::singleShot(4000, Qt::CoarseTimer, [backupPathToRemove](){
            if(QFile::exists(backupPathToRemove))
                QFile::remove(backupPathToRemove);
        }); // automatically remove backup file after 4 seconds

        m_tempPath.clear();
    }
}
