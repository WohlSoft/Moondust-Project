/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QApplication>
#include <QDesktopWidget>
#include <QDir>
#include <QtDebug>

#include "version.h"

#include "common_features/app_path.h"
#include "common_features/logger.h"
#include "common_features/episode_state.h"
#include "common_features/translator.h"
#include "common_features/crash_handler.h"
#include "common_features/number_limiter.h"
#include "common_features/pge_application.h"

#include <settings/global_settings.h>

#include "data_configs/select_config.h"
#include "data_configs/config_manager.h"

#include "graphics/window.h"

#include <audio/SdlMusPlayer.h>

#include <PGE_File_Formats/file_formats.h>
#include <PGE_File_Formats/smbx64.h>
#include <PGE_File_Formats/pge_x.h>
#include "fontman/font_manager.h"
#include "gui/pge_msgbox.h"
#include "gui/pge_textinputbox.h"

#include "networking/intproc.h"

#include <settings/global_settings.h>
#include <settings/debugger.h>

#include "scenes/scene_level.h"
#include "scenes/scene_world.h"
#include "scenes/scene_loading.h"
#include "scenes/scene_title.h"
#include "scenes/scene_credits.h"
#include "scenes/scene_gameover.h"

#include <QMessageBox>

//#include <cstdlib>
#include <ctime>
#include <iostream>

QString takeStrFromArg(QString &arg, bool &ok)
{
    QStringList tmp;
    QString out;
    tmp = arg.split('=');
    if(tmp.size()>1)
    {
        out = FileFormats::removeQuotes( tmp.last() );
        ok=true;
    } else {
        ok=false;
    }
    return out;
}

int takeIntFromArg(QString &arg, bool &ok)
{
    QStringList tmp;
    QString out;
    tmp = arg.split('=');
    if(tmp.size()>1)
    {
        out = FileFormats::removeQuotes( tmp.last() );
        ok=true;
    } else {
        ok=false;
    }
    return out.toInt(&ok);
}

enum Level_returnTo
{
    RETURN_TO_MAIN_MENU=0,
    RETURN_TO_WORLDMAP,
    RETURN_TO_GAMEOVER_SCREEN,
    RETURN_TO_CREDITS_SCREEN,
    RETURN_TO_EXIT
};
Level_returnTo end_level_jump=RETURN_TO_EXIT;

struct cmdArgs
{
    cmdArgs() :
        debugMode(false),
        testWorld(false),
        testLevel(false),
        test_NumPlayers(1),
        test_Characters{ -1, -1, -1, -1 },
        test_States{ 1, 1, 1, 1 },
        rendererType(GlRenderer::RENDER_AUTO)
    {}
    bool debugMode;
    bool testWorld;
    bool testLevel;
    int  test_NumPlayers;
    int  test_Characters[4];
    int  test_States[4];
    GlRenderer::RenderEngineType rendererType;
} _flags;

/*!
 * \brief Apply test settings to the game state
 * \param state Reference to the current episode state
 */
static void applyTestSettings(EpisodeState &state)
{
    state.numOfPlayers = _flags.test_NumPlayers;
    for(int i=0; i<4; i++)
    {
        if( _flags.test_Characters[i] == -1 )
            continue;
        PlayerState st = state.getPlayerState(i+1);
        st.characterID = _flags.test_Characters[i];
        st.stateID = _flags.test_States[i];
        st._chsetup.id = _flags.test_Characters[i];
        st._chsetup.state = _flags.test_States[i];
        state.setPlayerState(i+1, st);
    }
}

