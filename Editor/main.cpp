/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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
        WriteToLog(QtDebugMsg, "--> Application already running, aborting <--");
        return 0;
    }

    // ////////////////////////////////////////////////////
    WriteToLog(QtDebugMsg, "--> Application started <--");

    MainWindow w;
    w.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, QSize(qApp->desktop()->width()-100, qApp->desktop()->height()-100), qApp->desktop()->availableGeometry()));
    w.show();
    w.activateWindow();
    w.raise();

    w.openFilesByArgs(a.arguments());

    return a.exec();
}
