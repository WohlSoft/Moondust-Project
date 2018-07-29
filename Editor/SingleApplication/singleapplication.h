/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
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
#ifndef APPLICATION_H
#define APPLICATION_H

#include <QStringList>
#include <QSystemSemaphore>
#include <QSharedMemory>

#include "localserver.h"

/**
 * @brief The Application class handles trivial application initialization procedures
 */
class SingleApplication : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief SingleApplication::SingleApplication
     *  Constructor. Checks and fires up LocalServer or closes the program
     *  if another instance already exists
     * @param argc
     * @param argv
     */
    explicit SingleApplication(QStringList &args);
    ~SingleApplication();

    /**
     * @brief SingleApplication::shouldContinue
     *  Weather the program should be terminated
     * @return bool
     */
    bool shouldContinue();

    QStringList arguments();

public slots:

signals:
    void showUp();
    void stopServer();
    void openFile(QString path);
    void acceptedCommand(QString cmd);

private slots:
    /**
     * @brief SingleApplication::slotShowUp
     *  Executed when the showUp command is sent to LocalServer
     */
    void slotShowUp();

    void slotOpenFile(QString path);
    void slotAcceptedCommand(QString cmd);

private:
    //! Semaphore, avoids races
    QSystemSemaphore m_sema;
    //! Shared memory, stable way to avoid concurrent running multiple copies of same application
    QSharedMemory m_shmem;
    //! Pointer to currently working local server copy
    LocalServer* server;
    //! Recently accepted arguments
    QStringList m_arguments;
    //! Allows contination of application running. If false - another copy of same application already ranned
    bool m_shouldContinue;

};

#endif // APPLICATION_H