int main(int argc, char *argv[])
{
    CrashHandler::initSigs();

    srand( std::time(NULL) );

    QApplication::addLibraryPath( "." );
    QApplication::addLibraryPath( QFileInfo(QString::fromUtf8(argv[0])).dir().path() );
    QApplication::addLibraryPath( QFileInfo(QString::fromLocal8Bit(argv[0])).dir().path() );

    PGE_Application a(argc, argv);

    #ifdef Q_OS_LINUX
    a.setStyle("GTK");
    #endif

    ///Generating application path

    //Init system paths
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
            std::cout << _INTERNAL_NAME " " _FILE_VERSION << _FILE_RELEASE "-" _BUILD_VER << std::endl;
            QApplication::quit();
            QApplication::exit();
            return 0;
        }
    }

    g_AppSettings.load();
    g_AppSettings.apply();

    //Init log writer
    LoadLogSettings();
    LogDebug("<Application started>");

    PGE_Translator translator;
    translator.init(&a);

    QString             configPath  = "";
    QString             fileToOpen  = "";
    EpisodeState        _game_state;
    PlayEpisodeResult   episode;

    episode.character               = 0;
    episode.savefile                = "save1.savx";
    episode.worldfile               = "";
    g_AppSettings.debugMode         = false; //enable debug mode
    g_AppSettings.interprocessing   = false; //enable interprocessing

    QStringList all_Args = a.arguments();
    for(int pi=1; pi < all_Args.size(); pi++)
    {
        QString &param = all_Args[pi];
        LogDebug( "Argument: ["+param+"]");
        int i=0;
        for(i=0; i<4; i++)
        {
            if(param.startsWith( QString("--p%1c=").arg(i+1) ) )
            {
                int tmp; bool ok=false;
                tmp = takeIntFromArg(param, ok);
                if(ok) _flags.test_Characters[i] = tmp;
                break;
            }
            else if(param.startsWith( QString("--p%1s=").arg(i+1) ) )
            {
                int tmp; bool ok=false;
                tmp = takeIntFromArg(param, ok);
                if(ok)
                {
                    _flags.test_States[i] = tmp;
                    if( _flags.test_Characters[i] == -1 )
                        _flags.test_Characters[i] = 1;
                }
                break;
            }
        }
        if(i<4) //If one of pXc/pYs parameters has been detected
            continue;
        if(param.startsWith("--config="))
        {
            QString tmp; bool ok=false;
            tmp = takeStrFromArg(param, ok);
            if(ok) configPath = tmp;
        }
        else
        if(param.startsWith("--num-players="))
        {
            int tmp; bool ok=false;
            tmp = takeIntFromArg(param, ok);
            if(ok) _flags.test_NumPlayers = tmp;
            //1 or 2 players until 4-players mode will be implemented!
            NumberLimiter::applyD(_flags.test_NumPlayers, 1, 1, 2);
        }
        else
        if(param == ("--debug"))
        {
            g_AppSettings.debugMode=true;
            _flags.debugMode=true;
        }
        else
        if(param == ("--debug-pagan-god"))
        {
            if(_flags.debugMode)
                PGE_Debugger::cheat_pagangod = true;
        }
        else
        if(param == ("--debug-superman"))
        {
            if(_flags.debugMode)
                PGE_Debugger::cheat_superman = true;
        }
        else
        if(param == ("--debug-chucknorris"))
        {
            if(_flags.debugMode)
                PGE_Debugger::cheat_chucknorris = true;
        }
        else
        if(param == ("--debug-worldfreedom"))
        {
            if(_flags.debugMode)
                PGE_Debugger::cheat_worldfreedom = true;
        }
        else
        if(param == ("--debug-physics"))
        {
            if(_flags.debugMode)
                PGE_Window::showPhysicsDebug = true;
        }
        else
        if(param == ("--debug-print=yes"))
        {
            if(_flags.debugMode)
            {
                g_AppSettings.showDebugInfo = false;
                PGE_Window::showDebugInfo=true;
            }
        }
        else
        if(param == ("--debug-print=no"))
        {
            PGE_Window::showDebugInfo = false;
            g_AppSettings.showDebugInfo = false;
        }
        else
        if(param == ("--interprocessing"))
        {
            IntProc::init();
            g_AppSettings.interprocessing=true;
        }
        else
        if(param == ("--render-auto"))
        {
            _flags.rendererType = GlRenderer::RENDER_AUTO;
        }
        else
        if(param == ("--render-gl2"))
        {
            _flags.rendererType = GlRenderer::RENDER_OPENGL_2_1;
        }
        else
        if(param == ("--render-gl3"))
        {
            _flags.rendererType = GlRenderer::RENDER_OPENGL_3_1;
        }
        else
        if(param == ("--render-sw"))
        {
            _flags.rendererType = GlRenderer::RENDER_SW_SDL;
        }
        else
        if(param == ("--render-vsync"))
        {
            g_AppSettings.vsync = true;
            PGE_Window::vsync = true;
        }
        else
        {
            param = FileFormats::removeQuotes( param );
            if( QFile::exists(param) )
            {
                fileToOpen = param;
                LogDebug("Got file path: [" + param + "]");
            }
            else
            {
                LogWarning("Invalid argument or file path: [" + param + "]");
            }
        }
    }

    #ifdef __APPLE__
    if(fileToOpen.isEmpty())
    {
        a.processEvents(QEventLoop::QEventLoop::AllEvents);
        LogDebug("Attempt to take Finder args...");
        QStringList openArgs = a.getOpenFileChain();
        foreach(QString file, openArgs)
        {
            if( QFile::exists(file) )
            {
                fileToOpen = file;
                LogDebug("Got file path: [" + file + "]");
            }
            else
            {
                LogWarning("Invalid file path, sent by Mac OS X Finder event: [" + file + "]");
            }
        }
    }
    #endif

    ////Check & ask for configuration pack

    //Create empty config directory if not exists
    if(!QDir(AppPathManager::userAppDir() + "/" +  "configs").exists())
        QDir().mkdir(AppPathManager::userAppDir() + "/" +  "configs");

    // Config manager
    SelectConfig *cmanager = new SelectConfig();
    cmanager->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    cmanager->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, cmanager->size(), a.desktop()->availableGeometry() ));

    QString configPath_manager = cmanager->isPreLoaded();

    //If application runned first time or target configuration is not exist
    if(configPath_manager.isEmpty() && configPath.isEmpty())
    {
        //Ask for configuration
        if(cmanager->exec()==QDialog::Accepted)
        {
            configPath = cmanager->currentConfigPath;
        }
        else
        {
            delete cmanager;
            IntProc::quit();
            exit(0);
        }
    } else if(!configPath_manager.isEmpty() && configPath.isEmpty()) {
        configPath = cmanager->currentConfigPath;
    }
    delete cmanager;



    //Load selected configuration pack

    LogDebug("Opening of the configuration package...");
    ConfigManager::setConfigPath(configPath);

    LogDebug("Initalization of basic properties...");
    if( !ConfigManager::loadBasics() )
    {
        LogDebug( "<Application closed with failture>" );
        return 1;
    }

    LogDebug("Configuration package successfully loaded!");

    // Initalizing SDL
    LogDebug("Initialization of SDL...");
    int sdlInitFlags = 0;

    sdlInitFlags |= SDL_INIT_TIMER;
    sdlInitFlags |= SDL_INIT_AUDIO;
    sdlInitFlags |= SDL_INIT_VIDEO;
    sdlInitFlags |= SDL_INIT_EVENTS;
    sdlInitFlags |= SDL_INIT_JOYSTICK;
    //sdlInitFlags |= SDL_INIT_HAPTIC;
    sdlInitFlags |= SDL_INIT_GAMECONTROLLER;

    if ( SDL_Init(sdlInitFlags) < 0 )
    {
                                //% "Unable to init SDL!"
        PGE_Window::printSDLError(qtTrId("SDL_INIT_ERROR"));
        LogDebug( "<Application closed with failture>" );
        return 1;
    }

    LogDebug("Initialization of Audio subsystem...");
    if( PGE_MusPlayer::initAudio(44100, 32, 4096) == -1 )
    {
        QMessageBox::warning(NULL, "Audio subsystem Error",
                QString("Unable to load audio sub-system!\n%1\n\nContinuing without sound...")
                .arg( Mix_GetError() ),
                QMessageBox::Ok);
    } else {
        PGE_MusPlayer::MUS_changeVolume( g_AppSettings.volume_music );
        LogDebug("Build SFX index cache...");
        ConfigManager::buildSoundIndex(); //Load all sound effects into memory
    }

    LogDebug("Init main window...");
    if( !PGE_Window::init( QString("Platformer Game Engine - v")+
                           _FILE_VERSION+_FILE_RELEASE+" build "+_BUILD_VER,
                           _flags.rendererType ) )
    {
        LogDebug( "<Application closed with failture>" );
        return 1;
    }

    LogDebug("Init joystics...");
    g_AppSettings.initJoysticks();
    g_AppSettings.loadJoystickSettings();

    LogDebug("Clear screen...");
    GlRenderer::clearScreen();
    GlRenderer::flush();
    GlRenderer::repaint();

    if(g_AppSettings.fullScreen) qDebug()<<"Toggle fullscreen...";
    PGE_Window::setFullScreen(g_AppSettings.fullScreen);
    GlRenderer::resetViewport();

    //Init font manager
    LogDebug("Init font manager...");
    FontManager::init();

    LogDebug("Showing window...");
    SDL_ShowWindow(PGE_Window::window);
    SDL_PumpEvents();

    if( !fileToOpen.isEmpty() )
    {
        _game_state.reset();

        //Apply custom game parameters from command line
        applyTestSettings(_game_state);

        if(
           (fileToOpen.endsWith(".lvl", Qt::CaseInsensitive))
                ||
           (fileToOpen.endsWith(".lvlx", Qt::CaseInsensitive)))
        {
            _game_state.LevelFile = fileToOpen;
            _game_state.isEpisode = false;

            _flags.testLevel=true;
            _flags.testWorld=false;
            goto PlayLevel;
        }
        else
        if(
           (fileToOpen.endsWith(".wld", Qt::CaseInsensitive))
                ||
           (fileToOpen.endsWith(".wldx", Qt::CaseInsensitive)))
        {
            episode.character=1;
            episode.savefile="save1.savx";
            episode.worldfile=fileToOpen;
            _game_state._episodePath= QFileInfo(fileToOpen).absoluteDir().absolutePath()+"/";
            _game_state.saveFileName = episode.savefile;
            _game_state.isEpisode = true;
            _game_state.WorldFile = fileToOpen;

            _flags.testLevel=false;
            _flags.testWorld=true;
            goto PlayWorldMap;
        }
    }

    if(g_AppSettings.interprocessing)
    {
        //Apply custom game parameters from command line
        applyTestSettings(_game_state);
        goto PlayLevel;
    }

