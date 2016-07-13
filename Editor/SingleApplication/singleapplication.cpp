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

#include <QtDebug>

#include "singleapplication.h"

void sendIPCMessage(const char* shmem, const char* semaphore, const QString &data)
{
    QSystemSemaphore    t_sema(semaphore, 1);
    QSharedMemory       t_shmem(shmem);
    if(t_shmem.attach())
    {
        QByteArray bytes;
        bytes = data.toUtf8();
        QTextStream(stdout) << "Attempt to send data...\n";
        int attempts = 4;
        while(attempts > 0)
        {
            t_sema.acquire();
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
            t_sema.release();
            attempts--;
            QThread::msleep(50);
        }
    }
}

SingleApplication::SingleApplication(QStringList &args) :
    m_sema("PGE_EditorSemaphore_35y321c9m853n5y9312mc5ly891235y", 1),
    m_shmem("PGE_EditorSharedMemory_35y321c9m853n5y9312mc5ly891235y")
{
    _shouldContinue = false; // By default this is not the main process

    server = nullptr;
    //QString isServerRuns;

    bool isRunning=false;
    m_sema.acquire();//Avoid races
    if(m_shmem.attach()) //Detect shared memory copy
    {
        isRunning = true;
    }
    else
    {
        m_shmem.create(1);
        isRunning = false;
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
    _arguments = args;

    if(isRunning)
    {
        QString str = QString("CMD:showUp");
        for(int i=1; i<_arguments.size(); i++)
        {
            str.append(QString("\n%1").arg(_arguments[i]));
        }

        sendIPCMessage(PGE_EDITOR_SHARED_MEMORY, PGE_EDITOR_SEMAPHORE, str);
    }
    else
    {
        // The attempt was insuccessful, so we continue the program
        _shouldContinue = true;
        server = new LocalServer();
        server->start();
        QObject::connect(server, SIGNAL(showUp()), this, SLOT(slotShowUp()));
        QObject::connect(server, SIGNAL(dataReceived(QString)), this, SLOT(slotOpenFile(QString)));
        QObject::connect(server, SIGNAL(acceptedCommand(QString)), this, SLOT(slotAcceptedCommand(QString)));
        QObject::connect(this,   SIGNAL(stopServer()), server, SLOT(stopServer()));
    }
    m_sema.release();//Free semaphore
}

/**
 * @brief SingleApplication::~SingleApplication
 *  Destructor
 */
SingleApplication::~SingleApplication()
{
    if(_shouldContinue)
    {
        emit stopServer();
        if((server) && (!server->wait(5000)))
        {
            qDebug() << "TERMINATOR RETURNS BACK single application! 8-)";
            server->terminate();
            qDebug() << "Wait for nothing";
            server->wait();
            qDebug() << "Terminated!";
        }
    }
    if(server) delete server;
}

/**
 * @brief SingleApplication::shouldContinue
 *  Weather the program should be terminated
 * @return bool
 */
bool SingleApplication::shouldContinue()
{
    return _shouldContinue;
}

QStringList SingleApplication::arguments()
{
    return _arguments;
}

/**
 * @brief SingleApplication::slotShowUp
 *  Executed when the showUp command is sent to LocalServer
 */
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

