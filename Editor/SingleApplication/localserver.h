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

#pragma once
#ifndef LOCALSERVER_H
#define LOCALSERVER_H

#include <QThread>
#include <QHash>
#include <QSystemSemaphore>
#include <QSharedMemory>
#include <QAtomicInteger>
#include "semaphore_winapi.h"

#define PGE_EDITOR_SEMAPHORE     "PGE_EDITOR_SEMAF_wejQEThQetjqetJQEtjQeTJQTYJ"
#define PGE_EDITOR_SHARED_MEMORY "PGE_EDITOR_SHMEM_wejQEThQetjqetJQEtjQeTJQTYJ"

class LocalServer : public QThread
{
    friend class SingleApplication;
    Q_OBJECT
    //! Semaphore, avoids races
    PGE_Semaphore     m_sema;
    //! Shared memory, stable way to avoid concurrent running multiple copies of same application
    QSharedMemory     m_shmem;
    //! Server working state
    QAtomicInteger<bool> m_isWorking;

    enum class IPCCMD {
        Unknown,
        ShowUP,
        ConnectToEngine,
        EngineClosed,
        TestSetup
    };

    //! Available IPC commands
    QHash<int, QString> m_commands;
    /**
     * @brief Initialize map of available commands
     */
    void initCommands();
public:
    LocalServer();
    ~LocalServer();

protected:
    /**
     * @brief run
     *  Initiate the thread.
     */
    void run();
    /**
     * @brief exec
     *  Keeps the thread alive. Waits for incomming connections
     */
    void exec();

signals:
    void dataReceived(QString data);
    void showUp();
    void acceptedCommand(QString cmd);

private slots:
    /**
     * @brief Stops work of the server
     */
    void stopServer();
    /**
     * @brief LocalServer::slotOnData
     *  Executed when data is received
     * @param data
     */
    void slotOnData(QString data);

private:
    /**
     * @brief Command parser
     *  Receives raw data and parses possible internal commands inside
     * @param data received raw data string
     */
    void onCMD(QString data);
};

#endif // LOCALSERVER_H
