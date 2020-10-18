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

#include "scene_world.h"
#include <audio/play_music.h>
#include <audio/pge_audio.h>
#include <graphics/window.h>
#include <graphics/gl_renderer.h>
#include <fontman/font_manager.h>
#include <PGE_File_Formats/file_formats.h>
#include <common_features/graphics_funcs.h>
#include <common_features/logger.h>
#include <common_features/pge_delay.h>
#include <common_features/tr.h>
#include <Utils/maths.h>
#include <Utils/dir_list_ci.h>
#include <controls/controller_keyboard.h>
#include <controls/controller_joystick.h>
#include <gui/pge_msgbox.h>
#include <data_configs/config_manager.h>
#include <settings/global_settings.h>
#include <settings/debugger.h>

#include <Utils/files.h>
#include <common_features/fmt_format_ne.h>

#include <vector>
#include <unordered_map>

WorldScene::WorldScene()
    : Scene(World), luaEngine(this)
{
    m_events.abort();
    m_exitWorldCode = WldExit::EXIT_error;
    m_exitWorldDelay = 2000;
    m_worldIsContinues = true;
    m_pauseMenu.isShown = false;
    m_gameState = nullptr;
    m_isInit = false;
    debug_render_delay = 0;
    debug_phys_delay = 0;
    debug_event_delay = 0;
    debug_total_delay = 0;
    m_mapWalker.img_h = ConfigManager::default_grid;
    m_mapWalker.offsetX = 0;
    m_mapWalker.offsetY = 0;
    /*********Controller********/
    m_player1Controller = g_AppSettings.openController(1);
    /*********Controller********/
    m_pauseMenu.initPauseMenu1(this);
    m_frameSkip = g_AppSettings.frameSkip;
    /***********Number of Players*****************/
    m_numOfPlayers = 1;
    /***********Number of Players*****************/
    /*********Fader*************/
    m_fader.setFull();
    /*********Fader*************/
    m_mapWalker.moveSpeed = 125.0 / (1000.0 / uTickf);
    Maths::clearPrecision(m_mapWalker.moveSpeed);
    m_mapWalker.moveStepsCount = 0;
    ConfigManager::setup_WorldMap.initFonts();
    m_commonSetup = ConfigManager::setup_WorldMap;

    if(m_commonSetup.backgroundImg.empty())
        m_backgroundTexture.w = 0;
    else
        GlRenderer::loadTextureP(m_backgroundTexture, m_commonSetup.backgroundImg);

    m_imgs.clear();

    for(const auto &additionalImage : m_commonSetup.AdditionalImages)
    {
        if(additionalImage.imgFile.empty())
            continue;

        WorldScene_misc_img img;
        GlRenderer::loadTextureP(img.t, additionalImage.imgFile);
        img.x = additionalImage.x;
        img.y = additionalImage.y;
        img.a.construct(additionalImage.animated,
                        additionalImage.frames,
                        additionalImage.framedelay);
        img.frmH = (img.t.h / additionalImage.frames);
        m_imgs.push_back(std::move(img));
    }

    m_viewportRect.setX(m_commonSetup.viewport_x);
    m_viewportRect.setY(m_commonSetup.viewport_y);
    m_viewportRect.setWidth(m_commonSetup.viewport_w);
    m_viewportRect.setHeight(m_commonSetup.viewport_h);
    m_mapWalker.posX = 0;
    m_mapWalker.posY = 0;
    m_levelTitle = "Hello!";
    m_counters.health = 3;
    m_counters.points = 0;
    m_counters.stars  = 0;
    m_counters.coins  = 0;
    m_jumpTo = false;
    m_mapWalker.walkDirection = Walk_Idle;
    m_lockControls = false;
    m_allowedLeft = false;
    m_allowedUp = false;
    m_allowedRight = false;
    m_allowedDown = false;
    m_playStopSnd = false;
    m_playDenySnd = false;
    FileFormats::CreateWorldData(m_data);
    m_pathOpeningInProcess = false;
    m_pathOpener.setScene(this);
    m_pathOpener.setInterval(250.0);
    m_viewportCameraMover.setSpeed(15.0);
}

WorldScene::~WorldScene()
{
    PGE_MusPlayer::stop();
    m_events.abort();
    m_indexTable.clean();
    m_itemsToRender.clear();
    m_itemsTerrain.clear();
    m_itemsSceneries.clear();
    m_itemsPaths.clear();
    m_itemsLevels.clear();
    m_itemsMusicBoxes.clear();
    GlRenderer::deleteTexture(m_backgroundTexture);
    //destroy textures
    D_pLogDebugNA("clear world textures");

    for(auto &i : m_texturesBank)
        GlRenderer::deleteTexture(i);

    for(auto &img : m_imgs)
        GlRenderer::deleteTexture(img.t);

    ConfigManager::unloadLevelConfigs();
    ConfigManager::unloadWorldConfigs();
    delete m_player1Controller;
}

void WorldScene::setGameState(EpisodeState *_state)
{
    if(!_state)
        return;

    m_gameState = _state;
    m_numOfPlayers = _state->m_numOfPlayers;
    m_counters.points = m_gameState->m_gameSave.points;
    m_counters.coins  = m_gameState->m_gameSave.coins;
    m_counters.stars  = uint32_t(m_gameState->m_gameSave.gottenStars.size());
    m_counters.lives  = m_gameState->m_gameSave.lives;
    PlayerState x = m_gameState->getPlayerState(1);
    m_counters.health = x._chsetup.health;
    m_gameState->m_autoRestartFailedLevel = m_data.restartlevel;

    if(m_gameState->m_episodeIsStarted && !m_data.HubStyledWorld)
    {
        m_mapWalker.posX = m_gameState->m_gameSave.worldPosX;
        m_mapWalker.posY = m_gameState->m_gameSave.worldPosY;
        m_viewportCameraMover.setPos(static_cast<double>(m_mapWalker.posX), static_cast<double>(m_mapWalker.posY));
        updateAvailablePaths();
        updateCenter();
    }
    else
    {
        m_gameState->m_episodeIsStarted = true;
        m_gameState->m_currentWorldPath = m_data.meta.path;

        //Detect gamestart and set position on them
        for(auto &level : m_data.levels)
        {
            if(level.gamestart)
            {
                m_mapWalker.posX = level.x;
                m_mapWalker.posY = level.y;
                m_gameState->m_gameSave.worldPosX = static_cast<long>(m_mapWalker.posX);
                m_gameState->m_gameSave.worldPosY = static_cast<long>(m_mapWalker.posY);
                break;
            }
        }

        //open Intro level
        if(!m_data.IntroLevel_file.empty())
        {
            //Fix file extension
            if((!Files::hasSuffix(m_data.IntroLevel_file, ".lvlx")) &&
               (!Files::hasSuffix(m_data.IntroLevel_file, ".lvl")))
                m_data.IntroLevel_file.append(".lvl");

            std::string introLevelFile = m_gameState->m_currentWorldPath + "/" + m_data.IntroLevel_file;
            pLogDebug("Opening intro level: %s", introLevelFile.c_str());

            if(Files::fileExists(introLevelFile))
            {
                LevelData checking;

                if(FileFormats::OpenLevelFile(introLevelFile, checking))
                {
                    pLogDebug("File valid, do exit!");
                    m_gameState->m_nextLevelFile = introLevelFile;
                    m_gameState->m_currentLevelPath = checking.meta.path;

                    if(m_data.HubStyledWorld)
                    {
                        m_gameState->m_currentHubLevelFile = checking.meta.path;
                        m_gameState->m_nextLevelEnterWarp = m_gameState->m_gameSave.last_hub_warp;
                    }
                    else
                        m_gameState->m_nextLevelEnterWarp = 0;

                    m_gameState->m_isHubLevel = m_data.HubStyledWorld;
                    //Jump to the intro/hub level
                    m_doExit = true;
                    m_exitWorldCode = WldExit::EXIT_beginLevel;
                    return;
                }
                else
                    pLogDebug("File invalid");
            }
        }

        m_pathOpener.setForce();
    }
}

