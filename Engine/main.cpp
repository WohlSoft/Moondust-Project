/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#include <memory> //shared_ptr
#include "engine.hpp"

#include "version.h"

#include <audio/play_music.h>
#include <common_features/logger.h>
#include <common_features/tr.h>
#include <common_features/pge_delay.h>

#include <PGE_File_Formats/pge_x.h>

#include <Utils/files.h>
#include <DirManager/dirman.h>
#include <common_features/fmt_format_ne.h>
#include <fmt/fmt_qformat.h>

#include <gui/pge_msgbox.h>
#include <gui/pge_textinputbox.h>

#include <settings/global_settings.h>

#include <data_configs/config_manager.h>
#include <data_configs/config_select_scene/scene_config_select.h>

#include <scenes/scene_level.h>
#include <scenes/scene_world.h>
#include <scenes/scene_loading.h>
#include <scenes/scene_title.h>
#include <scenes/scene_credits.h>
#include <scenes/scene_gameover.h>

#include <networking/intproc.h>


#ifdef __APPLE__
/**
 * @brief Receive an opened file from the Finder (Must be created at least one window!)
 */
static void macosReceiveOpenFile()
{
    if(g_fileToOpen.empty())
    {
        pLogDebug("Attempt to take Finder args...");
        SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_DROPFILE)
            {
                std::string file(event.drop.file);
                if(Files::fileExists(file))
                {
                    g_fileToOpen = file;
                    pLogDebug("Got file path: [%s]", file.c_str());
                }
                else
                    pLogWarning("Invalid file path, sent by Mac OS X Finder event: [%s]", file.c_str());
            }
        }
        SDL_EventState(SDL_DROPFILE, SDL_DISABLE);
    }
}
#endif


struct AppInstance
{
    enum NextState
    {
        G_LOADING_SCREEN,
        G_CREDITS_SCREEN,
        G_GAME_OVER_SCREEN,
        G_MAIN_MENU,
        G_PLAY_WORLDMAP,
        G_PLAY_LEVEL,
        G_EXIT_APPLICATION
    };

    //! Next screen after end of currently one
    NextState next = G_LOADING_SCREEN;

    enum Level_returnTo
    {
        RETURN_TO_MAIN_MENU = 0,
        RETURN_TO_WORLDMAP,
        RETURN_TO_GAMEOVER_SCREEN,
        RETURN_TO_CREDITS_SCREEN,
        RETURN_TO_EXIT
    };
    //! Target scene after exiting from the level scene
    Level_returnTo goToOnLevelEnd = RETURN_TO_EXIT;

    //! Global game state
    EpisodeState  gameState;
};


static void pgeLoadingScreen(AppInstance &a)
{
    LoadingScene ttl;
    ttl.setWaitTime(15000);
    ttl.init();
    ttl.m_fader.setFade(10, 0.0, 0.01);
    int ret = ttl.exec();

    if(ttl.doShutDown())
        ret = -1;

    if(ret == -1)
    {
        a.next = AppInstance::G_EXIT_APPLICATION;
        return;
    }

    a.next = AppInstance::G_MAIN_MENU;
}


static void pgeCreditsScreen(AppInstance &a)
{
    CreditsScene ttl;
    ttl.setWaitTime(30000);
    ttl.init();
    ttl.m_fader.setFade(10, 0.0, 0.01);
    int ret = ttl.exec();

    if(ttl.doShutDown())
        ret = -1;

    if(ret == -1)
    {
        a.next = AppInstance::G_EXIT_APPLICATION;
        return;
    }

    if(g_flags.testWorld)
    {
        a.next = AppInstance::G_EXIT_APPLICATION;
        return;
    }

    a.next = AppInstance::G_MAIN_MENU;
}


static void pgeGameOverScreen(AppInstance &a)
{
    GameOverScene GOScene;
    int result = GOScene.exec();

    if(result == GameOverSceneResult::CONTINUE)
    {
        if(a.gameState.m_isHubLevel)
        {
            a.next = AppInstance::G_PLAY_LEVEL;
            return;
        }
        else
        {
            a.next = AppInstance::G_PLAY_WORLDMAP;
            return;
        }
    }

    if(g_flags.testWorld)
    {
        a.next = AppInstance::G_EXIT_APPLICATION;
        return;
    }

    a.next = AppInstance::G_MAIN_MENU;
}


