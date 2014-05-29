/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include <QtWidgets>
#include "mainwindow.h"
#include <QDebug>

#include <QSharedMemory>
#include <QSystemSemaphore>

#include "common_features/logger.h"

//Regular expressions for File Formats

int main(int argc, char *argv[])
{
    QApplication::addLibraryPath(".");
    QApplication a(argc, argv);

    LoadLogSettings();

    //Check if application is already running//////////////////
    QSystemSemaphore sema("Platformer Game Engine by Wohlstand 457h6329c2h32h744i", 1);
    bool isRunning;

    if(sema.acquire())
    {
        QSharedMemory shmem("Platformer Game Engine by Wohlstand fyhj246h46y46836u");
        shmem.attach();
    }

    QString sendToMem;
    foreach(QString str, a.arguments())
    {
        sendToMem+= str + "|";
    }

    QSharedMemory shmem("Platformer Game Engine by Wohlstand fyhj246h46y46836u");
    if (shmem.attach())
    {
        isRunning = true;
    }
    else
    {
        shmem.create(1);
        isRunning = false;
    }
    sema.release();

    if(isRunning)
    {
        WriteToLog(QtDebugMsg, "--> Application Already running, aborting <--");
        return 0;
    }

    // ////////////////////////////////////////////////////
    WriteToLog(QtDebugMsg, "--> Application started <--");

    MainWindow w;
    w.show();

    w.openFilesByArgs(a.arguments());

    return a.exec();
}
