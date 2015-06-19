/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifdef USE_SDL_MIXER
#undef main
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#undef main
#endif
#include <QFileInfo>
#include <QDir>

#include <common_features/logger.h>
#include <common_features/proxystyle.h>
#include <common_features/app_path.h>
#include <common_features/installer.h>
#include <common_features/themes.h>
#include <common_features/crashhandler.h>
#include <SingleApplication/singleapplication.h>

#include <networking/engine_intproc.h>
#include <audio/sdl_music_player.h>

#include "mainwindow.h"


int main(int argc, char *argv[])
{
    CrashHandler::initCrashHandlers();

    QApplication::addLibraryPath( QFileInfo(argv[0]).dir().path() );
    QApplication *a = new QApplication(argc, argv);
    QStringList args=a->arguments();

#ifdef Q_OS_MAC
    QDir dir(QApplication::applicationDirPath());
    dir.cdUp();
    QApplication::setLibraryPaths(QStringList(dir.absolutePath()));
#endif

    SingleApplication *as = new SingleApplication(args);
    if(!as->shouldContinue())
    {
        QTextStream(stdout) << "Editor already runned!\n";
        return 0;
    }

    a->setStyle(new PGE_ProxyStyle);

    QFont fnt = a->font();

    fnt.setPointSize(PGEDefaultFontSize);
    a->setFont(fnt);

    //Init system paths
    AppPathManager::initAppPath();

    foreach(QString arg, args)
    {
        if(arg=="--install")
        {
            AppPathManager::install();
            AppPathManager::initAppPath();

            Installer::moveFromAppToUser();
            Installer::associateFiles();

            QApplication::quit();
            QApplication::exit();
            delete a;
            delete as;
            return 0;
        }
    }

    //Init themes engine
    Themes::init();

    //Init log writer
    LoadLogSettings();

    WriteToLog(QtDebugMsg, "--> Application started <--");

    #ifdef Q_OS_ANDROID
    SDL_SetMainReady();
    #endif
    #ifdef USE_SDL_MIXER
    //Init SDL Audio subsystem
    if(SDL_Init(SDL_INIT_AUDIO)<0)
    {
        WriteToLog(QtWarningMsg, QString("Error of loading SDL: %1").arg(SDL_GetError()));
    }

    if(Mix_Init(MIX_INIT_FLAC|MIX_INIT_MOD|MIX_INIT_MP3|MIX_INIT_OGG)<0)
    {
        WriteToLog(QtWarningMsg, QString("Error of loading SDL Mixer: %1").arg(Mix_GetError()));
    }
    #endif

    int ret=0;
    //Init Main Window class
    MainWindow *w = new MainWindow;
    if(!w->continueLoad)
    {

        delete w;
        goto QuitFromEditor;
    }

    a->connect( a, SIGNAL(lastWindowClosed()), a, SLOT( quit() ) );
    a->connect( w, SIGNAL( closeEditor()), a, SLOT( quit() ) );
    a->connect( w, SIGNAL( closeEditor()), a, SLOT( closeAllWindows() ) );

    #ifndef Q_OS_ANDROID
    w->show();
    w->setWindowState(w->windowState()|Qt::WindowActive);
    w->raise();
    #else
    w->showFullScreen();
    #endif
    QApplication::setActiveWindow(w);

    //Open files saved by Crashsave (if any)
    CrashHandler::checkCrashsaves();

    //Open files which opened by command line
    w->openFilesByArgs(args);

    //Set acception of external file openings
    w->connect(as, SIGNAL(openFile(QString)), w, SLOT(OpenFile(QString)));

#ifdef Q_OS_WIN
    w->initWindowsThumbnail();
#endif

    //Run main loop
    ret=a->exec();

QuitFromEditor:
        WriteToLog(QtDebugMsg, "Closing interprocess communicator...");
    IntEngine::destroy();
    #ifdef USE_SDL_MIXER
        WriteToLog(QtDebugMsg, "Free music buffer...");
    PGE_MusPlayer::MUS_freeStream();
        WriteToLog(QtDebugMsg, "Free sound buffer...");
    PGE_Sounds::freeBuffer();
        WriteToLog(QtDebugMsg, "Closing audio...");
    Mix_CloseAudio();
        WriteToLog(QtDebugMsg, "Closing SDL...");
    SDL_Quit();
    #endif

    WriteToLog(QtDebugMsg, "--> Application closed <--");
    QApplication::quit();
    QApplication::exit();
    delete a;
    delete as;

    return ret;
}