WorldData *WorldScene::worldData()
{
    return &m_data;
}

bool WorldScene::init()
{
    //Global script path
    luaEngine.setLuaScriptPath(ConfigManager::PathScript());
    //Episode path
    luaEngine.appendLuaScriptPath(m_data.meta.path);
    //Level custom path
    luaEngine.appendLuaScriptPath(m_data.meta.path + "/" + m_data.meta.filename);
    luaEngine.setCoreFile(":/script/maincore_world.lua");
    luaEngine.setUserFile(ConfigManager::setup_WorldMap.luaFile);
    luaEngine.setErrorReporterFunc([this](const std::string & errorMessage, const std::string & stacktrace)
    {
        pLogWarning("Lua-Error: ");
        pLogWarning("Error Message: %s", errorMessage.data());
        pLogWarning("Stacktrace: \n%s", stacktrace.data());
        PGE_MsgBox msgBox(this,
                          fmt::format_ne("A lua error has been thrown: \n{0}"
                                      "\n\n"
                                      "More details in the log!",
                                        errorMessage),
                          PGE_MsgBox::msg_error);
        msgBox.exec();
    });
    luaEngine.init();
    m_indexTable.clean();
    m_itemsToRender.clear();
    m_itemsTerrain.clear();
    m_itemsSceneries.clear();
    m_itemsPaths.clear();
    m_itemsLevels.clear();
    m_itemsMusicBoxes.clear();

    if(m_doExit) return true;

    if(!loadConfigs())
        return false;

    m_players.clear();
    for(int i = 1; i <= m_numOfPlayers; i++)
    {
        if(!initPlayableCharacter(i, true))
            return false;
    }

    m_itemsTerrain.reserve(m_data.tiles.size());
    for(const auto &item : m_data.tiles)
    {
        m_itemsTerrain.emplace_back(item);
        WldTerrainItem &tile = m_itemsTerrain.back();
        if(!tile.init())
            continue;
        m_indexTable.addNode(&tile);
    }

    m_itemsSceneries.reserve(m_data.scenery.size());
    for(const auto &item : m_data.scenery)
    {
        m_itemsSceneries.emplace_back(item);
        WldSceneryItem &scenery = m_itemsSceneries.back();
        if(!scenery.init())
            continue;
        m_indexTable.addNode(&scenery);
    }

    m_itemsPaths.reserve(m_data.paths.size());
    for(const auto &item : m_data.paths)
    {
        m_itemsPaths.emplace_back(item);
        WldPathItem &path = m_itemsPaths.back();
        if(!path.init())
            continue;
        m_indexTable.addNode(&path);
    }

    m_itemsLevels.reserve(m_data.levels.size());
    for(const auto &item : m_data.levels)
    {
        m_itemsLevels.emplace_back(item);
        WldLevelItem &levelPoint = m_itemsLevels.back();
        if(!levelPoint.init())
            continue;
        m_indexTable.addNode(&levelPoint);
    }

    m_itemsMusicBoxes.reserve(m_data.music.size());
    for(const auto &item : m_data.music)
    {
        m_itemsMusicBoxes.emplace_back(item);
        WldMusicBoxItem &musicBox = m_itemsMusicBoxes.back();
        musicBox.r = 0.5f;
        musicBox.g = 0.5f;
        musicBox.b = 1.f;
        m_indexTable.addNode(&musicBox);
    }

    //Apply visibility settings to elements
    initElementsVisibility();
    m_pathOpener.startAt(PGE_PointF(m_mapWalker.posX, m_mapWalker.posY));
    PGE_PointF pos = m_pathOpener.curPos();
    m_viewportCameraMover.setPos(pos.x(), pos.y());
    m_pathOpeningInProcess = true;
    updateAvailablePaths();
    updateCenter();

    if(m_gameState)
        playMusic(m_gameState->m_gameSave.musicID, m_gameState->m_gameSave.musicFile, true, 200);

    m_isInit = true;
    return true;
}

bool WorldScene::initPlayableCharacter(int playerId, bool isInit)
{
    int player_portrait_step = 0;
    int player_portrait_x = m_commonSetup.portrait_x;
    int player_portrait_y = m_commonSetup.portrait_y;

    if(m_numOfPlayers > 1)
    {
        player_portrait_step = 30;
        player_portrait_x = player_portrait_x - (m_numOfPlayers * 30) / 2;
        player_portrait_x += player_portrait_step * (playerId - 1);
    }

    PlayerState state;
    if(isInit)
    {
        if(m_gameState)
        {
            state = m_gameState->getPlayerState(playerId);
            m_players.push_back(state);
        }
        else
        {
            state.characterID = 1;
            state.stateID = 1;
            state._chsetup = FileFormats::CreateSavCharacterState();
            m_players.push_back(state);
        }
    }
    else
    {
        if(playerId >= 1 && static_cast<size_t>(playerId) <= m_players.size())
            state = m_players[playerId - 1];
        else
            pLogWarning("Fail to get pre-loaded state for player ID %d", playerId);
    }

    if(m_commonSetup.portrait_en)
    {
        if(isInit)
        {
            // Create new portrait
            WorldScene_Portrait portrait(state.characterID,
                                         state.stateID,
                                         player_portrait_x,
                                         player_portrait_y,
                                         m_commonSetup.portrait_animation,
                                         m_commonSetup.portrait_frame_delay,
                                         m_commonSetup.portrait_direction);
            if(portrait.isValid())
                m_portraits.push_back(std::move(portrait));
            else
                pLogWarning("Fail to initialize portrait "
                            "of playable character %ul with state %ul",
                            state.characterID, state.stateID);
        }
        else
        {
            // Update existing portrait
            WorldScene_Portrait &portrait = m_portraits[playerId - 1];
            portrait.init(state.characterID,
                          state.stateID,
                          player_portrait_x,
                          player_portrait_y,
                          m_commonSetup.portrait_animation,
                          m_commonSetup.portrait_frame_delay,
                          m_commonSetup.portrait_direction);

            if(!portrait.isValid())
            {
                pLogWarning("Fail to update portrait "
                            "of playable character %ul with state %ul",
                            state.characterID, state.stateID);
            }
        }
    }

    // Initalize map walker for first player only
    if(playerId == 1)
    {
        PlayerState player_state;

        if(m_gameState)
            player_state = m_gameState->getPlayerState(playerId);

        m_mapWalker.setup = ConfigManager::playable_characters[player_state.characterID];
        long tID = ConfigManager::getWldPlayerTexture(player_state.characterID, player_state.stateID);

        if(tID < 0)
            return false;

        m_mapWalker.texture = ConfigManager::world_textures[static_cast<size_t>(tID)];
        m_mapWalker.img_h = m_mapWalker.texture.h / m_mapWalker.setup.wld_frames;
        m_mapWalker.ani.construct(true, m_mapWalker.setup.wld_frames, m_mapWalker.setup.wld_framespeed);
        m_mapWalker.ani.setFrameSequance(m_mapWalker.setup.wld_frames_down);
        m_mapWalker.offsetX = (static_cast<double>(ConfigManager::default_grid) / 2.0) - (m_mapWalker.texture.w / 2.0);
        m_mapWalker.offsetY = static_cast<double>(ConfigManager::default_grid)
                              - static_cast<double>(m_mapWalker.img_h)
                              + static_cast<double>(m_mapWalker.setup.wld_offset_y);
        if(!isInit)
            m_mapWalker.refreshDirection(true);
    }

    return true;
}