static void pgeMainMenuScreen(AppInstance &a)
{
    a.gameState.reset();
    std::shared_ptr<TitleScene> iScene(new TitleScene());
    iScene->init();
    iScene->m_fader.setFade(10, 0.0, 0.02);
    int answer = iScene->exec();
    PlayLevelResult   res_level   = iScene->m_result_level;
    PlayEpisodeResult res_episode = iScene->m_result_episode;

    if(iScene->doShutDown())
        answer = TitleScene::ANSWER_EXIT;

    switch(answer)
    {
    case TitleScene::ANSWER_EXIT:
        a.next = AppInstance::G_EXIT_APPLICATION;
        return;

    case TitleScene::ANSWER_CREDITS:
        a.next = AppInstance::G_CREDITS_SCREEN;
        return;

    case TitleScene::ANSWER_LOADING:
        a.next = AppInstance::G_LOADING_SCREEN;
        return;

    case TitleScene::ANSWER_GAMEOVER:
        a.next = AppInstance::G_GAME_OVER_SCREEN;
        return;

    case TitleScene::ANSWER_PLAYLEVEL:
    {
        a.goToOnLevelEnd = AppInstance::RETURN_TO_MAIN_MENU;
        a.gameState.m_isEpisode = false;
        a.gameState.m_numOfPlayers = 1;
        a.gameState.m_nextLevelFile = res_level.levelfile;
        a.gameState.m_episodePath.clear();
        a.gameState.m_saveFileName.clear();
        a.gameState.m_isTestingModeL = true;

        a.next = AppInstance::G_PLAY_LEVEL;
        return;
    }

    case TitleScene::ANSWER_PLAYEPISODE:
    case TitleScene::ANSWER_PLAYEPISODE_2P:
    {
        a.goToOnLevelEnd = AppInstance::RETURN_TO_WORLDMAP;
        a.gameState.m_numOfPlayers = (answer == TitleScene::ANSWER_PLAYEPISODE_2P) ? 2 : 1;
        PlayerState plr;
        plr._chsetup = FileFormats::CreateSavCharacterState();
        plr.characterID = 1;
        plr.stateID = 1;
        plr._chsetup.id = 1;
        plr._chsetup.state = 1;
        a.gameState.setPlayerState(1, plr);
        plr.characterID = 2;
        plr.stateID = 1;
        plr._chsetup.id = 2;
        plr._chsetup.state = 1;
        a.gameState.setPlayerState(2, plr);
        a.gameState.m_isEpisode = true;
        g_Episode = res_episode;
        a.gameState.m_episodePath = DirMan(Files::dirname(g_Episode.worldfile)).absolutePath() + "/";
        a.gameState.m_worldFileName = g_Episode.worldfile;
        a.gameState.m_saveFileName = g_Episode.savefile;
        a.gameState.load();

        a.next = AppInstance::G_PLAY_WORLDMAP;
        return;
    }

    default:
        a.next = AppInstance::G_PLAY_WORLDMAP;
        return;
    }
}


