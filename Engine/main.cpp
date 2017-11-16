/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
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

int main(int argc, char *argv[])
{
    std::vector<std::string> args;
    for(int i = 0; i < argc; i++)
        args.push_back(std::string(argv[i]));

    // Parse --version or --install low args
    if(!PGEEngineApp::parseLowArgs(args))
        return 0;

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

    // Initalizing SDL
    if(app.initSDL())
    {
        //% "Unable to init SDL!"
        PGE_Window::printSDLError(qtTrId("SDL_INIT_ERROR"));
        pLogDebug("<Application closed with failture>");
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
                                 NULL);
        g_flags.audioEnabled = false;
    }

    if(app.initWindow(INITIAL_WINDOW_TITLE, g_flags.rendererType))
    {
        pLogDebug("<Application closed with failture>");
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

        //If application runned first time or target configuration is not exist
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

        pLogDebug("Initalization of basic properties...");

        if(!ConfigManager::loadBasics())
        {
            pLogDebug("<Application closed with failture>");
            return 1;
        }

        app.enableConfigManager();

        if(!ConfigManager::config_name.empty())
            PGE_Window::setWindowTitle(ConfigManager::config_name);

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
        g_GameState.reset();
        //Apply custom game parameters from command line
        g_flags.applyTestSettings(g_GameState);

        if(Files::hasSuffix(g_fileToOpen, ".lvl") || Files::hasSuffix(g_fileToOpen, ".lvlx"))
        {
            g_GameState.LevelFile = g_fileToOpen;
            g_GameState.isEpisode = false;
            g_GameState.isTestingModeL = true;
            g_GameState.isTestingModeW = false;
            g_flags.testLevel = true;
            g_flags.testWorld = false;
            goto PlayLevel;
        }
        else if(Files::hasSuffix(g_fileToOpen, ".wld") || Files::hasSuffix(g_fileToOpen, ".wldx"))
        {
            g_Episode.character = 1;
            g_Episode.savefile = "save1.savx";
            g_Episode.worldfile = g_fileToOpen;
            g_GameState._episodePath = DirMan(Files::dirname(g_fileToOpen)).absolutePath() + "/";
            g_GameState.saveFileName = g_Episode.savefile;
            g_GameState.isEpisode = true;
            g_GameState.WorldFile = g_fileToOpen;
            g_GameState.isTestingModeL = false;
            g_GameState.isTestingModeW = true;
            g_flags.testLevel = false;
            g_flags.testWorld = true;
            goto PlayWorldMap;
        }
    }

    if(g_AppSettings.interprocessing)
    {
        //Apply custom game parameters from command line
        g_flags.applyTestSettings(g_GameState);
        goto PlayLevel;
    }

LoadingScreen:
    {
        LoadingScene ttl;
        ttl.setWaitTime(15000);
        ttl.init();
        ttl.m_fader.setFade(10, 0.0, 0.01);
        int ret = ttl.exec();

        if(ttl.doShutDown())
            ret = -1;

        if(ret == -1)
            goto ExitFromApplication;

        goto MainMenu;
    }
CreditsScreen:
    {
        CreditsScene ttl;
        ttl.setWaitTime(30000);
        ttl.init();
        ttl.m_fader.setFade(10, 0.0, 0.01);
        int ret = ttl.exec();

        if(ttl.doShutDown())
            ret = -1;

        if(ret == -1) goto ExitFromApplication;

        if(g_flags.testWorld)
            goto ExitFromApplication;

        goto MainMenu;
    }
GameOverScreen:
    {
        GameOverScene GOScene;
        int result = GOScene.exec();

        if(result == GameOverSceneResult::CONTINUE)
        {
            if(g_GameState.isHubLevel)
                goto PlayLevel;
            else
                goto PlayWorldMap;
        }

        if(g_flags.testWorld)
            goto ExitFromApplication;

        goto MainMenu;
    }