bool WorldScene::loadConfigs()
{
    bool success = true;
    std::string musIni = m_data.meta.path + "/music.ini";
    std::string sndIni = m_data.meta.path + "/sounds.ini";

    if(ConfigManager::music_lastIniFile != musIni)
    {
        ConfigManager::loadDefaultMusics();
        ConfigManager::loadMusic(m_data.meta.path + "/", musIni, true);
    }

    if(ConfigManager::sound_lastIniFile != sndIni)
    {
        ConfigManager::loadDefaultSounds();
        ConfigManager::loadSound(m_data.meta.path + "/", sndIni, true);

        if(ConfigManager::soundIniChanged())
            ConfigManager::buildSoundIndex();
    }

    //Set paths
    std::vector<std::string> extraPaths;
    std::string metaPath  = m_data.meta.path;
    std::string metaFName = m_data.meta.filename;
    ConfigManager::loadExtraFoldersList(metaPath, extraPaths);
    ConfigManager::Dir_EFFECT.setCustomDirs(metaPath, metaFName, ConfigManager::PathLevelEffect(), extraPaths);
    ConfigManager::Dir_Tiles.setCustomDirs(metaPath, metaFName, ConfigManager::PathWorldTiles(), extraPaths);
    ConfigManager::Dir_Scenery.setCustomDirs(metaPath, metaFName, ConfigManager::PathWorldScenery(), extraPaths);
    ConfigManager::Dir_WldPaths.setCustomDirs(metaPath, metaFName, ConfigManager::PathWorldPaths(), extraPaths);
    ConfigManager::Dir_WldLevel.setCustomDirs(metaPath, metaFName, ConfigManager::PathWorldLevels(), extraPaths);
    ConfigManager::Dir_PlayerLvl.setCustomDirs(metaPath, metaFName, ConfigManager::PathLevelPlayable(), extraPaths);
    ConfigManager::Dir_PlayerCalibrations.setCustomDirs(metaPath, metaFName, ConfigManager::PathLevelPlayerCalibrations(), extraPaths);
    ConfigManager::Dir_PlayerWld.setCustomDirs(metaPath, metaFName, ConfigManager::PathWorldPlayable(), extraPaths);
    //Load INI-files
    success = ConfigManager::loadWorldTiles();   //!< Tiles

    if(!success)
    {
        _errorString = "Fail on terrain tiles config loading";
        m_exitWorldCode = WldExit::EXIT_error;
        return success;
    }

    success = ConfigManager::loadWorldScenery(); //!< Scenery
    if(!success)
    {
        _errorString = "Fail on sceneries config loading";
        m_exitWorldCode = WldExit::EXIT_error;
        return success;
    }

    success = ConfigManager::loadWorldPaths();   //!< Paths
    if(!success)
    {
        _errorString = "Fail on paths config loading";
        m_exitWorldCode = WldExit::EXIT_error;
        return success;
    }

    success = ConfigManager::loadWorldLevels();  //!< Levels
    if(!success)
    {
        _errorString = "Fail on level entrances config loading";
        m_exitWorldCode = WldExit::EXIT_error;
        return success;
    }

    success = ConfigManager::loadPlayableCharacters();  //!< Playalbe Characters
    if(!success)
    {
        _errorString = "Fail on playalbe characters config loading";
        m_exitWorldCode = WldExit::EXIT_error;
        return success;
    }

    success = ConfigManager::loadLevelEffects();  //!< Effects
    if(!success)
    {
        _errorString = "Fail on effects config loading";
        m_exitWorldCode = WldExit::EXIT_error;
        return success;
    }

    //Validate all playable characters until use game state!
    if(m_gameState)
    {
        for(int i = 1; i <= m_numOfPlayers; i++)
        {
            PlayerState st = m_gameState->getPlayerState(i);

            if(!ConfigManager::playable_characters.contains(st.characterID))
            {
                //% "Invalid playable character ID"
                _errorString = qtTrId("ERROR_LVL_UNKNOWN_PL_CHARACTER") + " "
                               + std::to_string(st.characterID);
                m_errorMsg = _errorString;
                success = false;
                break;
            }
            else if(!ConfigManager::playable_characters[st.characterID].states.contains(st.stateID))
            {
                //% "Invalid playable character state ID"
                _errorString = qtTrId("ERROR_LVL_UNKNOWN_PL_STATE") + " "
                               + std::to_string(st.stateID);
                m_errorMsg = _errorString;
                success = false;
                break;
            }
        }
    }

    if(!success)
        m_exitWorldCode = WldExit::EXIT_error;

    return success;
}



void WorldScene::doMoveStep(double &posVal)
{
    m_mapWalker.moveStepsCount += m_mapWalker.moveSpeed;

    if(m_mapWalker.moveStepsCount >= ConfigManager::default_grid)
    {
        m_mapWalker.moveStepsCount = 0;
        posVal = Maths::roundTo(posVal, ConfigManager::default_grid);
    }

    if((ceil(posVal) == posVal) && (static_cast<long>(posVal) % ConfigManager::default_grid == 0))
    {
        m_mapWalker.walkDirection = 0;
        m_playStopSnd = true;
        updateAvailablePaths();
        updateCenter();
    }

    m_viewportCameraMover.setPos(m_mapWalker.posX, m_mapWalker.posY);
}

void WorldScene::setDir(int dr)
{
    m_mapWalker.walkDirection = dr;
    m_mapWalker.moveStepsCount = ConfigManager::default_grid - m_mapWalker.moveStepsCount;
    m_mapWalker.refreshDirection();
}


void WorldScene::PauseMenu::initPauseMenu1(WorldScene *parent)
{
    isOpened = false;
    menuId = 1;
    menu.construct(
        //% "Pause"
        qtTrId("WLD_MENU_PAUSE_TTL"),
        PGE_MenuBox::msg_info, PGE_Point(-1, -1),
        ConfigManager::setup_menu_box.box_padding,
        ConfigManager::setup_menu_box.sprite);
    menu.setParentScene(parent);
    menu.clearMenu();
    std::vector<std::string> items;
    //% "Continue"
    items.push_back(qtTrId("WLD_MENU_PAUSE_CONTINUE"));
    //% "Save and continue"
    items.push_back(qtTrId("WLD_MENU_PAUSE_CONTINUESAVE"));
    //% "Save and quit"
    items.push_back(qtTrId("WLD_MENU_PAUSE_EXITSAVE"));
    //% "Exit without saving"
    items.push_back(qtTrId("WLD_MENU_PAUSE_EXITNOSAVE"));
    menu.addMenuItems(items);
    menu.setRejectSnd(obj_sound_role::MenuPause);
    menu.setMaxMenuItems(4);
    isShown = false;
}

void WorldScene::PauseMenu::initPauseMenu2(WorldScene *parent)
{
    isOpened = false;
    menuId = 2;
    menu.construct(
        //% "Pause"
        qtTrId("WLD_MENU_PAUSE_TTL"),
        PGE_MenuBox::msg_info, PGE_Point(-1, -1),
        ConfigManager::setup_menu_box.box_padding,
        ConfigManager::setup_menu_box.sprite);
    menu.setParentScene(parent);
    menu.clearMenu();
    std::vector<std::string> items;
    //% "Continue"
    items.push_back(qtTrId("WLD_MENU_PAUSE_CONTINUE"));
    //% "Quit"
    items.push_back(qtTrId("WLD_MENU_PAUSE_EXIT"));
    menu.addMenuItems(items);
    menu.setRejectSnd(obj_sound_role::MenuPause);
    menu.setMaxMenuItems(4);
    isShown = false;
}

