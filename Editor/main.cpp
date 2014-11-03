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

#include "mainwindow.h"

#include <QSharedMemory>
#include <QSystemSemaphore>
#include <QDesktopWidget>


#include "common_features/logger.h"
#include "common_features/proxystyle.h"

#include "SingleApplication/singleapplication.h"

#include <iostream>
#include <stdlib.h>

#include "common_features/app_path.h"
#include "common_features/themes.h"

#undef main
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#undef main

QString ApplicationPath;
QString ApplicationPath_x;

namespace PGECrashHandler {
    void crashByFlood(){
        QMessageBox::warning(nullptr, QApplication::tr("Crash"), QApplication::tr("We're sorry, but PGE has crashed. Reason: Out of memory! :(\n"
                                                                                  "To prevent this, try closing other uneccessary programs to free up more memory."));

        std::exit(1);
    }

    void crashByUnhandledException(){
        std::exception_ptr unhandledException = std::current_exception();
        try{
            std::rethrow_exception(unhandledException);
        }
        catch(const std::exception& e){
            std::string s1 = "We're sorry, but PGE has crashed. Reason: ";
            std::string s2 = e.what();
            std::string s3 = " :(\nPlease inform our forum staff so we can try to fix this problem, Thank you\n\nForum link: engine.wohlnet.ru/forum";

            QMessageBox::warning(nullptr, QApplication::tr("Crash"), QApplication::tr((s1 + s2 + s3).c_str()));
            std::cout << e.what() << std::endl;
        }

        std::exit(2);
    }
}

int main(int argc, char *argv[])
{
    std::set_new_handler(PGECrashHandler::crashByFlood);
    std::set_terminate(PGECrashHandler::crashByUnhandledException);

    QApplication::addLibraryPath(".");


    QApplication *a = new QApplication(argc, argv);



    SingleApplication *as = new SingleApplication(argc, argv);
    if(!as->shouldContinue())
    {
        std::cout << "Editor already runned!\n";
        return 0;
    }





    ApplicationPath = QApplication::applicationDirPath();
    ApplicationPath_x = QApplication::applicationDirPath();

    #ifdef __APPLE__
    //Application path relative bundle folder of application
    QString osX_bundle = QApplication::applicationName()+".app/Contents/MacOS";
    if(ApplicationPath.endsWith(osX_bundle, Qt::CaseInsensitive))
        ApplicationPath.remove(ApplicationPath.length()-osX_bundle.length()-1, osX_bundle.length()+1);
    #endif

    /*
    QString osX_bundle = QApplication::applicationName()+".app/Contents/MacOS";
    QString test="/home/vasya/pge/"+osX_bundle;
    qDebug() << test << " <- before";
    if(test.endsWith(osX_bundle, Qt::CaseInsensitive))
        test.remove(test.length()-osX_bundle.length()-1, osX_bundle.length()+1);
    qDebug() << test << " <- after";
    */

    Themes::init();

    SDL_Init(SDL_INIT_AUDIO);

    a->setApplicationName("Editor - Platformer Game Engine by Wohlstand");

    LoadLogSettings();

    // ////////////////////////////////////////////////////
    a->setStyle(new PGE_ProxyStyle);
    WriteToLog(QtDebugMsg, "--> Application started <--");

    int ret=0;
    QRect screenSize;

    MainWindow *w = new MainWindow;
    if(!w->continueLoad)
    {
        delete w;
        goto QuitFromEditor;
    }

    screenSize = qApp->desktop()->availableGeometry(qApp->desktop()->primaryScreen());
    w->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter,
                                       QSize(screenSize.width()-100,\
                                             screenSize.height()-100), screenSize));

    a->connect( a, SIGNAL(lastWindowClosed()), a, SLOT( quit() ) );
    a->connect( w, SIGNAL( closeEditor()), a, SLOT( quit() ) );
    a->connect( w, SIGNAL( closeEditor()), a, SLOT( closeAllWindows() ) );

    w->showNormal();
    w->activateWindow();
    w->raise();

    w->openFilesByArgs(a->arguments());

    w->connect(as, SIGNAL(openFile(QString)), w, SLOT(OpenFile(QString)));

    ret=a->exec();

QuitFromEditor:
    QApplication::quit();
    QApplication::exit();
    delete a;
    delete as;

    SDL_Quit();
    return ret;
}
