/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2024 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer_ext.h>
#endif
#include <QFileInfo>
#include <QDir>
#include <QSettings>
#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC) && !defined(Q_OS_HAIKU) && QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
#include <QLibraryInfo>
#define MOONDUST_QSTYLERPOXY_WORKAROUND_NEEDED
#endif

#ifdef Q_OS_LINUX
#include <QStyleFactory>
#endif

#include <iostream>
#include <locale.h>
#include "version.h"

#include <common_features/logger.h>
#include <common_features/proxystyle.h>
#include <common_features/app_path.h>
#include <common_features/installer.h>
#include <common_features/themes.h>
#include <common_features/crashhandler.h>
#include <common_features/main_window_ptr.h>
#include <common_features/file_keeper.h>
#include <SingleApplication/singleapplication.h>
#include <SingleApplication/editor_application.h>
#include "main_window/global_settings.h"

#include <data_configs/selection_dialog/config_manager.h>

#include <audio/sdl_music_player.h>

#include "mainwindow.h"

#include <FreeImageLite.h>

static bool initied_sdl = false;
static bool initied_fig = false;
static bool restart_requested = false;

static PGE_Application     *app         = nullptr;
static SingleApplication   *appSingle   = nullptr;
static MainWindow          *mWindow     = nullptr;
static QStringList          args;

static void pgeInitSDL()
{
#ifdef Q_OS_ANDROID
    SDL_SetMainReady();
#endif
#ifdef USE_SDL_MIXER
    LogDebugNC("Initializing SDL Audio...");
    if(SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        LogWarning(QString("Error of loading SDL: %1").arg(SDL_GetError()));
        return;
    }
    LogDebugNC("Initializing SDL Mixer X...");
    if(Mix_Init(MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_MP3 | MIX_INIT_OGG | MIX_INIT_MID) < 0)
        LogWarning(QString("Error of loading SDL Mixer: %1").arg(Mix_GetError()));
#endif

    initied_sdl = true;
}

static void pgeInitFreeImage()
{
    LogDebugNC("Initializing of FreeImage...");
    FreeImage_Initialise();
    initied_fig = true;
}

static void pgeEditorQuit()
{
    // Remove all BAK files immediately
    FileKeeper::removeAllBaks();

    if(initied_sdl)
    {
#ifdef USE_SDL_MIXER
        LogDebugNC("Free music buffer...");
        PGE_MusPlayer::closeFile();
        LogDebugNC("Free sound buffer...");
        PGE_SfxPlayer::freeBuffer();
        LogDebugNC("Closing audio...");
        Mix_CloseAudio();
        LogDebugNC("Closing SDL...");
        SDL_Quit();
#endif
    }

    if(initied_fig)
    {
        LogDebugNC("Deinitializing FreeImage...");
        FreeImage_DeInitialise();
    }

    if(MainWinConnect::pMainWin)
    {
        LogDebugNC("Deleting MainWindow...");
        delete MainWinConnect::pMainWin;
    }

    QApplication::quit();
    QApplication::exit();

    GlobalSettings::LvlOpts.deInit();

    if(app)
    {
        LogDebugNC("Deleting Qt-Application...");
        delete app;
    }

    if(appSingle)
    {
        LogDebugNC("Deleting Single-Application...");
        delete appSingle;
    }
}

extern "C"
int main(int argc, char *argv[])
{
    bool argsParseEnd = false;
    bool hasRunConfigArg = false;
    QString runConfigArg;

    bool hasRunCustomAppPathArg = false;
    QString runCustomAppPath;

    CrashHandler::initCrashHandlers();

    //Init system paths
    AppPathManager::initAppPath(argv[0]);

    {
        //Initialize the HighDPI settings
        QSettings advSetup(AppPathManager::settingsFile(), QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        advSetup.setIniCodec("UTF-8");
#endif

        advSetup.beginGroup("extra");
        bool atr_hdpi   = advSetup.value("high-dpi-scaling", true).toBool();
        advSetup.endGroup();
#ifdef Q_OS_MAC
        QApplication::setAttribute(Qt::AA_DontShowIconsInMenus, true);
        //QApplication::setAttribute(Qt::AA_NativeWindows, true);  //Experimentally
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0) && QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        QApplication::setAttribute(Qt::AA_DisableHighDpiScaling, !atr_hdpi);
        QApplication::setAttribute(Qt::AA_EnableHighDpiScaling, atr_hdpi);
        QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, atr_hdpi);
#else
        Q_UNUSED(atr_hdpi);
#endif
    }

    app     = new PGE_Application(argc, argv);
#ifdef _WIN32
    for(int i = 0; i < argc; i++)
        args.push_back(QString::fromUtf8(argv[i]));
#else
    args    = app->arguments();
#endif

    // Workaround: https://doc.qt.io/qt-5/qcoreapplication.html#locale-settings
    setlocale(LC_NUMERIC, "C");

#ifdef Q_OS_MAC
    QDir dir(QApplication::applicationDirPath());
    dir.cdUp();
    QApplication::setLibraryPaths(QStringList(dir.absolutePath()));
#endif

    int ret = 0;
    appSingle = new SingleApplication(args);

    GlobalSettings::LvlOpts.postQApplicationInit();

    if(!appSingle->shouldContinue())
    {
        QTextStream(stdout) << "Editor already running!\n";
        pgeEditorQuit();
        return 0;
    }

#ifdef MOONDUST_QSTYLERPOXY_WORKAROUND_NEEDED
    QVersionNumber qVer = QLibraryInfo::version();
    // NOTE: GTK2 themes crash with the proxy style.
    // This seems like an upstream bug, so there's not much else that can be done.
    if(qVer.majorVersion() == 5 && qVer.minorVersion() == 15 && !QStyleFactory::keys().contains("gtk2"))
        app->setStyle(new PGE_ProxyStyle(app->style()));