MainMenu:
    {
        g_GameState.reset();
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
            goto ExitFromApplication;

        case TitleScene::ANSWER_CREDITS:
            goto CreditsScreen;

        case TitleScene::ANSWER_LOADING:
            goto LoadingScreen;

        case TitleScene::ANSWER_GAMEOVER:
            goto GameOverScreen;

        case TitleScene::ANSWER_PLAYLEVEL:
        {
            g_jumpOnLevelEndTo = RETURN_TO_MAIN_MENU;
            g_GameState.isEpisode = false;
            g_GameState.numOfPlayers = 1;
            g_GameState.LevelFile = res_level.levelfile;
            g_GameState._episodePath.clear();
            g_GameState.saveFileName.clear();
            g_GameState.isTestingModeL = true;
            goto PlayLevel;
        }

        case TitleScene::ANSWER_PLAYEPISODE:
        case TitleScene::ANSWER_PLAYEPISODE_2P:
        {
            g_jumpOnLevelEndTo = RETURN_TO_WORLDMAP;
            g_GameState.numOfPlayers = (answer == TitleScene::ANSWER_PLAYEPISODE_2P) ? 2 : 1;
            PlayerState plr;
            plr._chsetup = FileFormats::CreateSavCharacterState();
            plr.characterID = 1;
            plr.stateID = 1;
            plr._chsetup.id = 1;
            plr._chsetup.state = 1;
            g_GameState.setPlayerState(1, plr);
            plr.characterID = 2;
            plr.stateID = 1;
            plr._chsetup.id = 2;
            plr._chsetup.state = 1;
            g_GameState.setPlayerState(2, plr);
            g_GameState.isEpisode = true;
            g_Episode = res_episode;
            g_GameState._episodePath = DirMan(Files::dirname(g_Episode.worldfile)).absolutePath() + "/";
            g_GameState.saveFileName = g_Episode.savefile;
            g_GameState.load();
            goto PlayWorldMap;
        }

        default:
            goto PlayWorldMap;
        }

        //goto PlayLevel;
    }