LoadingScreen:
    {
        LoadingScene *ttl = new LoadingScene;
        ttl->setWaitTime(15000);

        ttl->init();
        ttl->m_fader.setFade(10, 0.0f, 0.01f);
        int ret = ttl->exec();
        if(ttl->doShutDown()) ret=-1;
        delete ttl;
        if(ret==-1) goto ExitFromApplication;

        goto MainMenu;
    }

CreditsScreen:
    {
        CreditsScene *ttl = new CreditsScene;
        ttl->setWaitTime(30000);

        ttl->init();
        ttl->m_fader.setFade(10, 0.0f, 0.01f);
        int ret = ttl->exec();
        if(ttl->doShutDown()) ret=-1;
        delete ttl;
        if(ret==-1) goto ExitFromApplication;

        if(_flags.testWorld)
            goto ExitFromApplication;

        goto MainMenu;
    }

GameOverScreen:
    {
        GameOverScene GOScene;
        int result = GOScene.exec();
        if (result == GameOverSceneResult::CONTINUE)
        {
            if (_game_state.isHubLevel)
                goto PlayLevel;
            else
                goto PlayWorldMap;
        }

        if(_flags.testWorld)
            goto ExitFromApplication;

        goto MainMenu;
    }

MainMenu:
    {
        _game_state.reset();
        TitleScene * iScene = new TitleScene();
        iScene->init();
        iScene->m_fader.setFade(10, 0.0f, 0.02f);
        int answer = iScene->exec();
        PlayLevelResult   res_level   = iScene->result_level;
        PlayEpisodeResult res_episode = iScene->result_episode;
        if(iScene->doShutDown()) answer=TitleScene::ANSWER_EXIT;
        delete iScene;

        switch(answer)
        {
            case TitleScene::ANSWER_EXIT:
                goto ExitFromApplication;
            case TitleScene::ANSWER_CREDITS:
                goto CreditsScreen;
            case TitleScene::ANSWER_LOADING:
                goto LoadingScreen;
            case TitleScene::ANSWER_GAMEOVER:
                goto GameOverScreen;
            case TitleScene::ANSWER_PLAYLEVEL:
                {
                    end_level_jump=RETURN_TO_MAIN_MENU;
                    _game_state.isEpisode=false;
                    _game_state.numOfPlayers=1;
                    _game_state.LevelFile = res_level.levelfile;
                    _game_state._episodePath.clear();
                    _game_state.saveFileName.clear();
                    goto PlayLevel;
                }
            case TitleScene::ANSWER_PLAYEPISODE:
            case TitleScene::ANSWER_PLAYEPISODE_2P:
                {
                    end_level_jump=RETURN_TO_WORLDMAP;
                    _game_state.numOfPlayers=(answer==TitleScene::ANSWER_PLAYEPISODE_2P)?2:1;
                    PlayerState plr;
                    plr._chsetup = FileFormats::CreateSavCharacterState();
                    plr.characterID=1;
                    plr.stateID=1;
                    plr._chsetup.id=1;
                    plr._chsetup.state=1;
                    _game_state.setPlayerState(1, plr);
                    plr.characterID=2;
                    plr.stateID=1;
                    plr._chsetup.id=2;
                    plr._chsetup.state=1;
                    _game_state.setPlayerState(2, plr);
                    _game_state.isEpisode=true;
                    episode = res_episode;
                    _game_state._episodePath = QFileInfo(episode.worldfile).absoluteDir().absolutePath()+"/";
                    _game_state.saveFileName = episode.savefile;
                    _game_state.load();
                    goto PlayWorldMap;
                }
            default:
                goto PlayWorldMap;
        }

        goto PlayLevel;
    }