void WorldScene::processPauseMenu()
{
    if(!m_pauseMenu.isOpened)
    {
        m_pauseMenu.menu.restart();
        m_pauseMenu.isOpened = true;
        PGE_Audio::playSoundByRole(obj_sound_role::MenuPause);
    }
    else
    {
        processPauseMenuSwitchCharacter();
        m_pauseMenu.menu.update(uTickf);

        if(!m_pauseMenu.menu.isRunning())
        {
            if(m_pauseMenu.menuId == 1)
            {
                switch(m_pauseMenu.menu.answer())
                {
                case PAUSE_Continue:
                    //do nothing!!
                    break;

                case PAUSE_SaveCont:
                    //Save game state!
                    m_gameState->m_gameSave.worldPosX = Maths::lRound(m_mapWalker.posX);
                    m_gameState->m_gameSave.worldPosY = Maths::lRound(m_mapWalker.posY);
                    saveElementsVisibility();
                    m_gameState->save();
                    break;

                case PAUSE_SaveQuit:
                    //Save game state! and exit from episode
                    m_gameState->m_gameSave.worldPosX = Maths::lRound(m_mapWalker.posX);
                    m_gameState->m_gameSave.worldPosY = Maths::lRound(m_mapWalker.posY);
                    saveElementsVisibility();
                    m_gameState->save();
                    setExiting(0, WldExit::EXIT_exitWithSave);
                    break;

                case PAUSE_Exit:
                    //Save game state! and exit from episode
                    setExiting(0, WldExit::EXIT_exitNoSave);
                    break;

                default:
                    break;
                }
            }
            else
            {
                switch(m_pauseMenu.menu.answer())
                {
                case PAUSE_2_Continue:
                    //do nothing!!
                    break;

                case PAUSE_2_Exit:
                    //Save game state! and exit from episode
                    setExiting(0, WldExit::EXIT_exitNoSave);
                    break;

                default:
                    break;
                }
            }

            m_pauseMenu.isOpened = false;
            m_pauseMenu.isShown = false;
        }
    }
}

void WorldScene::processPauseMenuSwitchCharacter()
{
    if((m_controls_1.left_pressed ^ m_controls_1.right_pressed) &&
       (m_controls_1.left_pressed || m_controls_1.right_pressed))
    {
        int32_t dir = m_controls_1.left_pressed ? -1 : m_controls_1.right_pressed ? +1 : 0;
        auto totalCharacters = static_cast<uint32_t>(ConfigManager::playable_characters.total());

        std::vector<uint32_t> allowedCharacterId;
        for(uint32_t i = 1; i <= totalCharacters; i++)
        {
            if(m_data.nocharacter.size() >= i && !m_data.nocharacter[i - 1])
                allowedCharacterId.push_back(i);
            else if(m_data.nocharacter.size() < i)
                allowedCharacterId.push_back(i);
        }

        if(!allowedCharacterId.empty())
        {
            for(size_t plrIdx = 0; plrIdx < m_players.size(); plrIdx++)
            {
                PlayerState &p = m_players[plrIdx];
                const int playerId = static_cast<int>(plrIdx + 1);
                if(plrIdx == 0)
                {
                    bool chosen = true;
                    while(chosen)
                    {
                        if(dir < 0)
                        {
                            p.characterID--;
                            if(p.characterID < 1)
                                p.characterID = totalCharacters;
                        }
                        else if(dir > 0)
                        {
                            p.characterID++;
                            if(p.characterID > totalCharacters)
                                p.characterID = 1;
                        }

                        if(std::find(allowedCharacterId.begin(), allowedCharacterId.end(), p.characterID) != allowedCharacterId.end())
                        {
                            chosen = false;
                            if(m_gameState)
                            {
                                p._chsetup = m_gameState->getPlayableCharacterSetup(playerId, p.characterID);
                                p.stateID = static_cast<uint32_t>(p._chsetup.state);
                                m_gameState->setPlayerState(playerId, p);
                            }

                            initPlayableCharacter(playerId, false);
                        }
                    }
                    PGE_Audio::playSoundByRole(obj_sound_role::MenuScroll);
                }
            }
        }
    }
}

