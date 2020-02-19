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

#include <common_features/logger.h>

#include "singleapplication.h"
#include "semaphore_winapi.h"

#define PGE_SHARED_MEM_KEY "PGE_EditorSharedMemory_35y321c9m853n5y9312mc5ly891235y"
#define PGE_SEMAPHORE_KEY  "PGE_EditorSemaphore_35y321c9m853n5y9312mc5ly891235y"

/**
 * @brief Sends IPC message to shared memory with semaphore protection
 * @param shmem Identificator of target shared memory object
 * @param semaphore Identificator of target semaphore object
 * @param data Data to setnd
 */
static void sendIPCMessage(const char* shmem, const char* semaphore, const QString &data)
{
    PGE_Semaphore       t_sema(semaphore, 1);
    QSharedMemory       t_shmem(shmem);

#ifdef Q_OS_WIN
    //Just keep compatibility with WinAPI
    t_shmem.setNativeKey(PGE_EDITOR_SHARED_MEMORY);
#endif

    if(t_shmem.attach())
    {
        QByteArray bytes;
        bytes = data.toUtf8();
        QTextStream(stdout) << "Attempt to send data...\n";
        int attempts = 4;
        while(attempts > 0)
        {
            {
                PGE_SemaphoreLocker semaLock(&t_sema); Q_UNUSED(semaLock);
                typedef char* pchar;
                typedef int*  pint;
                char* inData = pchar(t_shmem.data());
                QTextStream(stdout) << int(inData[0]) << "\n";
                if(inData[0] == 0)
                {
                    inData[0] = 1;
                    int* size = pint(inData+1);
                    *size = bytes.size();
                    memcpy(inData+1+sizeof(int),
                           bytes.data(),
                           qMin(bytes.size(), int(4095-sizeof(int))));
                    QTextStream(stdout) << "DATA SENT!\n";
                    break;
                }
            }
            attempts--;
            QThread::msleep(50);
        }
        t_shmem.detach();
    }
}

SingleApplication::SingleApplication(QStringList &args) :
    m_sema(PGE_SEMAPHORE_KEY, 1),
    m_shmem(PGE_SHARED_MEM_KEY),
    server(nullptr),
    m_shouldContinue(false) //By default this is not the main process
{
    bool isRunning = false;
    m_sema.acquire();   //Avoid races

    if(!m_shmem.create(1))//Detect shared memory copy
    {
        m_shmem.attach();
        m_shmem.detach();
        if(!m_shmem.create(1))
        {
            isRunning = true;
            if(!m_shmem.attach())
                qWarning() << "Can't re-attach existing shared memory!";
        }
    }

    //Force run second copy of application
    if(args.contains("--force-run", Qt::CaseInsensitive))
    {
        isRunning=false;
        args.removeAll("--force-run");
    }

    if(args.contains("--force-run", Qt::CaseInsensitive))
    {
        args.removeAll("--force-run");
    }
    m_arguments = args;

    if(isRunning)
    {
        QString str = QString("CMD:showUp");
        for(int i = 1; i < m_arguments.size(); i++)
        {
            str.append(QString("\n%1").arg(m_arguments[i]));
        }
        sendIPCMessage(PGE_EDITOR_SHARED_MEMORY, PGE_EDITOR_SEMAPHORE, str);
    }
    else
    {
        // The attempt was insuccessful, so we continue the program
        m_shouldContinue = true;
        server = new LocalServer();
        server->start();
        QObject::connect(server,SIGNAL(showUp()),
                         this,  SLOT(slotShowUp()));
        QObject::connect(server,SIGNAL(dataReceived(QString)),
                         this,  SLOT(slotOpenFile(QString)));
        QObject::connect(server,SIGNAL(acceptedCommand(QString)),
                         this,  SLOT(slotAcceptedCommand(QString)));
        QObject::connect(this,  SIGNAL(stopServer()),
                         server,SLOT(stopServer()));
    }
    m_sema.release();//Free semaphore
}

SingleApplication::~SingleApplication()
{
    if(m_shouldContinue)
    {
        emit stopServer();
        if((server) && (!server->wait(5000)))
        {
            LogDebugQD("TERMINATOR RETURNS BACK single application! 8-)");
            server->terminate();
            LogDebugQD("Wait for nothing");
            server->wait();
            LogDebugQD("Terminated!");
        }
    }
    if(server)
        delete server;
}


bool SingleApplication::shouldContinue()
{
    return m_shouldContinue;
}

QStringList SingleApplication::arguments()
{
    return m_arguments;
}

void SingleApplication::slotShowUp()
{
    emit showUp();
}

void SingleApplication::slotOpenFile(QString path)
{
    emit openFile(path);
}

void SingleApplication::slotAcceptedCommand(QString cmd)
{
    emit acceptedCommand(cmd);
}

