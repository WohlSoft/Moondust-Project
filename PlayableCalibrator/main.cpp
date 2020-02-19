/*
 * SMBX64 Playble Character Sprite Calibrator, a free tool for playable srite design
 * This is a part of the Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifdef Q_OS_MACX
#undef Q_OS_MACX // Workaround for AutoMOC
#endif

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

    AppPathManager::initAppPath(argv[0]);

    for(const QString &arg : a.arguments())
    {
        if(arg == "--install")
        {
            AppPathManager::install();
            AppPathManager::initAppPath(argv[0]);

            QApplication::quit();
            QApplication::exit();
            return 0;
        } else if(arg == "--version") {
            std::cout << V_INTERNAL_NAME " " V_FILE_VERSION << V_FILE_RELEASE "-" V_BUILD_VER << "-" << V_BUILD_BRANCH << std::endl;
            QApplication::quit();
            QApplication::exit();
            return 0;
        }
    }

    CalibrationMain w;

    w.setWindowFlags(w.windowFlags() & ~(Qt::WindowMaximizeButtonHint));
    {
        QList<QScreen*> screens = QGuiApplication::screens();
        if(!screens.isEmpty())
        {
            auto rect = QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, w.size(), screens[0]->availableGeometry());
            w.setGeometry(rect);
        }
    }

    if(!w.m_wasCanceled)
        w.show();
    else
    {
        w.close();
        a.quit();
        return 0;
    }

    return a.exec();
}