void WorldScene::update()
{
    Scene::update();

    if(!m_pauseMenu.isShown)
        tickAnimations(uTickf);

    m_viewportFader.tickFader(uTickf);
    updateLua();

    if(m_doExit)
    {
        if(m_exitWorldCode == WldExit::EXIT_close)
        {
            m_fader.setFull();
            m_worldIsContinues = false;
            m_isRunning = false;
        }
        else
        {
            if(m_fader.isNull())
                m_fader.setFade(10, 1.0, 0.01);

            if(m_fader.isFull())
            {
                m_worldIsContinues = false;
                m_isRunning = false;
            }
        }
    }
    else if(m_pauseMenu.isShown)
        processPauseMenu();
    else
    {
        m_events.processEvents(uTickf);
        m_viewportCameraMover.iterate(uTickf);
        processEffects(uTickf);

        if(m_pathOpeningInProcess)
        {
            m_lockControls = true;
            bool tickHappen = false;
            if(!m_pathOpener.processOpener(uTickf, &tickHappen))
            {
                m_pathOpeningInProcess = false;
                m_lockControls = false;
                updateAvailablePaths();
                updateCenter();
                m_viewportCameraMover.setTargetAuto(m_mapWalker.posX, m_mapWalker.posY, 1500.0);
            }
            else
            {
                if(tickHappen)
                {
                    PGE_PointF pos = m_pathOpener.curPos();
                    m_viewportCameraMover.setTarget(pos.x(), pos.y(), 0.2);
                }
            }
        }

        if(m_mapWalker.walkDirection == Walk_Idle)
        {
            if(!m_lockControls && ((m_controls_1.left ^ m_controls_1.right) || (m_controls_1.up ^ m_controls_1.down)))
            {
                if(m_controls_1.left && (m_allowedLeft || PGE_Debugger::cheat_worldfreedom))
                    m_mapWalker.walkDirection = Walk_Left;

                if(m_controls_1.right && (m_allowedRight || PGE_Debugger::cheat_worldfreedom))
                    m_mapWalker.walkDirection = Walk_Right;

                if(m_controls_1.up && (m_allowedUp || PGE_Debugger::cheat_worldfreedom))
                    m_mapWalker.walkDirection = Walk_Up;

                if(m_controls_1.down && (m_allowedDown || PGE_Debugger::cheat_worldfreedom))
                    m_mapWalker.walkDirection = Walk_Down;

                //If movement denied - play sound
                if(m_mapWalker.walkDirection == Walk_Idle)
                {
                    m_playStopSnd = false;
                    if(!m_playDenySnd)
                    {
                        PGE_Audio::playSoundByRole(obj_sound_role::WorldDeny);
                        m_playDenySnd = true;
                    }
                }
            }
            else
            {
                // When keys are released, deny sound can be played again
                m_playDenySnd = false;
            }

            if(m_mapWalker.walkDirection != Walk_Idle)
            {
                m_playDenySnd = false;
                m_playStopSnd = false;
                m_gameState->m_nextLevelFile.clear();
                m_jumpTo = false;
                m_mapWalker.refreshDirection();
            }

            // Play sound when player have stopped motion
            if(m_playStopSnd)
            {
                PGE_Audio::playSoundByRole(obj_sound_role::WorldMove);
                m_playStopSnd = false;
            }
        }
        else // While player walks
        {
            m_mapWalker.ani.manualTick(uTickf);

            // Change move direction if that possible
            if((!m_controls_1.left || !m_controls_1.right) && (!m_controls_1.up || !m_controls_1.down))
            {
                switch(m_mapWalker.walkDirection)
                {
                case Walk_Left:
                    if(m_controls_1.right)
                        setDir(Walk_Right);
                    break;

                case Walk_Right:
                    if(m_controls_1.left)
                        setDir(Walk_Left);
                    break;

                case Walk_Up:
                    if(m_controls_1.down)
                        setDir(Walk_Down);
                    break;

                case Walk_Down:
                    if(m_controls_1.up)
                        setDir(Walk_Up);
                    break;

                default:
                    break;
                }
            }
        }

        // Process move step
        switch(m_mapWalker.walkDirection)
        {
        case Walk_Left:
            m_mapWalker.posX -= m_mapWalker.moveSpeed;
            doMoveStep(m_mapWalker.posX);
            break;

        case Walk_Right:
            m_mapWalker.posX += m_mapWalker.moveSpeed;
            doMoveStep(m_mapWalker.posX);
            break;

        case Walk_Up:
            m_mapWalker.posY -= m_mapWalker.moveSpeed;
            doMoveStep(m_mapWalker.posY);
            break;

        case Walk_Down:
            m_mapWalker.posY += m_mapWalker.moveSpeed;
            doMoveStep(m_mapWalker.posY);
            break;

        default:
            break;
        }

        {
            double curPosX = m_viewportCameraMover.posX();
            double curPosY = m_viewportCameraMover.posY();
            m_itemsToRender.clear();
            m_indexTable.query(Maths::lRound(curPosX - (double(m_viewportRect.width()) / 2.0)),
                               Maths::lRound(curPosY - (double(m_viewportRect.height()) / 2.0)),
                               Maths::lRound(curPosX + (double(m_viewportRect.width()) / 2.0)),
                               Maths::lRound(curPosY + (double(m_viewportRect.height()) / 2.0)),
                               m_itemsToRender, true);
        }
    }

    if(m_controls_1.jump || m_controls_1.alt_jump)
    {
        if((!m_doExit) && (!m_lockControls) && (!m_pauseMenu.isShown) && (m_gameState))
        {
            if(!m_gameState->m_nextLevelFile.empty())
            {
                pLogDebug("Entering level %s...", m_gameState->m_nextLevelFile.c_str());
                m_gameState->m_gameSave.worldPosX = Maths::lRound(m_mapWalker.posX);
                m_gameState->m_gameSave.worldPosY = Maths::lRound(m_mapWalker.posY);
                saveElementsVisibility();
                PGE_Audio::playSoundByRole(obj_sound_role::WorldEnterLevel);
                stopMusic(true, 300);
                m_lockControls = true;
                setExiting(0, WldExit::EXIT_beginLevel);
            }
            else if(m_jumpTo)
            {
                pLogDebug("Trying to jump to X=%f, Y=%f...", m_jumpToXY.x(), m_jumpToXY.y());
                //Don't inheret exit code when going through warps!
                m_gameState->m_lastLevelExitCode = LvlExit::EXIT_Warp;
                //Create events
                EventQueueEntry<WorldScene >event1;
                event1.makeWaiterCond([this]()->bool
                {
                    return m_viewportFader.isFading();
                }, true, 100);
                m_events.events.push_back(event1);

                EventQueueEntry<WorldScene >event2;
                event2.makeCallerT(this, &WorldScene::jump, 100);
                m_events.events.push_back(event2);

                EventQueueEntry<WorldScene >event3;
                event3.makeCaller([this]()->void
                {
                    m_viewportFader.setFade(10, 0.0, 0.05);
                    this->m_lockControls = false;
                    //Open new paths if possible
                    this->m_pathOpener.startAt(PGE_PointF(this->m_mapWalker.posX, this->m_mapWalker.posY));
                    this->m_pathOpeningInProcess = true;
                }, 0);
                m_events.events.push_back(event3);

                this->m_lockControls = true;
                PGE_Audio::playSoundByRole(obj_sound_role::WarpPipe);
                m_viewportFader.setFade(10, 1.0, 0.03);
            }
        }
    }

    //Process Z-sort of the render functions
    renderArrayPrepare();
    //Process message boxes
    m_messages.process();
}


void WorldScene::fetchSideNodes(bool &side, std::vector<WorldNode * > &nodes, long cx, long cy)
{
    side = false;
    size_t size = nodes.size();
    WorldNode **nodedata = nodes.data();

    for(size_t i = 0; i < size; i++)
    {
        WorldNode *&x = nodedata[i];

        if(x->type == WorldNode::path)
        {
            side = x->collidePoint(cx, cy);

            if(side)
            {
                auto *u = dynamic_cast<WldPathItem *>(x);
                if(u) side = u->vizible;
                if(side) break;
            }
            else continue;
        }

        if(x->type == WorldNode::level)
        {
            side = x->collidePoint(cx, cy);

            if(side)
            {
                auto *u = dynamic_cast<WldLevelItem *>(x);
                if(u) side = u->vizible;
                if(side) break;
            }
            else continue;
        }
    }
}

void WorldScene::updateAvailablePaths()
{
    std::vector<WorldNode * > nodes;
    long x, y;

    //left
    x = Maths::lRound(m_mapWalker.posX + m_indexTable.grid_half() - m_indexTable.grid());
    y = Maths::lRound(m_mapWalker.posY + m_indexTable.grid_half());
    m_indexTable.query(x, y, nodes);
    fetchSideNodes(m_allowedLeft, nodes, x, y);
    nodes.clear();

    //Right
    x = Maths::lRound(m_mapWalker.posX + m_indexTable.grid_half() + m_indexTable.grid());
    y = Maths::lRound(m_mapWalker.posY + m_indexTable.grid_half());
    m_indexTable.query(x, y, nodes);
    fetchSideNodes(m_allowedRight, nodes, x, y);
    nodes.clear();

    //Top
    x = Maths::lRound(m_mapWalker.posX + m_indexTable.grid_half());
    y = Maths::lRound(m_mapWalker.posY + m_indexTable.grid_half() - m_indexTable.grid());
    m_indexTable.query(x, y, nodes);
    fetchSideNodes(m_allowedUp, nodes, x, y);
    nodes.clear();

    //Bottom
    x = Maths::lRound(m_mapWalker.posX + m_indexTable.grid_half());
    y = Maths::lRound(m_mapWalker.posY + m_indexTable.grid_half() + m_indexTable.grid());
    m_indexTable.query(x, y, nodes);
    fetchSideNodes(m_allowedDown, nodes, x, y);

    nodes.clear();
}

