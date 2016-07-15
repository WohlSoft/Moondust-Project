/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef SEMAPHORE_WINAPI_H
#define SEMAPHORE_WINAPI_H

#ifdef _WIN32
#include <windows.h>
#include <QString>
/**
 * @brief A simple proxy over WinAPI semaphore, semi-compatible with QSystemSemaphore
 */
class WinSemaphore
{
    HANDLE*         m_semaphore;
    std::wstring    m_key;
public:
    WinSemaphore(const QString &key, int initialValue = 0) :
        m_semaphore(nullptr)
    {
        setKey(key, initialValue);
    }

    void setKey(const QString &key, int initialValue = 0)
    {
        if(m_semaphore)
            CloseHandle(m_semaphore);
        m_key = key.toStdWString();
        m_semaphore = (HANDLE *)CreateSemaphoreW(NULL, initialValue, qMax(initialValue, 1), m_key.c_str());
        if (m_semaphore == NULL)
        {
            m_semaphore = (HANDLE*)OpenSemaphoreW(SEMAPHORE_ALL_ACCESS, 0, m_key.c_str());
        }
    }

    QString key() const
    {
        return QString::fromStdWString(m_key);
    }

    ~WinSemaphore()
    {
        CloseHandle(m_semaphore);
    }

    bool acquire()
    {
        DWORD r = WaitForSingleObject(m_semaphore, 3000L);
        return r == WAIT_OBJECT_0;
    }

    bool release(int n=1)
    {
        BOOL r = ReleaseSemaphore(m_semaphore, n, NULL);
        return r != FALSE;
    }

    void close()
    {
        CloseHandle(m_semaphore);
    }
};

typedef WinSemaphore        PGE_Semaphore;
#else
typedef QSystemSemaphore    PGE_Semaphore;
#endif

class PGE_SemaphoreLocker
{
    PGE_Semaphore* m_sema;
public:
    PGE_SemaphoreLocker(PGE_Semaphore* sema):
        m_sema(sema)
    {
        if(m_sema)
        {
            if(!m_sema->acquire())
                m_sema = nullptr;
        }

    }
    ~PGE_SemaphoreLocker()
    {
        if(m_sema)
            m_sema->release();
    }
};

#endif // SEMAPHORE_WINAPI_H