static void pgeWorldMapScreen(AppInstance &a)
{
    WldExit::ExitWorldCodes wldExitCode = WldExit::EXIT_close;
    std::shared_ptr<WorldScene> wScene;
    wScene.reset(new WorldScene());
    bool sceneResult = true;

    if(g_Episode.worldfile.empty())
    {
        sceneResult = false;
        //% "No opened files"
        PGE_MsgBox::warn(qtTrId("ERROR_NO_OPEN_FILES_MSG"));

        if(g_AppSettings.debugMode || g_flags.testWorld)
        {
            a.next = AppInstance::G_EXIT_APPLICATION;
            return;
        }
        else
        {
            a.next = AppInstance::G_MAIN_MENU;
            return;
        }
    }
    else
    {
        sceneResult = wScene->loadFile(g_Episode.worldfile);
        wScene->setGameState(&a.gameState); //Load game state to the world map

        if(!sceneResult)
        {
            //% "ERROR:\nFail to start world map\n\n%1"
            PGE_MsgBox::error( fmt::qformat(qtTrId("ERROR_FAIL_START_WLD"), wScene->getLastError()) );
            wldExitCode = WldExit::EXIT_error;
        }
    }

    if(sceneResult)
        sceneResult = wScene->init();

    if(sceneResult)
        wScene->m_fader.setFade(10, 0.0, 0.02);

    if(sceneResult)
        wldExitCode = (WldExit::ExitWorldCodes)wScene->exec();

    if(!sceneResult)
    {
        wldExitCode = WldExit::EXIT_error;
        //% "World map was closed with error.\n%1"
        PGE_MsgBox::error( fmt::qformat(qtTrId("WLD_ERROR_LVLCLOSED"), wScene->errorString()) );
    }

    a.gameState.m_lastWorldExitCode = (int)wldExitCode;

    if(wScene->doShutDown())
    {
        wScene.reset();
        a.next = AppInstance::G_EXIT_APPLICATION;
        return;
    }

    if(g_AppSettings.debugMode)
    {
        if(wldExitCode == WldExit::EXIT_beginLevel)
        {
            std::string msg;
            //% "Start level\n%1"
            msg += fmt::qformat(qtTrId("MSG_START_LEVEL"), a.gameState.m_nextLevelFile) + "\n\n";
            //% "Type an exit code (signed integer)"
            msg += qtTrId("MSG_WLDTEST_EXIT_CODE");
            PGE_TextInputBox text(nullptr, msg, PGE_BoxBase::msg_info_light,
                                  PGE_Point(-1, -1),
                                  ConfigManager::setup_message_box.box_padding,
                                  ConfigManager::setup_message_box.sprite);
            text.exec();
            a.gameState.m_lastLevelExitCode  = LvlExit::EXIT_Neutral;

            if(PGEFile::IsIntS(text.inputText()))
                a.gameState.m_lastLevelExitCode = SDL_atoi(text.inputText().c_str());

            if(a.gameState.m_isHubLevel)
            {
                a.next = AppInstance::G_EXIT_APPLICATION;
                return;
            }

            a.next = AppInstance::G_PLAY_WORLDMAP;
            return;
        }
        else
        {
            a.next = AppInstance::G_EXIT_APPLICATION;
            return;
        }
    }

    switch(wldExitCode)
    {
    case WldExit::EXIT_beginLevel:
    {
        a.next = AppInstance::G_PLAY_LEVEL;
        return;
    }

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

    if(g_flags.testWorld)
    {
        a.next = AppInstance::G_EXIT_APPLICATION;
        return;
    }

    a.next = AppInstance::G_MAIN_MENU;
}


static void pgeLevelPlayScreen(AppInstance &a)
{
    bool playAgain = true;
    unsigned long entranceID = 0;
    std::shared_ptr<LevelScene> lScene(nullptr);

    while(playAgain)
    {
        entranceID = a.gameState.m_nextLevelEnterWarp;

        if(a.gameState.m_currentHubLevelFile == a.gameState.m_nextLevelFile)
        {
            a.gameState.m_isHubLevel = true;
            entranceID = a.gameState.m_gameSave.last_hub_warp;
        }

        int levelExitCode = 0;
        lScene.reset(new LevelScene());

        if(g_AppSettings.interprocessing)
            a.gameState.m_isTestingModeL = true;

        lScene->setGameState(&a.gameState);
        bool sceneResult = true;

        if(a.gameState.m_nextLevelFile.empty())
        {
            if(g_AppSettings.interprocessing && IntProc::isEnabled())
            {
                sceneResult = lScene->loadFileIP();

                if((!sceneResult) && (!lScene->isExiting()))
                {
                    //PGE_Delay(50);
                    levelExitCode = LvlExit::EXIT_Error;
                    PGE_MsgBox msgBox(nullptr, fmt::format_ne("ERROR:\nFail to start level\n\n{0}",
                                            lScene->getLastError()),
                                            PGE_MsgBox::msg_error);
                    msgBox.exec();
                }
            }
            else
            {
                sceneResult = false;
                levelExitCode = LvlExit::EXIT_Error;
                //% "No opened files"
                PGE_MsgBox::warn(qtTrId("ERROR_NO_OPEN_FILES_MSG"));
            }
        }
        else
        {
            sceneResult = lScene->loadFile(a.gameState.m_nextLevelFile);

            if(!sceneResult)
            {
                PGE_Delay(50);
                PGE_MsgBox msgBox(nullptr,
                                  fmt::format_ne("ERROR:\nFail to start level\n\n"
                                                 "{0}", lScene->getLastError()),
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
            lScene->m_fader.setFade(10, 0.0, 0.02);
            levelExitCode = lScene->exec();
            a.gameState.m_lastLevelExitCode = levelExitCode;
        }

        if(!sceneResult)
            levelExitCode = LvlExit::EXIT_Error;

        switch(levelExitCode)
        {
        case LvlExit::EXIT_Warp:
        {
            if(lScene->m_warpToWorld)
            {
                a.gameState.m_gameSave.worldPosX = lScene->toWorldXY().x();
                a.gameState.m_gameSave.worldPosY = lScene->toWorldXY().y();
                a.gameState.m_nextLevelFile.clear();
                entranceID = 0;
                a.goToOnLevelEnd = a.gameState.m_isEpisode ?
                                        AppInstance::RETURN_TO_WORLDMAP :
                                        AppInstance::RETURN_TO_MAIN_MENU;
            }
            else
            {
                a.gameState.m_nextLevelFile = lScene->toAnotherLevel();
                a.gameState.m_nextLevelEnterWarp = lScene->toAnotherEntrance();
                entranceID = a.gameState.m_nextLevelEnterWarp;

                if(a.gameState.m_isHubLevel)
                {
                    a.gameState.m_isHubLevel = false;
                    a.gameState.m_gameSave.last_hub_warp = lScene->m_lastWarpID;
                }
            }

            if(a.gameState.m_nextLevelFile.empty())
                playAgain = false;

            if(g_AppSettings.debugMode)
            {
                std::string target;

                if(lScene->m_warpToWorld)
                {
                    target = fmt::format_ne("X={0}, Y={1}",
                             a.gameState.m_gameSave.worldPosX,
                             a.gameState.m_gameSave.worldPosY);
                }
                else
                    target = a.gameState.m_nextLevelFile;

                if(!target.empty())
                {
                    //% "Warp exit\n\nExit into:\n%1\n\nEntrance point: %2"
                    PGE_MsgBox::warn( fmt::qformat(qtTrId("LVL_EXIT_WARP_INFO"), target, entranceID) );
                }

                playAgain = false;
            }
        }
        break;

        case LvlExit::EXIT_Closed:
        {
            a.goToOnLevelEnd = AppInstance::RETURN_TO_EXIT;
            playAgain = false;
        }
        break;

        case LvlExit::EXIT_ReplayRequest:
        {
            playAgain = true;
        }
        break;

        case LvlExit::EXIT_MenuExit:
        {
            a.goToOnLevelEnd = a.gameState.m_isEpisode ?
                                    AppInstance::RETURN_TO_WORLDMAP :
                                    AppInstance::RETURN_TO_MAIN_MENU;

            if(a.gameState.m_isHubLevel)
            {
                a.goToOnLevelEnd = g_flags.testLevel ?
                                    AppInstance::RETURN_TO_EXIT :
                                    AppInstance::RETURN_TO_MAIN_MENU;
            }

            playAgain = false;
        }
        break;

        case LvlExit::EXIT_PlayerDeath:
        {
            playAgain = a.gameState.m_isEpisode ? a.gameState.m_autoRestartFailedLevel : true;
            a.goToOnLevelEnd = a.gameState.m_isEpisode ?
                                    AppInstance::RETURN_TO_WORLDMAP :
                                    AppInstance::RETURN_TO_MAIN_MENU;

            //check the number of player lives here and decided to return worldmap or gameover
            if(a.gameState.m_isEpisode)
            {
                a.gameState.m_gameSave.lives--;

                if(a.gameState.m_gameSave.lives < 0)
                {
                    playAgain = false;
                    a.gameState.m_gameSave.coins = 0;
                    a.gameState.m_gameSave.points = 0;
                    a.gameState.m_gameSave.lives = 3;
                    a.goToOnLevelEnd = AppInstance::RETURN_TO_GAMEOVER_SCREEN;
                }
            }
        }
        break;

        case LvlExit::EXIT_Error:
        {
            a.goToOnLevelEnd = a.gameState.m_isEpisode ?
                                    AppInstance::RETURN_TO_WORLDMAP :
                                    AppInstance::RETURN_TO_MAIN_MENU;
            playAgain = false;
            //% "Level was closed with error.\n%1"
            PGE_MsgBox::error( fmt::qformat(qtTrId("LVL_ERROR_LVLCLOSED"), lScene->errorString()) );
        }
        break;

        default:
            a.goToOnLevelEnd = a.gameState.m_isEpisode ?
                                    AppInstance::RETURN_TO_WORLDMAP :
                                    AppInstance::RETURN_TO_MAIN_MENU;
            playAgain = false;
        }

        if(g_flags.testLevel || g_AppSettings.debugMode)
            a.goToOnLevelEnd = AppInstance::RETURN_TO_EXIT;

        ConfigManager::unloadLevelConfigs();
        lScene.reset();
    }

    if(g_AppSettings.interprocessing)
    {
        a.next = AppInstance::G_EXIT_APPLICATION;
        return;
    }

    switch(a.goToOnLevelEnd)
    {
    default:
    case AppInstance::RETURN_TO_WORLDMAP:
        a.next = AppInstance::G_PLAY_WORLDMAP;
        return;

    case AppInstance::RETURN_TO_MAIN_MENU:
        a.next = AppInstance::G_MAIN_MENU;
        return;

    case AppInstance::RETURN_TO_EXIT:
        a.next = AppInstance::G_EXIT_APPLICATION;
        return;

    case AppInstance::RETURN_TO_GAMEOVER_SCREEN:
        a.next = AppInstance::G_GAME_OVER_SCREEN;
        return;

    case AppInstance::RETURN_TO_CREDITS_SCREEN:
        a.next = AppInstance::G_CREDITS_SCREEN;
        return;
    }
}


int main(int argc, char *argv[])
{
    std::vector<std::string> args;
    for(int i = 0; i < argc; i++)
        args.emplace_back(argv[i]);

    // Parse --version or --install low args
    if(!PGEEngineApp::parseLowArgs(args))
        return 0;

    AppInstance instance;

    // RAII for loaded/initialized libraries and modules
    PGEEngineApp  app;
    //Initialize Qt's subsystem
    AppPathManager::initAppPath();
    //Load settings
    app.loadSettings();
    //Init log writer
    app.loadLogger();
    //Initialize translation sub-system
    app.loadTr();
    // Parse high arguments
    app.parseHighArgs(args);

    // Initializing SDL
    if(app.initSDL())
    {
        //% "Unable to init SDL!"
        PGE_Window::printSDLError(qtTrId("SDL_INIT_ERROR"));
        pLogDebug("<Application closed with failure>");
        return 1;
    }

    if(g_flags.audioEnabled && app.initAudio(g_flags.audioEnabled))
    {
        std::string msg = "Unable to load audio sub-system!\n";
        msg += app.errorAudio();
        msg += "\n\nContinuing without sound...";
        pLogWarning(msg.c_str());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING,
                                 "Audio subsystem Error",
                                 msg.c_str(),
                                 nullptr);
        g_flags.audioEnabled = false;
    }

    if(app.initWindow(INITIAL_WINDOW_TITLE, g_flags.rendererType))
    {
        pLogDebug("<Application closed with failure>");
        return 1;
    }

    app.loadJoysticks();
    SDL_PumpEvents();

    if(g_AppSettings.fullScreen)
        pLogDebug("Toggle fullscreen...");

#ifdef __APPLE__
    macosReceiveOpenFile();
#endif

    PGE_Window::setFullScreen(g_AppSettings.fullScreen);
    GlRenderer::resetViewport();
    //Init font manager
    app.initFontBasics();
    pLogDebug("Showing window...");
    SDL_ShowWindow(PGE_Window::window);
    pLogDebug("Clear screen...");
    GlRenderer::clearScreen();
    GlRenderer::flush();
    GlRenderer::repaint();
    SDL_PumpEvents();
    /************************************************
     *      Check & ask for configuration pack      *
     ************************************************/
    //Process config manager screen
    {
        // Create configs folder if not exists
        app.createConfigsDir();
        // Initialize config selection screen
        ConfigSelectScene GOScene;

        // Are any config packs exists?
        if(!GOScene.hasConfigPacks())
        {
            pLogCritical("Config packs not found");
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                                     //% "Config packs not found"
                                     qtTrId("ERROR_NO_CONFIG_PACKS_TTL").c_str(),
                                     /*% "Can't start game, because available\n"
                                         "configuration packages are not found!" */
                                     qtTrId("ERROR_NO_CONFIG_PACKS_TEXT").c_str(),
                                     PGE_Window::window);
            return 2;
        }

        std::string configPath_manager = GOScene.isPreLoaded();

        if(!g_fileToOpen.empty())
        {
            //% "Choose a game to test:"
            GOScene.setLabel(qtTrId("CONFIG_SELECT_TEST"));
        }

        //If application have ran a first time or target configuration is not exist
        if(configPath_manager.empty() && g_configPackPath.empty())
        {
            //Ask for configuration
            if(GOScene.exec() == 1)
                g_configPackPath = GOScene.currentConfigPath;
            else
                return 2;
        }
        else if(!configPath_manager.empty() && g_configPackPath.empty())
            g_configPackPath = GOScene.currentConfigPath;

        pLogDebug("Opening of the configuration package...");
        ConfigManager::setConfigPath(g_configPackPath);

        pLogDebug("Initialization of basic properties...");

        if(!ConfigManager::loadBasics())
        {
            pLogDebug("<Application closed with failure>");
            return 1;
        }

        app.enableConfigManager();

        if(!ConfigManager::config_name.empty())
            PGE_Window::setWindowTitle(ConfigManager::config_name);

        pLogDebug("Current scene resolution: %d x %d", PGE_Window::Width, PGE_Window::Height);
        pLogDebug("Config pack scene resolution: %d x %d", ConfigManager::viewport_width, ConfigManager::viewport_height);

        if(ConfigManager::viewport_width != static_cast<unsigned int>(PGE_Window::Width) ||
           ConfigManager::viewport_height != static_cast<unsigned int>(PGE_Window::Height))
        {
            PGE_Window::changeViewportResolution(ConfigManager::viewport_width, ConfigManager::viewport_height);
            pLogDebug("Using scene resolution: %d x %d", ConfigManager::viewport_width, ConfigManager::viewport_height);
        }

        pLogDebug("Configuration package successfully loaded!");

        if(g_flags.audioEnabled)
        {
            PGE_MusPlayer::setVolume(g_AppSettings.volume_music);
            pLogDebug("Build SFX index cache...");
            ConfigManager::buildSoundIndex(); //Load all sound effects into memory
        }

        //Init font manager
        app.initFontFull();
    }

    if(!g_fileToOpen.empty())
    {
        instance.gameState.reset();
        //Apply custom game parameters from command line
        g_flags.applyTestSettings(instance.gameState);

        if(Files::hasSuffix(g_fileToOpen, ".lvl") || Files::hasSuffix(g_fileToOpen, ".lvlx"))
        {
            instance.gameState.m_nextLevelFile = g_fileToOpen;
            instance.gameState.m_isEpisode = false;
            instance.gameState.m_isTestingModeL = true;
            instance.gameState.m_isTestingModeW = false;
            g_flags.testLevel = true;
            g_flags.testWorld = false;
            instance.next = AppInstance::G_PLAY_LEVEL;
            g_AppSettings.interprocessing = false; // Disable inteprocessing
        }
        else if(Files::hasSuffix(g_fileToOpen, ".wld") || Files::hasSuffix(g_fileToOpen, ".wldx"))
        {
            g_Episode.character = 1;
            g_Episode.savefile = "save1.savx";
            g_Episode.worldfile = g_fileToOpen;
            instance.gameState.m_episodePath = DirMan(Files::dirname(g_fileToOpen)).absolutePath() + "/";
            instance.gameState.m_saveFileName = g_Episode.savefile;
            instance.gameState.m_isEpisode = true;
            instance.gameState.m_nextWorldFile = g_fileToOpen;
            instance.gameState.m_isTestingModeL = false;
            instance.gameState.m_isTestingModeW = true;
            g_flags.testLevel = false;
            g_flags.testWorld = true;
            instance.next = AppInstance::G_PLAY_WORLDMAP;
            g_AppSettings.interprocessing = false; // Disable inteprocessing
        }
    }

    if(g_AppSettings.interprocessing)
    {
        //Apply custom game parameters from command line
        g_flags.applyTestSettings(instance.gameState);
        instance.next = AppInstance::G_PLAY_LEVEL;
    }

    // Worker
    while(instance.next != AppInstance::G_EXIT_APPLICATION)
    {
        switch(instance.next)
        {
        case AppInstance::G_LOADING_SCREEN:
            pgeLoadingScreen(instance);
            break;
        case AppInstance::G_CREDITS_SCREEN:
            pgeCreditsScreen(instance);
            break;
        case AppInstance::G_GAME_OVER_SCREEN:
            pgeGameOverScreen(instance);
            break;
        case AppInstance::G_MAIN_MENU:
            pgeMainMenuScreen(instance);
            break;
        case AppInstance::G_PLAY_WORLDMAP:
            pgeWorldMapScreen(instance);
            break;
        case AppInstance::G_PLAY_LEVEL:
            pgeLevelPlayScreen(instance);
            break;
        default:
        case AppInstance::G_EXIT_APPLICATION:
            break;
        }
    }

    return 0;
}