void WorldScene::updateCenter()
{
    if(m_gameState)
    {
        m_gameState->m_nextLevelFile.clear();
        m_gameState->m_nextLevelEnterWarp = 0;
    }

    m_levelTitle.clear();
    m_jumpTo = false;
    std::vector<WorldNode * > nodes;
    long px = Maths::lRound(m_mapWalker.posX + m_indexTable.grid_half());
    long py = Maths::lRound(m_mapWalker.posY + m_indexTable.grid_half());
    m_indexTable.query(px, py, nodes);

    for(WorldNode *x : nodes)
    {
        //Skip uncollided elements!
        if(!x->collidePoint(px, py))
            continue;

        /*************MusicBox***************/
        if(x->type == WorldNode::musicbox)
        {
            auto *y = dynamic_cast<WldMusicBoxItem *>(x);

            if(y && y->collidePoint(px, py))
            {
                if(m_isInit)
                    playMusic(y->data.id, y->data.music_file);
                else if(m_gameState)
                {
                    m_gameState->m_gameSave.musicID   = static_cast<unsigned int>(y->data.id);
                    m_gameState->m_gameSave.musicFile = y->data.music_file;
                }
            }
        }
        /*************MusicBox***************/
        /*************Level Point***************/
        else if(x->type == WorldNode::level)
        {
            auto *y = dynamic_cast<WldLevelItem *>(x);

            if(y && y->collidePoint(px, py))
            {
                m_levelTitle = y->data.title;

                if(!y->data.lvlfile.empty())
                {
                    DirListCI ci(m_data.meta.path);
                    std::string lvlName = ci.resolveFileCase(y->data.lvlfile);
                    std::string lvlPath = m_data.meta.path + "/" + lvlName;
                    pLogDebug("Trying to check level path %s...", lvlPath.c_str());
                    LevelData head;

                    if(FileFormats::OpenLevelFileHeader(lvlPath, head))
                    {
                        pLogDebug("FOUND!");

                        if(!y->data.title.empty())
                            m_levelTitle = y->data.title;
                        else if(!head.LevelName.empty())
                            m_levelTitle = head.LevelName;
                        else if(!lvlName.empty())
                            m_levelTitle = lvlName;

                        if(m_gameState)
                        {
                            m_gameState->m_nextLevelFile = lvlPath;
                            m_gameState->m_nextLevelEnterWarp = y->data.entertowarp;
                        }
                    }
                    else
                    {
                        pLogDebug("FAILED: %s, line %ld, data %s",
                                 head.meta.ERROR_info.c_str(),
                                 head.meta.ERROR_linenum,
                                 head.meta.ERROR_linedata.c_str());
                    }
                }
                else
                {
                    pLogDebug("Checking for a warp coordinates...");

                    if((y->data.gotox != -1) && (y->data.gotoy != -1))
                    {
                        m_jumpToXY.setX(static_cast<double>(y->data.gotox));
                        m_jumpToXY.setY(static_cast<double>(y->data.gotoy));
                        m_jumpTo = true;
                    }
                }
            }
        }

        /*************Level Point***************/
    }
}

void WorldScene::initElementsVisibility()
{
    if(m_gameState)
    {
        for(size_t i = 0; i < m_itemsSceneries.size(); i++)
        {
            if(i < m_gameState->m_gameSave.visibleScenery.size())
                m_itemsSceneries[i].vizible = m_gameState->m_gameSave.visibleScenery[i].second;
            else
            {
                m_itemsSceneries[i].vizible = true;
                visibleItem viz;
                viz.first = static_cast<unsigned int>(m_itemsSceneries[i].data.meta.array_id);
                viz.second = true;
                m_gameState->m_gameSave.visibleScenery.push_back(viz);
            }
        }

        for(size_t i = 0; i < m_itemsPaths.size(); i++)
        {
            if(i < m_gameState->m_gameSave.visiblePaths.size())
                m_itemsPaths[i].vizible = m_gameState->m_gameSave.visiblePaths[i].second;
            else
            {
                m_itemsPaths[i].vizible = false;
                visibleItem viz;
                viz.first = static_cast<unsigned int>(m_itemsPaths[i].data.meta.array_id);
                viz.second = false;
                m_gameState->m_gameSave.visiblePaths.push_back(viz);
            }
        }

        for(size_t i = 0; i < m_itemsLevels.size(); i++)
        {
            if(i < m_gameState->m_gameSave.visibleLevels.size())
                m_itemsLevels[i].vizible = m_gameState->m_gameSave.visibleLevels[i].second;
            else
            {
                m_itemsLevels[i].vizible = (m_itemsLevels[i].data.alwaysVisible || m_itemsLevels[i].data.gamestart);
                visibleItem viz;
                viz.first = static_cast<unsigned int>(m_itemsLevels[i].data.meta.array_id);
                viz.second = m_itemsLevels[i].vizible;
                m_gameState->m_gameSave.visibleLevels.push_back(viz);
            }
        }
    }
}

void WorldScene::saveElementsVisibility()
{
    if(m_gameState)
    {
        for(size_t i = 0; i < m_itemsSceneries.size(); i++)
        {
            if(i < m_gameState->m_gameSave.visibleScenery.size())
            {
                m_gameState->m_gameSave.visibleScenery[i].first = m_itemsSceneries[i].data.meta.array_id;
                m_gameState->m_gameSave.visibleScenery[i].second = m_itemsSceneries[i].vizible;
            }
            else
            {
                visibleItem viz;
                viz.first = static_cast<unsigned int>(m_itemsSceneries[i].data.meta.array_id);
                viz.second = m_itemsSceneries[i].vizible;
                m_gameState->m_gameSave.visibleScenery.push_back(viz);
            }
        }

        for(size_t i = 0; i < m_itemsPaths.size(); i++)
        {
            if(i < m_gameState->m_gameSave.visiblePaths.size())
            {
                m_gameState->m_gameSave.visiblePaths[i].first = m_itemsPaths[i].data.meta.array_id;
                m_gameState->m_gameSave.visiblePaths[i].second = m_itemsPaths[i].vizible;
            }
            else
            {
                visibleItem viz;
                viz.first = static_cast<unsigned int>(m_itemsPaths[i].data.meta.array_id);
                viz.second = m_itemsPaths[i].vizible;
                m_gameState->m_gameSave.visiblePaths.push_back(viz);
            }
        }

        for(size_t i = 0; i < m_itemsLevels.size(); i++)
        {
            if(i < m_gameState->m_gameSave.visibleLevels.size())
            {
                m_gameState->m_gameSave.visibleLevels[i].first = m_itemsLevels[i].data.meta.array_id;
                m_gameState->m_gameSave.visibleLevels[i].second = m_itemsLevels[i].vizible;
            }
            else
            {
                visibleItem viz;
                viz.first = static_cast<unsigned int>(m_itemsLevels[i].data.meta.array_id);
                viz.second = m_itemsLevels[i].vizible;
                m_gameState->m_gameSave.visibleLevels.push_back(viz);
            }
        }
    }
}