#else
    app->setStyle(new PGE_ProxyStyle(app->style()));
#endif

//TODO: Implement a "view" setting like VLC has to change the view theme
//#ifdef Q_OS_LINUX
//    {
//        QStringList availableStyles = QStyleFactory::keys();
//        if(availableStyles.contains("GTK2", Qt::CaseInsensitive))
//            QApplication::setStyle(QStyleFactory::create("GTK2"));
//        else if(availableStyles.contains("Fusion", Qt::CaseInsensitive))
//            QApplication::setStyle(QStyleFactory::create("Fusion"));
//    }
//#endif

    for(auto arg_i = args.begin(); arg_i != args.end(); )
    {
        const QString &arg = *arg_i;

        if(hasRunConfigArg)
        {
            runConfigArg = arg;
#ifdef _WIN32
            runConfigArg.replace('\\', '/');
#endif
            hasRunConfigArg = false;
            arg_i = args.erase(arg_i);
            continue;
        }
        else if(hasRunCustomAppPathArg)
        {
            runCustomAppPath = arg;
#ifdef _WIN32
            runCustomAppPath.replace('\\', '/');
#endif
            hasRunCustomAppPathArg = false;
            arg_i = args.erase(arg_i);
            continue;
        }

        if(arg == "--install")
        {
            AppPathManager::install();
            AppPathManager::initAppPath(argv[0]);

            Installer::moveFromAppToUser();
            Installer::associateFiles();

            pgeEditorQuit();
            return 0;
        }
        else if(!argsParseEnd)
        {
            if(arg == "--version")
            {
                std::cout << V_INTERNAL_NAME " " V_FILE_VERSION << V_FILE_RELEASE "-" V_BUILD_VER << "-" << V_BUILD_BRANCH << std::endl;
                pgeEditorQuit();
                return 0;
            }
            else if(arg == "--config")
            {
                hasRunConfigArg = true;
                arg_i = args.erase(arg_i);
                continue;
            }
            else if(arg == "--app-path")
            {
                hasRunCustomAppPathArg = true;
                arg_i = args.erase(arg_i);
                continue;
            }
            else if(arg == "--")
            {
                argsParseEnd = true;
                arg_i = args.erase(arg_i);
                continue;
            }
        }

        ++arg_i;
    }

    //Init themes engine
    Themes::init();

    //Init log writer
    LoadLogSettings();
    LogDebug("--> Application started <--");

    pgeInitSDL();
    pgeInitFreeImage();

    mWindow = new MainWindow; //Construct MainWindow to initialize language settings

    /******************************Config manager*********************************/
    QString currentConfigDir;
    QString themePack;

    if(!runConfigArg.isEmpty())
    {
        currentConfigDir = runConfigArg;
    }
    else
    {
        ConfigManager cmanager(nullptr);
        currentConfigDir    = cmanager.loadConfigs();
        themePack           = cmanager.m_themePackName;

        //If application started first time or target configuration is not exist
        if(cmanager.m_doAskAgain || currentConfigDir.isEmpty())
        {
            //Ask for configuration
            if(cmanager.hasConfigPacks() && (cmanager.exec() == QDialog::Accepted))
                currentConfigDir = cmanager.m_currentConfigPath;
            else
            {
                LogDebug("<Configuration is not selected, application closed>");
                pgeEditorQuit();
                return 0;
            }
        }

        currentConfigDir = cmanager.m_currentConfigPath;
    }
    /******************************Config manager***END***************************/

    //Init Main Window class
    if(!mWindow->initEverything(currentConfigDir, themePack, runCustomAppPath))
    {
        delete mWindow;
        goto QuitFromEditor;
    }

    app->connect(app,  SIGNAL(lastWindowClosed()), app, SLOT(quit()));
    app->connect(mWindow, SIGNAL(closeEditor()), app, SLOT(quit()));
    app->connect(mWindow, SIGNAL(closeEditor()), app, SLOT(closeAllWindows()));

#ifndef Q_OS_ANDROID
    mWindow->show();
    mWindow->setWindowState(mWindow->windowState() | Qt::WindowActive);
    mWindow->raise();
#else
    mWindow->showFullScreen();
#endif

    QApplication::setActiveWindow(mWindow);

    //Open files saved by Crashsave (if any)
    CrashHandler::checkCrashsaves();

    //Open files which opened by command line
    mWindow->openFilesByArgs(args);
#ifdef __APPLE__
    mWindow->openFilesByArgs(app->getOpenFileChain(), 0);
#endif

    //Set acception of external file openings
    mWindow->connect(appSingle, SIGNAL(openFile(QString)), mWindow, SLOT(OpenFile(QString)));
#ifdef __APPLE__
    mWindow->connect(app, SIGNAL(openFileRequested(QString)), mWindow, SLOT(OpenFile(QString)));
    app->setConnected();
#endif

#ifdef Q_OS_WIN
    mWindow->initWindowsThumbnail();
#endif

    //Show greeting
    mWindow->showWelcomeDialog();

    //Show tip of a day
    mWindow->showTipOfDay();

    //Run main loop
    ret = app->exec();

    restart_requested = mWindow->isAppRestartRequested();

QuitFromEditor:
    pgeEditorQuit();
    LogDebugNC("--> Application closed <--");

    if(restart_requested) // Self-restart
    {
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
        QProcess::startDetached(QString::fromUtf8(argv[0]), {});
#else
        QProcess::startDetached(QString::fromUtf8(argv[0]));
#endif
    }

    return ret;
}
