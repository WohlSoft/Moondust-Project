/*
 * SMBX64 Playble Character Sprite Calibrator, a free tool for playable srite design
 * This is a part of the Platformer Game Engine by Wohlstand, a free platform for game making
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

#include <QtWidgets/QApplication>
#include <QDesktopWidget>
#include "calibrationmain.h"
#include <main/app_path.h>
#include <iostream>

#include "version.h"

int main(int argc, char *argv[])
{
    QApplication::addLibraryPath( "." );
    QApplication::addLibraryPath( QFileInfo(QString::fromUtf8(argv[0])).dir().path() );
    QApplication::addLibraryPath( QFileInfo(QString::fromLocal8Bit(argv[0])).dir().path() );

    QApplication a(argc, argv);

    AppPathManager::initAppPath();

    foreach(QString arg, a.arguments())
    {
        if(arg=="--install")
        {
            AppPathManager::install();
            AppPathManager::initAppPath();

            QApplication::quit();
            QApplication::exit();
            return 0;
        } else if(arg=="--version") {
            std::cout << _INTERNAL_NAME " " _FILE_VERSION << _FILE_RELEASE << std::endl;
            QApplication::quit();
            QApplication::exit();
            return 0;
        }
    }

    CalibrationMain w;

    w.setWindowFlags(w.windowFlags() & ~(Qt::WindowMaximizeButtonHint));
    w.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter,
    w.size(), qApp->desktop()->availableGeometry(0)));

    if(!w.wasCanceled)
        w.show();
    else
    {
        w.close();
        a.quit();
        return 0;
    }
    
    return a.exec();
}