void WorldScene::render()
{
    GlRenderer::clearScreen();

    if(!m_isInit)
        goto renderBlack;

    GlRenderer::setTextureColor(1.0f, 1.0f, 1.0f, 1.0f);

    if(m_backgroundTexture.w > 0)
    {
        GlRenderer::renderTexture(&m_backgroundTexture,
                                  PGE_Window::Width / 2 - m_backgroundTexture.w / 2,
                                  PGE_Window::Height / 2 - m_backgroundTexture.h / 2);
    }

    for(auto &m_img : m_imgs)
    {
        AniPos x(0, 1);
        x = m_img.a.image();
        GlRenderer::renderTexture(&m_img.t,
                                  m_img.x,
                                  m_img.y,
                                  m_img.t.w,
                                  m_img.frmH,
                                  static_cast<float>(x.first),
                                  static_cast<float>(x.second));
    }

    for(auto &m_portrait : m_portraits)
        m_portrait.render();

    //Viewport zone black background
    GlRenderer::renderRect(m_viewportRect.left(), m_viewportRect.top(), m_viewportRect.width(), m_viewportRect.height(), 0.f, 0.f, 0.f);
    {
        double curPosX = m_viewportCameraMover.posX();
        double curPosY = m_viewportCameraMover.posY();
        //Set small viewport
        GlRenderer::setViewport(m_viewportRect.left(), m_viewportRect.top(), m_viewportRect.width(), m_viewportRect.height());
        double renderX = curPosX + 16 - (m_viewportRect.width() / 2);
        double renderY = curPosY + 16 - (m_viewportRect.height() / 2);
        //Render items
        GlRenderer::setTextureColor(1.0f, 1.0f, 1.0f, 1.0f);
        const size_t render_sz = m_itemsToRender.size();
        WorldNode **render_obj = m_itemsToRender.data();

        for(size_t i = 0; i < render_sz; i++)
            render_obj[i]->render(render_obj[i]->x - renderX,
                                  render_obj[i]->y - renderY);

        //draw our "character"
        AniPos img(0, 1);
        img = m_mapWalker.ani.image();
        GlRenderer::renderTexture(&m_mapWalker.texture,
                                  static_cast<float>(m_mapWalker.posX - renderX + m_mapWalker.offsetX),
                                  static_cast<float>(m_mapWalker.posY - renderY + m_mapWalker.offsetY),
                                  static_cast<float>(m_mapWalker.texture.w),
                                  static_cast<float>(m_mapWalker.img_h),
                                  static_cast<float>(img.first),
                                  static_cast<float>(img.second));

        for(auto &item : WorkingEffects)
            item.render(renderX, renderY);

        if(m_pathOpeningInProcess && PGE_Window::showPhysicsDebug)
            m_pathOpener.debugRender(renderX, renderY);

        if(!m_viewportFader.isNull())
        {
            GlRenderer::renderRect(0.f,
                                   0.f,
                                   static_cast<float>(m_viewportRect.width()),
                                   static_cast<float>(m_viewportRect.height()),
                                   0.f, 0.f, 0.f,
                                   static_cast<float>(m_viewportFader.fadeRatio()));
        }

        //Restore viewport
        GlRenderer::resetViewport();
    }

    //FIXME: implement "always-visible" flag to show this counter without comparison to zero
    if(m_commonSetup.points_en && (m_counters.points > 0))
    {
        FontManager::printText(fmt::format_ne("{0}", m_counters.points),
                               m_commonSetup.points_x,
                               m_commonSetup.points_y,
                               m_commonSetup.points_fontID,
                               m_commonSetup.points_rgba.Red(),
                               m_commonSetup.points_rgba.Green(),
                               m_commonSetup.points_rgba.Blue(),
                               m_commonSetup.points_rgba.Alpha());
    }

    //FIXME: implement "always-visible" flag to show this counter without comparison to zero
    if(m_commonSetup.health_en)
    {
        FontManager::printText(fmt::format_ne("{0}", m_counters.health),
                               m_commonSetup.health_x,
                               m_commonSetup.health_y,
                               m_commonSetup.health_fontID,
                               m_commonSetup.health_rgba.Red(),
                               m_commonSetup.health_rgba.Green(),
                               m_commonSetup.health_rgba.Blue(),
                               m_commonSetup.health_rgba.Alpha());
    }

    if(m_commonSetup.lives_en)
    {
        FontManager::printText(fmt::format_ne("{0}", m_counters.lives),
                               m_commonSetup.lives_x,
                               m_commonSetup.lives_y,
                               m_commonSetup.lives_fontID,
                               m_commonSetup.lives_rgba.Red(),
                               m_commonSetup.lives_rgba.Green(),
                               m_commonSetup.lives_rgba.Blue(),
                               m_commonSetup.lives_rgba.Alpha());
    }

    if(m_commonSetup.coin_en)
    {
        FontManager::printText(fmt::format_ne("{0}", m_counters.coins),
                               m_commonSetup.coin_x,
                               m_commonSetup.coin_y,
                               m_commonSetup.coin_fontID,
                               m_commonSetup.coin_rgba.Red(),
                               m_commonSetup.coin_rgba.Green(),
                               m_commonSetup.coin_rgba.Blue(),
                               m_commonSetup.coin_rgba.Alpha());
    }

    //FIXME: implement "always-visible" flag to show this counter without comparison to zero
    if(m_commonSetup.star_en)
    {
        FontManager::printText(fmt::format_ne("{0}", m_counters.stars),
                               m_commonSetup.star_x,
                               m_commonSetup.star_y,
                               m_commonSetup.star_fontID,
                               m_commonSetup.star_rgba.Red(),
                               m_commonSetup.star_rgba.Green(),
                               m_commonSetup.star_rgba.Blue(),
                               m_commonSetup.star_rgba.Alpha());
    }

    FontManager::printText(fmt::format_ne("{0}", m_levelTitle),
                           m_commonSetup.title_x,
                           m_commonSetup.title_y,
                           m_commonSetup.title_fontID,
                           m_commonSetup.title_rgba.Red(),
                           m_commonSetup.title_rgba.Green(),
                           m_commonSetup.title_rgba.Blue(),
                           m_commonSetup.title_rgba.Alpha()
                          );

    if(PGE_Window::showDebugInfo)
    {
        FontManager::printText(fmt::format_ne("X={0} Y={1}", m_mapWalker.posX, m_mapWalker.posY), 300, 10);
        {
            PGE_Point grid = m_indexTable.applyGrid(Maths::lRound(m_mapWalker.posX), Maths::lRound(m_mapWalker.posY));
            FontManager::printText(fmt::format_ne("TILE X={0} Y={1}", grid.x(), grid.y()), 300, 45);
        }
        FontManager::printText(fmt::format_ne("TICK-SUB: {0}, Visible items={1};", uTickf, m_itemsToRender.size()), 10, 100);
        FontManager::printText(fmt::format_ne("Delays E={0} R={1} P={2} [{3}]",
                                            debug_event_delay,
                                            debug_render_delay,
                                            debug_phys_delay,
                                            debug_total_delay), 10, 120);

        if(m_doExit)
            FontManager::printText(fmt::format_ne("Exit delay {0}, {1}",
                                                m_exitWorldDelay,
                                                uTickf), 10, 140, FontManager::DefaultRaster, 1.0, 0, 0, 1.0);
    }

renderBlack:
    Scene::render();
    if(m_pauseMenu.isShown)
        m_pauseMenu.menu.render();
}

void WorldScene::onKeyboardPressedSDL(SDL_Keycode sdl_key, Uint16)
{
    if(m_doExit)
        return;

    if(m_pauseMenu.isShown && (m_player1Controller->type() != Controller::type_keyboard))
        m_pauseMenu.menu.processKeyEvent(sdl_key);

    switch(sdl_key)
    {
    // Check which
    case SDLK_ESCAPE: // ESC
    case SDLK_RETURN:// Enter
    case SDLK_AC_BACK:
    {
        if(m_pauseMenu.isShown && (m_player1Controller->type() == Controller::type_keyboard))
            m_pauseMenu.menu.processKeyEvent(sdl_key);

        if(m_pauseMenu.isShown || m_doExit || m_lockControls)
        {
            if(m_pathOpeningInProcess)
                m_pathOpener.skipAnimation();
            break;
        }

        m_pauseMenu.isShown = true;
    }
    break;

    case SDLK_BACKQUOTE:
    {
        PGE_Debugger::executeCommand(this);
        break;
    }

    default:
        break;
    }
}

LuaEngine *WorldScene::getLuaEngine()
{
    return &luaEngine;
}

void WorldScene::processEvents()
{
    Scene::processEvents();
    if(!m_pauseMenu.isOpened)// Don't process controllers from the pause menu
    {
        m_player1Controller->update();
    }
    m_controls_1 = m_player1Controller->keys;
    if(m_controls_1.start_pressed)
        onKeyboardPressedSDL(SDLK_ESCAPE, 0);
}