PlayWorldMap:
    {
        int ExitCode = WldExit::EXIT_close;

        WorldScene *wScene;
        wScene = new WorldScene();
        bool sceneResult=true;

        if(episode.worldfile.isEmpty())
        {
            sceneResult = false;
                             //% "No opened files"
            PGE_MsgBox::warn(qtTrId("ERROR_NO_OPEN_FILES_MSG"));
            if( g_AppSettings.debugMode || _flags.testWorld )
                goto ExitFromApplication;
            else
                goto MainMenu;
        }
        else
        {
            sceneResult = wScene->loadFile(episode.worldfile);
            wScene->setGameState(&_game_state); //Load game state to the world map
            if(!sceneResult)
            {
                //% "ERROR:\nFail to start world map\n\n%1"
                PGE_MsgBox::error(qtTrId("ERROR_FAIL_START_WLD").arg(wScene->getLastError()));
                ExitCode = WldExit::EXIT_error;
            }
        }

        if(sceneResult)
            sceneResult = wScene->init();

        if(sceneResult)
            wScene->m_fader.setFade(10, 0.0f, 0.02f);

        if(sceneResult)
            ExitCode = wScene->exec();

        if(!sceneResult)
        {
            ExitCode = WldExit::EXIT_error;
                        //% "World map was closed with error.\n%1"
            PGE_MsgBox::error(qtTrId("WLD_ERROR_LVLCLOSED").arg(wScene->errorString()));
        }

        _game_state._recent_ExitCode_world = ExitCode;

        if(wScene->doShutDown())
        {
            delete wScene;
            goto ExitFromApplication;
        }

        if(g_AppSettings.debugMode)
        {
            if(ExitCode==WldExit::EXIT_beginLevel)
            {
                QString msg;
                       //% "Start level\n%1"
                msg += qtTrId("MSG_START_LEVEL").arg(_game_state.LevelFile) + "\n\n";
                       //% "Type an exit code (signed integer)"
                msg += qtTrId("MSG_WLDTEST_EXIT_CODE");
                PGE_TextInputBox text(NULL, msg, PGE_BoxBase::msg_info_light,
                                      PGE_Point(-1,-1),
                                      ConfigManager::setup_message_box.box_padding,
                                      ConfigManager::setup_message_box.sprite);
                text.exec();

                _game_state._recent_ExitCode_level  = LvlExit::EXIT_Neutral;
                if(PGEFile::IsIntS(text.inputText()))
                {
                    _game_state._recent_ExitCode_level = toInt(text.inputText());
                }

                delete wScene;
                if(_game_state.isHubLevel) goto ExitFromApplication;

                goto PlayWorldMap;
            }
            else
            {
                delete wScene;
                goto ExitFromApplication;
            }
        }

        delete wScene;

        switch(ExitCode)
        {
            case WldExit::EXIT_beginLevel:
                goto PlayLevel;
                break;
            case WldExit::EXIT_close:
                break;
            case WldExit::EXIT_error:
                break;
            case WldExit::EXIT_exitNoSave:
                break;
            case WldExit::EXIT_exitWithSave:
                break;
            default:
                break;
        }

        if(_flags.testWorld)
            goto ExitFromApplication;

        goto MainMenu;
    }