PlayWorldMap:
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
                goto ExitFromApplication;
            else
                goto MainMenu;
        }
        else
        {
            sceneResult = wScene->loadFile(g_Episode.worldfile);
            wScene->setGameState(&g_GameState); //Load game state to the world map

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

        g_GameState._recent_ExitCode_world = (int)wldExitCode;

        if(wScene->doShutDown())
        {
            wScene.reset();
            goto ExitFromApplication;
        }

        if(g_AppSettings.debugMode)
        {
            if(wldExitCode == WldExit::EXIT_beginLevel)
            {
                std::string msg;
                //% "Start level\n%1"
                msg += fmt::qformat(qtTrId("MSG_START_LEVEL"), g_GameState.LevelFile) + "\n\n";
                //% "Type an exit code (signed integer)"
                msg += qtTrId("MSG_WLDTEST_EXIT_CODE");
                PGE_TextInputBox text(nullptr, msg, PGE_BoxBase::msg_info_light,
                                      PGE_Point(-1, -1),
                                      ConfigManager::setup_message_box.box_padding,
                                      ConfigManager::setup_message_box.sprite);
                text.exec();
                g_GameState._recent_ExitCode_level  = LvlExit::EXIT_Neutral;

                if(PGEFile::IsIntS(text.inputText()))
                    g_GameState._recent_ExitCode_level = toInt(text.inputText());

                if(g_GameState.isHubLevel)
                    goto ExitFromApplication;

                goto PlayWorldMap;
            }
            else
                goto ExitFromApplication;
        }

        switch(wldExitCode)
        {
        case WldExit::EXIT_beginLevel:
            goto PlayLevel;

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
            goto ExitFromApplication;

        goto MainMenu;
    }
PlayLevel:
    {
        bool playAgain = true;
        unsigned long entranceID = 0;
        std::shared_ptr<LevelScene> lScene(nullptr);

        while(playAgain)
        {
            entranceID = g_GameState.LevelTargetWarp;

            if(g_GameState.LevelFile_hub == g_GameState.LevelFile)
            {
                g_GameState.isHubLevel = true;
                entranceID = g_GameState.game_state.last_hub_warp;
            }

            int levelExitCode = 0;
            lScene.reset(new LevelScene());

            if(g_AppSettings.interprocessing)
                g_GameState.isTestingModeL = true;

            lScene->setGameState(&g_GameState);
            bool sceneResult = true;

            if(g_GameState.LevelFile.empty())
            {
                if(g_AppSettings.interprocessing && IntProc::isEnabled())
                {
                    sceneResult = lScene->loadFileIP();

                    if((!sceneResult) && (!lScene->isExiting()))
                    {
                        //SDL_Delay(50);
                        levelExitCode = LvlExit::EXIT_Error;
                        PGE_MsgBox msgBox(NULL, fmt::format_ne("ERROR:\nFail to start level\n\n{0}",
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
                sceneResult = lScene->loadFile(g_GameState.LevelFile);

                if(!sceneResult)
                {
                    SDL_Delay(50);
                    PGE_MsgBox msgBox(NULL,
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
                g_GameState._recent_ExitCode_level = levelExitCode;
            }

            if(!sceneResult)
                levelExitCode = LvlExit::EXIT_Error;

            switch(levelExitCode)
            {
            case LvlExit::EXIT_Warp:
            {
                if(lScene->m_warpToWorld)
                {
                    g_GameState.game_state.worldPosX = lScene->toWorldXY().x();
                    g_GameState.game_state.worldPosY = lScene->toWorldXY().y();
                    g_GameState.LevelFile.clear();
                    entranceID = 0;
                    g_jumpOnLevelEndTo = g_GameState.isEpisode ? RETURN_TO_WORLDMAP : RETURN_TO_MAIN_MENU;
                }
                else
                {
                    g_GameState.LevelFile = lScene->toAnotherLevel();
                    g_GameState.LevelTargetWarp = lScene->toAnotherEntrance();
                    entranceID = g_GameState.LevelTargetWarp;

                    if(g_GameState.isHubLevel)
                    {
                        g_GameState.isHubLevel = false;
                        g_GameState.game_state.last_hub_warp = lScene->m_lastWarpID;
                    }
                }

                if(g_GameState.LevelFile.empty())
                    playAgain = false;

                if(g_AppSettings.debugMode)
                {
                    std::string target;

                    if(lScene->m_warpToWorld)
                    {
                        target = fmt::format_ne("X={0}, Y={1}",
                                 g_GameState.game_state.worldPosX,
                                 g_GameState.game_state.worldPosY);
                    }
                    else
                        target = g_GameState.LevelFile;

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
                g_jumpOnLevelEndTo = RETURN_TO_EXIT;
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
                g_jumpOnLevelEndTo = g_GameState.isEpisode ? RETURN_TO_WORLDMAP : RETURN_TO_MAIN_MENU;

                if(g_GameState.isHubLevel)
                    g_jumpOnLevelEndTo = g_flags.testLevel ? RETURN_TO_EXIT : RETURN_TO_MAIN_MENU;

                playAgain = false;
            }
            break;

            case LvlExit::EXIT_PlayerDeath:
            {
                playAgain = g_GameState.isEpisode ? g_GameState.replay_on_fail : true;
                g_jumpOnLevelEndTo = g_GameState.isEpisode ? RETURN_TO_WORLDMAP : RETURN_TO_MAIN_MENU;

                //check the number of player lives here and decided to return worldmap or gameover
                if(g_GameState.isEpisode)
                {
                    g_GameState.game_state.lives--;

                    if(g_GameState.game_state.lives < 0)
                    {
                        playAgain = false;
                        g_GameState.game_state.coins = 0;
                        g_GameState.game_state.points = 0;
                        g_GameState.game_state.lives = 3;
                        g_jumpOnLevelEndTo = RETURN_TO_GAMEOVER_SCREEN;
                    }
                }
            }
            break;

            case LvlExit::EXIT_Error:
            {
                g_jumpOnLevelEndTo = (g_GameState.isEpisode) ? RETURN_TO_WORLDMAP : RETURN_TO_MAIN_MENU;
                playAgain = false;
                //% "Level was closed with error.\n%1"
                PGE_MsgBox::error( fmt::qformat(qtTrId("LVL_ERROR_LVLCLOSED"), lScene->errorString()) );
            }
            break;

            default:
                g_jumpOnLevelEndTo = g_GameState.isEpisode ? RETURN_TO_WORLDMAP : RETURN_TO_MAIN_MENU;
                playAgain = false;
            }

            if(g_flags.testLevel || g_AppSettings.debugMode)
                g_jumpOnLevelEndTo = RETURN_TO_EXIT;

            ConfigManager::unloadLevelConfigs();
            lScene.reset();
        }

        if(g_AppSettings.interprocessing)
            goto ExitFromApplication;

        switch(g_jumpOnLevelEndTo)
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
    return 0;
}