int WorldScene::exec()
{
    m_worldIsContinues = true;
    GlRenderer::setClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //World scene's Loop
    Uint32 start_render = 0;
    Uint32 stop_render = 0;
    double doUpdate_render = 0.0;
    Uint32 start_physics = 0;
    Uint32 stop_physics = 0;
    Uint32 start_events = 0;
    Uint32 stop_events = 0;
    Uint32 start_common = 0;
    m_isRunning = !m_doExit;
    /****************Initial update***********************/
    //(Need to prevent accidental spawn of messagebox or pause menu with empty screen)
    m_controls_1 = Controller::noKeys();

    if(m_isRunning)
        update();

    runVsyncValidator();

    /*****************************************************/

    while(m_isRunning)
    {
        start_common = SDL_GetTicks();

        if(PGE_Window::showDebugInfo)
        {
            start_physics = 0;
            start_events = 0;
        }

        while(times.doUpdate_physics < static_cast<double>(uTick))
        {
            if(PGE_Window::showDebugInfo && start_events == 0)
                start_events = SDL_GetTicks();

            processEvents();

            if(PGE_Window::showDebugInfo)
                stop_events = SDL_GetTicks();

            if(PGE_Window::showDebugInfo)
                debug_event_delay = static_cast<int>(stop_events - start_events);

            if(PGE_Window::showDebugInfo && start_physics == 0)
                start_physics = SDL_GetTicks();

            update();

            if(PGE_Window::showDebugInfo)
                stop_physics = SDL_GetTicks();

            times.doUpdate_physics += uTickf;
            Maths::clearPrecision(times.doUpdate_physics);
        }

        times.doUpdate_physics -= static_cast<double>(uTick);
        Maths::clearPrecision(times.doUpdate_physics);

        if(PGE_Window::showDebugInfo)
            debug_phys_delay = static_cast<int>(stop_physics - start_physics);

        stop_render = 0;
        start_render = 0;

        if((PGE_Window::vsync) || (doUpdate_render <= 0.0))
        {
            start_render = SDL_GetTicks();
            render();
            GlRenderer::flush();
            GlRenderer::repaint();
            stop_render = SDL_GetTicks();
            doUpdate_render = m_frameSkip ? uTickf + (stop_render - start_render) : 0;

            if(PGE_Window::showDebugInfo)
                debug_render_delay = static_cast<int>(stop_render - start_render);
        }

        doUpdate_render -= uTickf;

        if(stop_render < start_render)
        {
            stop_render = 0;
            start_render = 0;
        }

        if((!PGE_Window::vsync) && (uTick > (SDL_GetTicks() - start_common)))
            PGE_Delay(uTick - (SDL_GetTicks() - start_common));

        if(PGE_Window::showDebugInfo)
            debug_total_delay = static_cast<int>(SDL_GetTicks() - start_common);
    }

    return m_exitWorldCode;
}

void WorldScene::tickAnimations(double ticks)
{
    //tick animation
    for(ConfigManager::AnimatorsArray::iterator it = ConfigManager::Animator_Tiles.begin(); it != ConfigManager::Animator_Tiles.end(); it++)
        it->manualTick(ticks);

    for(ConfigManager::AnimatorsArray::iterator it = ConfigManager::Animator_Scenery.begin(); it != ConfigManager::Animator_Scenery.end(); it++)
        it->manualTick(ticks);

    for(ConfigManager::AnimatorsArray::iterator it = ConfigManager::Animator_WldPaths.begin(); it != ConfigManager::Animator_WldPaths.end(); it++)
        it->manualTick(ticks);

    for(ConfigManager::AnimatorsArray::iterator it = ConfigManager::Animator_WldLevel.begin(); it != ConfigManager::Animator_WldLevel.end(); it++)
        it->manualTick(ticks);

    for(auto &m_img : m_imgs)
        m_img.a.manualTick(ticks);

    for(auto &m_portrait : m_portraits)
        m_portrait.update(ticks);
}

bool WorldScene::isExit()
{
    return !m_worldIsContinues;
}

void WorldScene::setExiting(int delay, int reason)
{
    m_exitWorldDelay = delay;
    m_exitWorldCode = reason;
    m_doExit = true;
}

bool WorldScene::loadFile(std::string filePath)
{
    m_data.meta.ReadFileValid = false;

    if(!Files::fileExists(filePath))
    {
        m_errorMsg += "File not exist\n\n";
        m_errorMsg += filePath;
        return false;
    }

    if(!FileFormats::OpenWorldFile(filePath, m_data))
        m_errorMsg += "Bad file format\n";

    return m_data.meta.ReadFileValid;
}

std::string WorldScene::getLastError()
{
    return m_errorMsg;
}

void WorldScene::jump()
{
    m_mapWalker.posX = m_jumpToXY.x();
    m_mapWalker.posY = m_jumpToXY.y();
    m_viewportCameraMover.setPos(m_mapWalker.posX, m_mapWalker.posY);

    if(m_gameState)
    {
        m_gameState->m_gameSave.worldPosX = Maths::lRound(m_mapWalker.posX);
        m_gameState->m_gameSave.worldPosY = Maths::lRound(m_mapWalker.posY);
    }

    updateAvailablePaths();
    updateCenter();
}



void WorldScene::stopMusic(bool fade, int fadeLen)
{
    if(fade)
        PGE_MusPlayer::fadeOut(fadeLen);
    else
        PGE_MusPlayer::stop();
}

void WorldScene::playMusic(unsigned long musicID, std::string customMusicFile, bool fade, int fadeLen)
{
    std::string musPath;

    if(!customMusicFile.empty())
    {
        DirListCI m(m_data.meta.path);
        musPath = ConfigManager::getWldMusic(musicID, m_data.meta.path + "/" + m.resolveFileCase(customMusicFile));
    }
    else
        musPath = ConfigManager::getWldMusic(musicID, std::string());

    if(musPath.empty())
        return;

    PGE_MusPlayer::openFile(musPath);

    if(fade)
        PGE_MusPlayer::fadeIn(fadeLen);
    else
        PGE_MusPlayer::play();

    if(m_gameState)
    {
        m_gameState->m_gameSave.musicID = static_cast<unsigned int>(musicID);
        m_gameState->m_gameSave.musicFile = customMusicFile;
    }
}

void WorldScene::MapWalker::refreshDirection(bool restorePrev)
{
    int dir = restorePrev ? prevWalkDirection : walkDirection;
    switch(dir)
    {
    case Walk_Left:
        ani.setFrameSequance(setup.wld_frames_left);
        prevWalkDirection = dir;
        break;
    case Walk_Right:
        ani.setFrameSequance(setup.wld_frames_right);
        prevWalkDirection = dir;
        break;
    case Walk_Up:
        ani.setFrameSequance(setup.wld_frames_up);
        prevWalkDirection = dir;
        break;
    case Walk_Down:
        ani.setFrameSequance(setup.wld_frames_down);
        prevWalkDirection = dir;
        break;
    default:
        break;
    }
}

bool WorldScene::isVizibleOnScreen(PGE_RectF &rect)
{
    PGE_RectF screen(0, 0, m_viewportRect.width(), m_viewportRect.height());
    double renderX = m_mapWalker.posX + 16 - (m_viewportRect.width() / 2);
    double renderY = m_mapWalker.posY + 16 - (m_viewportRect.height() / 2);
    screen.setPos(renderX, renderY);

    return screen.collideRect(rect);
}