PlayLevel:
    {
        bool playAgain = true;
        long entranceID = 0;
        LevelScene* lScene;
        while(playAgain)
        {
            entranceID = _game_state.LevelTargetWarp;

            if( _game_state.LevelFile_hub == _game_state.LevelFile )
            {
                    _game_state.isHubLevel = true;
                    entranceID = _game_state.game_state.last_hub_warp;
            }

            int ExitCode=0;
                lScene = new LevelScene();

                lScene->setGameState( &_game_state );
                bool sceneResult=true;

                if( _game_state.LevelFile.isEmpty() )
                {
                    if(g_AppSettings.interprocessing && IntProc::isEnabled())
                    {
                        sceneResult = lScene->loadFileIP();
                        if((!sceneResult) && (!lScene->isExiting()))
                        {
                            //SDL_Delay(50);
                            ExitCode = WldExit::EXIT_error;
                            PGE_MsgBox msgBox(NULL, QString("ERROR:\nFail to start level\n\n%1")
                                              .arg(lScene->getLastError()),
                                              PGE_MsgBox::msg_error);
                            msgBox.exec();
                        }
                    }
                    else
                    {
                        sceneResult = false;
                        ExitCode = WldExit::EXIT_error;
                                              //% "No opened files"
                        PGE_MsgBox::warn(qtTrId("ERROR_NO_OPEN_FILES_MSG"));
                    }
                }
                else
                {
                    sceneResult = lScene->loadFile(_game_state.LevelFile);
                    if(!sceneResult)
                    {
                        SDL_Delay(50);
                        PGE_MsgBox msgBox(NULL, QString("ERROR:\nFail to start level\n\n"
                                                        "%1")
                                          .arg(lScene->getLastError()),
                                          PGE_MsgBox::msg_error);
                        msgBox.exec();
                    }
                }

                if(sceneResult)
                    sceneResult = lScene->setEntrance(entranceID);

                if(sceneResult)
                    sceneResult = lScene->init();

                if(sceneResult)
                {
                    lScene->m_fader.setFade(10, 0.0f, 0.02f);
                    ExitCode = lScene->exec();
                    _game_state._recent_ExitCode_level = ExitCode;
                }

                if(!sceneResult)
                    ExitCode = LvlExit::EXIT_Error;

                switch(ExitCode)
                {
                case LvlExit::EXIT_Warp:
                    {
                       if(lScene->warpToWorld)
                       {
                           _game_state.game_state.worldPosX = lScene->toWorldXY().x();
                           _game_state.game_state.worldPosY = lScene->toWorldXY().y();
                           _game_state.LevelFile.clear();
                           entranceID = 0;
                           end_level_jump = _game_state.isEpisode ? RETURN_TO_WORLDMAP : RETURN_TO_MAIN_MENU;
                       }
                       else
                       {
                           _game_state.LevelFile = lScene->toAnotherLevel();
                           _game_state.LevelTargetWarp = lScene->toAnotherEntrance();
                           entranceID = _game_state.LevelTargetWarp;

                           if(_game_state.isHubLevel)
                           {
                               _game_state.isHubLevel=false;
                               _game_state.game_state.last_hub_warp = lScene->lastWarpID;
                           }
                       }

                       if(_game_state.LevelFile.isEmpty()) playAgain = false;


                       if(g_AppSettings.debugMode)
                       {
                           if(!fileToOpen.isEmpty())
                           {
                                               //% "Warp exit\n\nExit to:\n%1\n\nEnter to: %2"
                               PGE_MsgBox::warn( qtTrId("LVL_EXIT_WARP_INFO") .arg(fileToOpen) .arg(entranceID) );
                           }
                           playAgain = false;
                       }
                    }
                    break;
                case LvlExit::EXIT_Closed:
                    {
                        end_level_jump=RETURN_TO_EXIT;
                        playAgain = false;
                    }
                    break;
                case LvlExit::EXIT_MenuExit:
                    {
                        end_level_jump = _game_state.isEpisode ? RETURN_TO_WORLDMAP : RETURN_TO_MAIN_MENU;
                        if(_game_state.isHubLevel)
                            end_level_jump = _flags.testLevel ? RETURN_TO_EXIT : RETURN_TO_MAIN_MENU;

                        playAgain = false;
                    }
                    break;
                case LvlExit::EXIT_PlayerDeath:
                    {
                        playAgain = _game_state.isEpisode ? _game_state.replay_on_fail : true;
                        end_level_jump = _game_state.isEpisode ? RETURN_TO_WORLDMAP : RETURN_TO_MAIN_MENU;
                        //check the number of player lives here and decided to return worldmap or gameover
                        if(_game_state.isEpisode)
                        {
                            _game_state.game_state.lives--;
                            if(_game_state.game_state.lives<0)
                            {
                                playAgain=false;
                                _game_state.game_state.coins=0;
                                _game_state.game_state.points=0;
                                _game_state.game_state.lives=3;
                                end_level_jump=RETURN_TO_GAMEOVER_SCREEN;
                            }
                        }
                    }
                    break;
                case LvlExit::EXIT_Error:
                    {
                        end_level_jump = (_game_state.isEpisode)? RETURN_TO_WORLDMAP : RETURN_TO_MAIN_MENU;
                        playAgain = false;
                                            //% "Level was closed with error.\n%1"
                        PGE_MsgBox::error(qtTrId("LVL_ERROR_LVLCLOSED").arg(lScene->errorString()));
                    }
                    break;
                default:
                    end_level_jump = _game_state.isEpisode ? RETURN_TO_WORLDMAP : RETURN_TO_MAIN_MENU;
                    playAgain = false;
                }

                if(_flags.testLevel || g_AppSettings.debugMode)
                    end_level_jump=RETURN_TO_EXIT;

                ConfigManager::unloadLevelConfigs();
                delete lScene;

                if(g_AppSettings.interprocessing)
                    goto ExitFromApplication;
        }

        switch(end_level_jump)
        {
            case RETURN_TO_WORLDMAP:
                goto PlayWorldMap;
            case RETURN_TO_MAIN_MENU:
                goto MainMenu;
            case RETURN_TO_EXIT:
                goto ExitFromApplication;
            case RETURN_TO_GAMEOVER_SCREEN:
                goto GameOverScreen;
            case RETURN_TO_CREDITS_SCREEN:
                goto CreditsScreen;
        }
    }

ExitFromApplication:
    ConfigManager::unluadAll();
    if( IntProc::isEnabled() )
        IntProc::editor->shut();
    PGE_MusPlayer::freeStream();
    PGE_Sounds::clearSoundBuffer();
    Mix_CloseAudio();
    g_AppSettings.save();
    g_AppSettings.closeJoysticks();
    IntProc::quit();
    FontManager::quit();
    PGE_Window::uninit();
    LogDebug( "<Application closed>" );
    CloseLog();
    a.quit();
    return 0;
}

