/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "scene_world.h"
#include <audio/play_music.h>
#include <audio/pge_audio.h>
#include <graphics/window.h>
#include <graphics/gl_renderer.h>
#include <fontman/font_manager.h>
#include <PGE_File_Formats/file_formats.h>
#include <common_features/graphics_funcs.h>
#include <common_features/logger.h>
#include <common_features/tr.h>
#include <Utils/maths.h>
#include <controls/controller_keyboard.h>
#include <controls/controller_joystick.h>
#include <gui/pge_msgbox.h>
#include <data_configs/config_manager.h>
#include <settings/global_settings.h>
#include <settings/debugger.h>

#include <Utils/files.h>
#include <fmt/fmt_format.h>

#include <vector>
#include <unordered_map>

WorldScene::WorldScene()
    : Scene(World), luaEngine(this)
{
    wld_events.abort();
    exitWorldCode = WldExit::EXIT_error;
    exitWorldDelay = 2000;
    worldIsContinues = true;
    isPauseMenu = false;
    gameState = NULL;
    isInit = false;
    debug_render_delay = 0;
    debug_phys_delay = 0;
    debug_event_delay = 0;
    debug_total_delay = 0;
    mapwalker_img_h = ConfigManager::default_grid;
    mapwalker_offset_x = 0;
    mapwalker_offset_y = 0;
    /*********Controller********/
    player1Controller = g_AppSettings.openController(1);
    /*********Controller********/
    initPauseMenu1();
    frameSkip = g_AppSettings.frameSkip;
    /***********Number of Players*****************/
    numOfPlayers = 1;
    /***********Number of Players*****************/
    /*********Fader*************/
    m_fader.setFull();
    /*********Fader*************/
    move_speed = 125 / PGE_Window::TicksPerSecond;
    move_steps_count = 0;
    ConfigManager::setup_WorldMap.initFonts();
    common_setup = ConfigManager::setup_WorldMap;

    if(common_setup.backgroundImg.empty())
        backgroundTex.w = 0;
    else
        GlRenderer::loadTextureP(backgroundTex, common_setup.backgroundImg);

    imgs.clear();

    for(size_t i = 0; i < common_setup.AdditionalImages.size(); i++)
    {
        if(common_setup.AdditionalImages[i].imgFile.empty())
            continue;

        WorldScene_misc_img img;
        GlRenderer::loadTextureP(img.t, common_setup.AdditionalImages[i].imgFile);
        img.x = common_setup.AdditionalImages[i].x;
        img.y = common_setup.AdditionalImages[i].y;
        img.a.construct(common_setup.AdditionalImages[i].animated,
                        common_setup.AdditionalImages[i].frames,
                        common_setup.AdditionalImages[i].framedelay);
        img.frmH = (img.t.h / common_setup.AdditionalImages[i].frames);
        imgs.push_back(std::move(img));
    }

    viewportRect.setX(common_setup.viewport_x);
    viewportRect.setY(common_setup.viewport_y);
    viewportRect.setWidth(common_setup.viewport_w);
    viewportRect.setHeight(common_setup.viewport_h);
    posX = 0;
    posY = 0;
    levelTitle = "Hello!";
    health = 3;
    points = 0;
    stars  = 0;
    coins  = 0;
    jumpTo = false;
    walk_direction = Walk_Idle;
    lock_controls = false;
    allow_left = false;
    allow_up = false;
    allow_right = false;
    allow_down = false;
    _playStopSnd = false;
    _playDenySnd = false;
    FileFormats::CreateWorldData(data);
    pathOpeningInProcess = false;
    pathOpener.setScene(this);
    pathOpener.setInterval(250.0);
    cameraMover.setSpeed(15.0);
}

WorldScene::~WorldScene()
{
    PGE_MusPlayer::stop();
    wld_events.abort();
    m_indexTable.clean();
    _itemsToRender.clear();
    wld_tiles.clear();
    wld_sceneries.clear();
    wld_paths.clear();
    wld_levels.clear();
    wld_musicboxes.clear();
    GlRenderer::deleteTexture(backgroundTex);
    //destroy textures
    D_pLogDebug("clear world textures");

    for(size_t i = 0; i < textures_bank.size(); i++)
        GlRenderer::deleteTexture(textures_bank[i]);

    for(size_t i = 0; i < imgs.size(); i++)
        GlRenderer::deleteTexture(imgs[i].t);

    ConfigManager::unloadLevelConfigs();
    ConfigManager::unloadWorldConfigs();
    delete player1Controller;
}

void WorldScene::setGameState(EpisodeState *_state)
{
    if(!_state)
        return;

    gameState = _state;
    numOfPlayers = _state->numOfPlayers;
    points = gameState->game_state.points;
    coins  = gameState->game_state.coins;
    stars  = gameState->game_state.gottenStars.size();
    lives  = gameState->game_state.lives;
    PlayerState x = gameState->getPlayerState(1);
    health = x._chsetup.health;
    gameState->replay_on_fail = data.restartlevel;

    if(gameState->episodeIsStarted && !data.HubStyledWorld)
    {
        posX = gameState->game_state.worldPosX;
        posY = gameState->game_state.worldPosY;
        cameraMover.setPos(static_cast<double>(posX), static_cast<double>(posY));
        updateAvailablePaths();
        updateCenter();
    }
    else
    {
        gameState->episodeIsStarted = true;
        gameState->WorldPath = data.meta.path;

        //Detect gamestart and set position on them
        for(size_t i = 0; i < data.levels.size(); i++)
        {
            if(data.levels[i].gamestart)
            {
                posX = data.levels[i].x;
                posY = data.levels[i].y;
                gameState->game_state.worldPosX = static_cast<long>(posX);
                gameState->game_state.worldPosY = static_cast<long>(posY);
                break;
            }
        }

        //open Intro level
        if(!data.IntroLevel_file.empty())
        {
            //Fix file extension
            if((!Files::hasSuffix(data.IntroLevel_file, ".lvlx")) &&
               (!Files::hasSuffix(data.IntroLevel_file, ".lvl")))
                data.IntroLevel_file.append(".lvl");

            std::string introLevelFile = gameState->WorldPath + "/" + data.IntroLevel_file;
            pLogDebug("Opening intro level: %s", introLevelFile.c_str());

            if(Files::fileExists(introLevelFile))
            {
                LevelData checking;

                if(FileFormats::OpenLevelFile(introLevelFile, checking))
                {
                    pLogDebug("File valid, do exit!");
                    gameState->LevelFile = introLevelFile;
                    gameState->LevelPath = checking.meta.path;

                    if(data.HubStyledWorld)
                    {
                        gameState->LevelFile_hub = checking.meta.path;
                        gameState->LevelTargetWarp = gameState->game_state.last_hub_warp;
                    }
                    else
                        gameState->LevelTargetWarp = 0;

                    gameState->isHubLevel = data.HubStyledWorld;
                    //Jump to the intro/hub level
                    m_doExit = true;
                    exitWorldCode = WldExit::EXIT_beginLevel;
                    return;
                }
                else
                    pLogDebug("File invalid");
            }
        }

        pathOpener.setForce();
    }
}

bool WorldScene::init()
{
    //Global script path
    luaEngine.setLuaScriptPath(ConfigManager::PathScript());
    //Episode path
    luaEngine.appendLuaScriptPath(data.meta.path);
    //Level custom path
    luaEngine.appendLuaScriptPath(data.meta.path + "/" + data.meta.filename);
    luaEngine.setCoreFile(":/script/maincore_world.lua");
    luaEngine.setUserFile(ConfigManager::setup_WorldMap.luaFile);
    luaEngine.setErrorReporterFunc([this](const std::string & errorMessage, const std::string & stacktrace)
    {
        pLogWarning("Lua-Error: ");
        pLogWarning("Error Message: %s", errorMessage.data());
        pLogWarning("Stacktrace: \n%s", stacktrace.data());
        PGE_MsgBox msgBox(this,
                          fmt::format("A lua error has been thrown: \n{0}"
                                      "\n\n"
                                      "More details in the log!",
                                        errorMessage),
                          PGE_MsgBox::msg_error);
        msgBox.exec();
    });
    luaEngine.init();
    m_indexTable.clean();
    _itemsToRender.clear();
    wld_tiles.clear();
    wld_sceneries.clear();
    wld_paths.clear();
    wld_levels.clear();
    wld_musicboxes.clear();

    if(m_doExit) return true;

    if(!loadConfigs())
        return false;

    int player_portrait_step = 0;
    int player_portrait_x = common_setup.portrait_x;
    int player_portrait_y = common_setup.portrait_y;

    if(numOfPlayers > 1)
    {
        player_portrait_step = 30;
        player_portrait_x = player_portrait_x - (numOfPlayers * 30) / 2;
    }

    players.clear();

    for(int i = 1; i <= numOfPlayers; i++)
    {
        PlayerState state;

        if(gameState)
        {
            state = gameState->getPlayerState(i);
            players.push_back(state);
        }
        else
        {
            state.characterID = 1;
            state.stateID = 1;
            state._chsetup = FileFormats::CreateSavCharacterState();
            players.push_back(state);
        }

        if(common_setup.points_en)
        {
            WorldScene_Portrait portrait(state.characterID,
                                         state.stateID,
                                         player_portrait_x,
                                         player_portrait_y,
                                         common_setup.portrait_animation,
                                         common_setup.portrait_frame_delay,
                                         common_setup.portrait_direction);
            if(portrait.isValid())
            {
                portraits.push_back(std::move(portrait));
                player_portrait_x += player_portrait_step;
            }
            else
            {
                pLogWarning("Fail to initialize portrait "
                            "of playable character %ul with state %ul",
                            state.characterID, state.stateID);
            }
        }
    }

    PlayerState player_state;

    if(gameState)
        player_state = gameState->getPlayerState(1);

    mapwalker_setup = ConfigManager::playable_characters[player_state.characterID];
    long tID = ConfigManager::getWldPlayerTexture(player_state.characterID, player_state.stateID);

    if(tID < 0)
        return false;

    mapwalker_texture = ConfigManager::world_textures[static_cast<size_t>(tID)];
    mapwalker_img_h = mapwalker_texture.h / mapwalker_setup.wld_frames;
    mapwalker_ani.construct(true, mapwalker_setup.wld_frames, mapwalker_setup.wld_framespeed);
    mapwalker_ani.setFrameSequance(mapwalker_setup.wld_frames_down);
    mapwalker_offset_x = (static_cast<int>(ConfigManager::default_grid) / 2) - (mapwalker_texture.w / 2);
    mapwalker_offset_y = static_cast<int>(ConfigManager::default_grid) - static_cast<int>(mapwalker_img_h) + mapwalker_setup.wld_offset_y;

    for(size_t i = 0; i < data.tiles.size(); i++)
    {
        WldTileItem tile(data.tiles[i]);

        if(!tile.init())
            continue;

        wld_tiles.push_back(std::move(tile));
        m_indexTable.addNode(tile.x, tile.y, tile.w, tile.h, &(wld_tiles.back()));
    }

    for(size_t i = 0; i < data.scenery.size(); i++)
    {
        WldSceneryItem scenery(data.scenery[i]);
        if(!scenery.init())
            continue;
        wld_sceneries.push_back(std::move(scenery));
        m_indexTable.addNode(scenery.x, scenery.y, scenery.w, scenery.h, &(wld_sceneries.back()));
    }

    for(size_t i = 0; i < data.paths.size(); i++)
    {
        WldPathItem path(data.paths[i]);
        if(!path.init())
            continue;
        wld_paths.push_back(std::move(path));
        m_indexTable.addNode(path.x, path.y, path.w, path.h, &(wld_paths.back()));
    }

    for(size_t i = 0; i < data.levels.size(); i++)
    {
        WldLevelItem levelp(data.levels[i]);

        if(!levelp.init())
            continue;
        wld_levels.push_back(std::move(levelp));
        m_indexTable.addNode(levelp.x + static_cast<long>(levelp.offset_x),
                            levelp.y + static_cast<long>(levelp.offset_y),
                            levelp.texture.w,
                            levelp.texture.h,
                            &(wld_levels.back()));
    }

    for(size_t i = 0; i < data.music.size(); i++)
    {
        WldMusicBoxItem musicbox(data.music[i]);
        musicbox.r = 0.5f;
        musicbox.g = 0.5f;
        musicbox.b = 1.f;
        wld_musicboxes.push_back(std::move(musicbox));
        m_indexTable.addNode(musicbox.x, musicbox.y, musicbox.w, musicbox.h, &(wld_musicboxes.back()));
    }

    //Apply vizibility settings to elements
    initElementsVisibility();
    pathOpener.startAt(PGE_PointF(posX, posY));
    PGE_PointF pos = pathOpener.curPos();
    cameraMover.setPos(pos.x(), pos.y());
    pathOpeningInProcess = true;
    updateAvailablePaths();
    updateCenter();

    if(gameState)
        playMusic(gameState->game_state.musicID, gameState->game_state.musicFile, true, 200);

    isInit = true;
    return true;
}

bool WorldScene::loadConfigs()
{
    bool success = true;
    std::string musIni = data.meta.path + "/music.ini";
    std::string sndIni = data.meta.path + "/sounds.ini";

    if(ConfigManager::music_lastIniFile != musIni)
    {
        ConfigManager::loadDefaultMusics();
        ConfigManager::loadMusic(data.meta.path + "/", musIni, true);
    }

    if(ConfigManager::sound_lastIniFile != sndIni)
    {
        ConfigManager::loadDefaultSounds();
        ConfigManager::loadSound(data.meta.path + "/", sndIni, true);

        if(ConfigManager::soundIniChanged())
            ConfigManager::buildSoundIndex();
    }

    //Set paths
    ConfigManager::Dir_EFFECT.setCustomDirs(data.meta.path, data.meta.filename, ConfigManager::PathLevelEffect());
    ConfigManager::Dir_Tiles.setCustomDirs(data.meta.path, data.meta.filename, ConfigManager::PathWorldTiles());
    ConfigManager::Dir_Scenery.setCustomDirs(data.meta.path, data.meta.filename, ConfigManager::PathWorldScenery());
    ConfigManager::Dir_WldPaths.setCustomDirs(data.meta.path, data.meta.filename, ConfigManager::PathWorldPaths());
    ConfigManager::Dir_WldLevel.setCustomDirs(data.meta.path, data.meta.filename, ConfigManager::PathWorldLevels());
    ConfigManager::Dir_PlayerLvl.setCustomDirs(data.meta.path, data.meta.filename, ConfigManager::PathLevelPlayable());
    ConfigManager::Dir_PlayerWld.setCustomDirs(data.meta.path, data.meta.filename, ConfigManager::PathWorldPlayable());
    //Load INI-files
    success = ConfigManager::loadWorldTiles();   //!< Tiles

    if(!success)
    {
        _errorString = "Fail on terrain tiles config loading";
        exitWorldCode = WldExit::EXIT_error;
        goto abortInit;
    }

    success = ConfigManager::loadWorldScenery(); //!< Scenery
    if(!success)
    {
        _errorString = "Fail on sceneries config loading";
        exitWorldCode = WldExit::EXIT_error;
        goto abortInit;
    }

    success = ConfigManager::loadWorldPaths();   //!< Paths
    if(!success)
    {
        _errorString = "Fail on paths config loading";
        exitWorldCode = WldExit::EXIT_error;
        goto abortInit;
    }

    success = ConfigManager::loadWorldLevels();  //!< Levels
    if(!success)
    {
        _errorString = "Fail on level entrances config loading";
        exitWorldCode = WldExit::EXIT_error;
        goto abortInit;
    }

    success = ConfigManager::loadLevelEffects();
    if(!success)
    {
        _errorString = "Fail on effects config loading";
        exitWorldCode = WldExit::EXIT_error;
        goto abortInit;
    }

    //Validate all playable characters until use game state!
    if(gameState)
    {
        for(int i = 1; i <= numOfPlayers; i++)
        {
            PlayerState st = gameState->getPlayerState(i);

            if(!ConfigManager::playable_characters.contains(st.characterID))
            {
                //% "Invalid playable character ID"
                _errorString = qtTrId("ERROR_LVL_UNKNOWN_PL_CHARACTER") + " "
                               + std::to_string(st.characterID);
                errorMsg = _errorString;
                success = false;
                break;
            }
            else if(!ConfigManager::playable_characters[st.characterID].states.contains(st.stateID))
            {
                //% "Invalid playable character state ID"
                _errorString = qtTrId("ERROR_LVL_UNKNOWN_PL_STATE") + " "
                               + std::to_string(st.stateID);
                errorMsg = _errorString;
                success = false;
                break;
            }
        }
    }

    if(!success) exitWorldCode = WldExit::EXIT_error;

abortInit:
    return success;
}



void WorldScene::doMoveStep(double &posVal)
{
    move_steps_count += move_speed;

    if(move_steps_count >= ConfigManager::default_grid)
    {
        move_steps_count = 0;
        posVal = Maths::roundTo(posVal, ConfigManager::default_grid);
    }

    if((ceil(posVal) == posVal) && (static_cast<long>(posVal) % ConfigManager::default_grid == 0))
    {
        walk_direction = 0;
        _playStopSnd = true;
        updateAvailablePaths();
        updateCenter();
    }

    cameraMover.setPos(posX, posY);
}

void WorldScene::setDir(int dr)
{
    walk_direction = dr;
    move_steps_count = ConfigManager::default_grid - move_steps_count;
    mapwalker_refreshDirection();
}


void WorldScene::initPauseMenu1()
{
    _pauseMenu_opened = false;
    _pauseMenuID = 1;
    _pauseMenu.setParentScene(this);
    _pauseMenu.construct(
        //% "Pause"
        qtTrId("WLD_MENU_PAUSE_TTL"),
        PGE_MenuBox::msg_info, PGE_Point(-1, -1),
        ConfigManager::setup_menu_box.box_padding,
        ConfigManager::setup_menu_box.sprite);
    _pauseMenu.clearMenu();
    std::vector<std::string> items;
    //% "Continue"
    items.push_back(qtTrId("WLD_MENU_PAUSE_CONTINUE"));
    //% "Save and continue"
    items.push_back(qtTrId("WLD_MENU_PAUSE_CONTINUESAVE"));
    //% "Save and quit"
    items.push_back(qtTrId("WLD_MENU_PAUSE_EXITSAVE"));
    //% "Exit without saving"
    items.push_back(qtTrId("WLD_MENU_PAUSE_EXITNOSAVE"));
    _pauseMenu.addMenuItems(items);
    _pauseMenu.setRejectSnd(obj_sound_role::MenuPause);
    _pauseMenu.setMaxMenuItems(4);
    isPauseMenu = false;
}

void WorldScene::initPauseMenu2()
{
    _pauseMenu_opened = false;
    _pauseMenuID = 2;
    _pauseMenu.setParentScene(this);
    _pauseMenu.construct(
        //% "Pause"
        qtTrId("WLD_MENU_PAUSE_TTL"),
        PGE_MenuBox::msg_info, PGE_Point(-1, -1),
        ConfigManager::setup_menu_box.box_padding,
        ConfigManager::setup_menu_box.sprite);
    _pauseMenu.clearMenu();
    std::vector<std::string> items;
    //% "Continue"
    items.push_back(qtTrId("WLD_MENU_PAUSE_CONTINUE"));
    //% "Quit"
    items.push_back(qtTrId("WLD_MENU_PAUSE_EXIT"));
    _pauseMenu.addMenuItems(items);
    _pauseMenu.setRejectSnd(obj_sound_role::MenuPause);
    _pauseMenu.setMaxMenuItems(4);
    isPauseMenu = false;
}

void WorldScene::processPauseMenu()
{
    if(!_pauseMenu_opened)
    {
        _pauseMenu.restart();
        _pauseMenu_opened = true;
        PGE_Audio::playSoundByRole(obj_sound_role::MenuPause);
    }
    else
    {
        _pauseMenu.update(uTickf);

        if(!_pauseMenu.isRunning())
        {
            if(_pauseMenuID == 1)
            {
                switch(_pauseMenu.answer())
                {
                case PAUSE_Continue:
                    //do nothing!!
                    break;

                case PAUSE_SaveCont:
                    //Save game state!
                    gameState->game_state.worldPosX = Maths::lRound(posX);
                    gameState->game_state.worldPosY = Maths::lRound(posY);
                    saveElementsVisibility();
                    gameState->save();
                    break;

                case PAUSE_SaveQuit:
                    //Save game state! and exit from episode
                    gameState->game_state.worldPosX = Maths::lRound(posX);
                    gameState->game_state.worldPosY = Maths::lRound(posY);
                    saveElementsVisibility();
                    gameState->save();
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
                switch(_pauseMenu.answer())
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

            _pauseMenu_opened = false;
            isPauseMenu = false;
        }
    }
}

void WorldScene::update()
{
    tickAnimations(uTickf);
    Scene::update();
    m_viewportFader.tickFader(uTickf);
    updateLua();

    if(m_doExit)
    {
        if(exitWorldCode == WldExit::EXIT_close)
        {
            m_fader.setFull();
            worldIsContinues = false;
            m_isRunning = false;
        }
        else
        {
            if(m_fader.isNull())
                m_fader.setFade(10, 1.0, 0.01);

            if(m_fader.isFull())
            {
                worldIsContinues = false;
                m_isRunning = false;
            }
        }
    }
    else if(isPauseMenu)
        processPauseMenu();
    else
    {
        wld_events.processEvents(uTickf);
        cameraMover.iterate(uTickf);
        processEffects(uTickf);

        if(pathOpeningInProcess)
        {
            lock_controls = true;
            bool tickHappen = false;
            if(!pathOpener.processOpener(uTickf, &tickHappen))
            {
                pathOpeningInProcess = false;
                lock_controls = false;
                updateAvailablePaths();
                updateCenter();
                cameraMover.setTargetAuto(posX, posY, 1500.0);
            }
            else
            {
                if(tickHappen)
                {
                    PGE_PointF pos = pathOpener.curPos();
                    cameraMover.setTarget(pos.x(), pos.y(), 0.2);
                }
            }
        }

        if(walk_direction == Walk_Idle)
        {
            if(!lock_controls && ((controls_1.left ^ controls_1.right) || (controls_1.up ^ controls_1.down)))
            {
                if(controls_1.left && (allow_left || PGE_Debugger::cheat_worldfreedom))
                    walk_direction = Walk_Left;

                if(controls_1.right && (allow_right || PGE_Debugger::cheat_worldfreedom))
                    walk_direction = Walk_Right;

                if(controls_1.up && (allow_up || PGE_Debugger::cheat_worldfreedom))
                    walk_direction = Walk_Up;

                if(controls_1.down && (allow_down || PGE_Debugger::cheat_worldfreedom))
                    walk_direction = Walk_Down;

                //If movement denied - play sound
                if((controls_1.left || controls_1.right || controls_1.up || controls_1.down) && (walk_direction == Walk_Idle))
                {
                    _playStopSnd = false;

                    if(!_playDenySnd)
                    {
                        PGE_Audio::playSoundByRole(obj_sound_role::WorldDeny);
                        _playDenySnd = true;
                    }
                }
                else if(!controls_1.left && !controls_1.right && !controls_1.up && !controls_1.down)
                    _playDenySnd = false;
            }

            if(walk_direction != Walk_Idle)
            {
                _playDenySnd = false;
                _playStopSnd = false;
                gameState->LevelFile.clear();
                jumpTo = false;
                mapwalker_refreshDirection();
            }

            if(_playStopSnd)
            {
                PGE_Audio::playSoundByRole(obj_sound_role::WorldMove);
                _playStopSnd = false;
            }
        }
        else
        {
            mapwalker_ani.manualTick(uTickf);

            if((!controls_1.left || !controls_1.right) && (!controls_1.up || !controls_1.down))
            {
                switch(walk_direction)
                {
                case Walk_Left:
                    if(controls_1.right)
                        setDir(Walk_Right);

                    break;

                case Walk_Right:
                    if(controls_1.left)
                        setDir(Walk_Left);

                    break;

                case Walk_Up:
                    if(controls_1.down)
                        setDir(Walk_Down);

                    break;

                case Walk_Down:
                    if(controls_1.up)
                        setDir(Walk_Up);

                    break;

                default:
                    break;
                }
            }
        }

        switch(walk_direction)
        {
        case Walk_Left:
            posX -= move_speed;
            doMoveStep(posX);
            break;

        case Walk_Right:
            posX += move_speed;
            doMoveStep(posX);
            break;

        case Walk_Up:
            posY -= move_speed;
            doMoveStep(posY);
            break;

        case Walk_Down:
            posY += move_speed;
            doMoveStep(posY);
            break;

        default:
            break;
        }

        {
            double curPosX = cameraMover.posX();
            double curPosY = cameraMover.posY();
            _itemsToRender.clear();
            m_indexTable.query(Maths::lRound(curPosX - (viewportRect.width() / 2)),
                              Maths::lRound(curPosY - (viewportRect.height() / 2)),
                              Maths::lRound(curPosX + (viewportRect.width() / 2)),
                              Maths::lRound(curPosY + (viewportRect.height() / 2)),
                              _itemsToRender, true);
        }
    }

    if(controls_1.jump || controls_1.alt_jump)
    {
        if((!m_doExit) && (!lock_controls) && (!isPauseMenu) && (gameState))
        {
            if(!gameState->LevelFile.empty())
            {
                pLogDebug("Entering level %s...", gameState->LevelFile.c_str());
                gameState->game_state.worldPosX = Maths::lRound(posX);
                gameState->game_state.worldPosY = Maths::lRound(posY);
                saveElementsVisibility();
                PGE_Audio::playSoundByRole(obj_sound_role::WorldEnterLevel);
                stopMusic(true, 300);
                lock_controls = true;
                setExiting(0, WldExit::EXIT_beginLevel);
            }
            else if(jumpTo)
            {
                pLogDebug("Trying to jump to X=%f, Y=%f...", jumpToXY.x(), jumpToXY.y());
                //Don't inheret exit code when going through warps!
                gameState->_recent_ExitCode_level = LvlExit::EXIT_Warp;
                //Create events
                EventQueueEntry<WorldScene >event1;
                event1.makeWaiterCond([this]()->bool
                {
                    return m_viewportFader.isFading();
                }, true, 100);
                wld_events.events.push_back(event1);

                EventQueueEntry<WorldScene >event2;
                event2.makeCallerT(this, &WorldScene::jump, 100);
                wld_events.events.push_back(event2);

                EventQueueEntry<WorldScene >event3;
                event3.makeCaller([this]()->void
                {
                    m_viewportFader.setFade(10, 0.0, 0.05);
                    this->lock_controls = false;
                    //Open new paths if possible
                    this->pathOpener.startAt(PGE_PointF(this->posX, this->posY));
                    this->pathOpeningInProcess = true;
                }, 0);
                wld_events.events.push_back(event3);

                this->lock_controls = true;
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
                WldPathItem *u = dynamic_cast<WldPathItem *>(x);

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
                WldLevelItem *u = dynamic_cast<WldLevelItem *>(x);

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
    x = Maths::lRound(posX + m_indexTable.grid_half() - m_indexTable.grid());
    y = Maths::lRound(posY + m_indexTable.grid_half());
    m_indexTable.query(x, y, nodes);
    fetchSideNodes(allow_left, nodes, x, y);
    nodes.clear();

    //Right
    x = Maths::lRound(posX + m_indexTable.grid_half() + m_indexTable.grid());
    y = Maths::lRound(posY + m_indexTable.grid_half());
    m_indexTable.query(x, y, nodes);
    fetchSideNodes(allow_right, nodes, x, y);
    nodes.clear();

    //Top
    x = Maths::lRound(posX + m_indexTable.grid_half());
    y = Maths::lRound(posY + m_indexTable.grid_half() - m_indexTable.grid());
    m_indexTable.query(x, y, nodes);
    fetchSideNodes(allow_up, nodes, x, y);
    nodes.clear();

    //Bottom
    x = Maths::lRound(posX + m_indexTable.grid_half());
    y = Maths::lRound(posY + m_indexTable.grid_half() + m_indexTable.grid());
    m_indexTable.query(x, y, nodes);
    fetchSideNodes(allow_down, nodes, x, y);

    nodes.clear();
}

void WorldScene::updateCenter()
{
    if(gameState)
    {
        gameState->LevelFile.clear();
        gameState->LevelTargetWarp = 0;
    }

    levelTitle.clear();
    jumpTo = false;
    std::vector<WorldNode * > nodes;
    long px = Maths::lRound(posX + m_indexTable.grid_half());
    long py = Maths::lRound(posY + m_indexTable.grid_half());
    m_indexTable.query(px, py, nodes);

    for(WorldNode *x : nodes)
    {
        //Skip uncollided elements!
        if(!x->collidePoint(px, py))
            continue;

        /*************MusicBox***************/
        if(x->type == WorldNode::musicbox)
        {
            WldMusicBoxItem *y = dynamic_cast<WldMusicBoxItem *>(x);

            if(y && y->collidePoint(px, py))
            {
                if(isInit)
                    playMusic(y->data.id, y->data.music_file);
                else if(gameState)
                {
                    gameState->game_state.musicID   = static_cast<unsigned int>(y->data.id);
                    gameState->game_state.musicFile = y->data.music_file;
                }
            }
        }
        /*************MusicBox***************/
        /*************Level Point***************/
        else if(x->type == WorldNode::level)
        {
            WldLevelItem *y = dynamic_cast<WldLevelItem *>(x);

            if(y && y->collidePoint(px, py))
            {
                levelTitle = y->data.title;

                if(!y->data.lvlfile.empty())
                {
                    std::string lvlPath = data.meta.path + "/" + y->data.lvlfile;
                    pLogDebug("Trying to check level path %s...", lvlPath.c_str());
                    LevelData head;

                    if(FileFormats::OpenLevelFileHeader(lvlPath, head))
                    {
                        pLogDebug("FOUND!");

                        if(!y->data.title.empty())
                            levelTitle = y->data.title;
                        else if(!head.LevelName.empty())
                            levelTitle = head.LevelName;
                        else if(!y->data.lvlfile.empty())
                            levelTitle = y->data.lvlfile;

                        if(gameState)
                        {
                            gameState->LevelFile = lvlPath;
                            gameState->LevelTargetWarp = y->data.entertowarp;
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
                        jumpToXY.setX(static_cast<double>(y->data.gotox));
                        jumpToXY.setY(static_cast<double>(y->data.gotoy));
                        jumpTo = true;
                    }
                }
            }
        }

        /*************Level Point***************/
    }
}

void WorldScene::initElementsVisibility()
{
    if(gameState)
    {
        for(size_t i = 0; i < wld_sceneries.size(); i++)
        {
            if(i < gameState->game_state.visibleScenery.size())
                wld_sceneries[i].vizible = gameState->game_state.visibleScenery[i].second;
            else
            {
                wld_sceneries[i].vizible = true;
                visibleItem viz;
                viz.first = static_cast<unsigned int>(wld_sceneries[i].data.meta.array_id);
                viz.second = true;
                gameState->game_state.visibleScenery.push_back(viz);
            }
        }

        for(size_t i = 0; i < wld_paths.size(); i++)
        {
            if(i < gameState->game_state.visiblePaths.size())
                wld_paths[i].vizible = gameState->game_state.visiblePaths[i].second;
            else
            {
                wld_paths[i].vizible = false;
                visibleItem viz;
                viz.first = static_cast<unsigned int>(wld_paths[i].data.meta.array_id);
                viz.second = false;
                gameState->game_state.visiblePaths.push_back(viz);
            }
        }

        for(size_t i = 0; i < wld_levels.size(); i++)
        {
            if(i < gameState->game_state.visibleLevels.size())
                wld_levels[i].vizible = gameState->game_state.visibleLevels[i].second;
            else
            {
                wld_levels[i].vizible = (wld_levels[i].data.alwaysVisible || wld_levels[i].data.gamestart);
                visibleItem viz;
                viz.first = static_cast<unsigned int>(wld_levels[i].data.meta.array_id);
                viz.second = wld_levels[i].vizible;
                gameState->game_state.visibleLevels.push_back(viz);
            }
        }
    }
}

void WorldScene::saveElementsVisibility()
{
    if(gameState)
    {
        for(size_t i = 0; i < wld_sceneries.size(); i++)
        {
            if(i < gameState->game_state.visibleScenery.size())
            {
                gameState->game_state.visibleScenery[i].first = wld_sceneries[i].data.meta.array_id;
                gameState->game_state.visibleScenery[i].second = wld_sceneries[i].vizible;
            }
            else
            {
                visibleItem viz;
                viz.first = static_cast<unsigned int>(wld_sceneries[i].data.meta.array_id);
                viz.second = wld_sceneries[i].vizible;
                gameState->game_state.visibleScenery.push_back(viz);
            }
        }

        for(size_t i = 0; i < wld_paths.size(); i++)
        {
            if(i < gameState->game_state.visiblePaths.size())
            {
                gameState->game_state.visiblePaths[i].first = wld_paths[i].data.meta.array_id;
                gameState->game_state.visiblePaths[i].second = wld_paths[i].vizible;
            }
            else
            {
                visibleItem viz;
                viz.first = static_cast<unsigned int>(wld_paths[i].data.meta.array_id);
                viz.second = wld_paths[i].vizible;
                gameState->game_state.visiblePaths.push_back(viz);
            }
        }

        for(size_t i = 0; i < wld_levels.size(); i++)
        {
            if(i < gameState->game_state.visibleLevels.size())
            {
                gameState->game_state.visibleLevels[i].first = wld_levels[i].data.meta.array_id;
                gameState->game_state.visibleLevels[i].second = wld_levels[i].vizible;
            }
            else
            {
                visibleItem viz;
                viz.first = static_cast<unsigned int>(wld_levels[i].data.meta.array_id);
                viz.second = wld_levels[i].vizible;
                gameState->game_state.visibleLevels.push_back(viz);
            }
        }
    }
}


void WorldScene::render()
{
    GlRenderer::clearScreen();

    if(!isInit)
        goto renderBlack;

    GlRenderer::setTextureColor(1.0f, 1.0f, 1.0f, 1.0f);

    if(backgroundTex.w > 0)
        GlRenderer::renderTexture(&backgroundTex, PGE_Window::Width / 2 - backgroundTex.w / 2, PGE_Window::Height / 2 - backgroundTex.h / 2);

    for(std::vector<WorldScene_misc_img>::iterator it = imgs.begin(); it != imgs.end(); it++)
    {
        AniPos x(0, 1);
        x = it->a.image();
        GlRenderer::renderTexture(&it->t,
                                  it->x,
                                  it->y,
                                  it->t.w,
                                  it->frmH,
                                  static_cast<float>(x.first),
                                  static_cast<float>(x.second));
    }

    for(std::vector<WorldScene_Portrait>::iterator it = portraits.begin(); it != portraits.end(); it++)
        it->render();

    //Viewport zone black background
    GlRenderer::renderRect(viewportRect.left(), viewportRect.top(), viewportRect.width(), viewportRect.height(), 0.f, 0.f, 0.f);
    {
        double curPosX = cameraMover.posX();
        double curPosY = cameraMover.posY();
        //Set small viewport
        GlRenderer::setViewport(viewportRect.left(), viewportRect.top(), viewportRect.width(), viewportRect.height());
        double renderX = curPosX + 16 - (viewportRect.width() / 2);
        double renderY = curPosY + 16 - (viewportRect.height() / 2);
        //Render items
        GlRenderer::setTextureColor(1.0f, 1.0f, 1.0f, 1.0f);
        const size_t render_sz = _itemsToRender.size();
        WorldNode **render_obj = _itemsToRender.data();

        for(size_t i = 0; i < render_sz; i++)
            render_obj[i]->render(render_obj[i]->x - renderX,
                                  render_obj[i]->y - renderY);

        //draw our "character"
        AniPos img(0, 1);
        img = mapwalker_ani.image();
        GlRenderer::renderTexture(&mapwalker_texture,
                                  static_cast<float>(posX - renderX + mapwalker_offset_x),
                                  static_cast<float>(posY - renderY + mapwalker_offset_y),
                                  static_cast<float>(mapwalker_texture.w),
                                  static_cast<float>(mapwalker_img_h),
                                  static_cast<float>(img.first),
                                  static_cast<float>(img.second));

        for(SceneEffectsArray::iterator it = WorkingEffects.begin(); it != WorkingEffects.end(); it++)
        {
            Scene_Effect &item = (*it);
            item.render(renderX, renderY);
        }

        if(pathOpeningInProcess && PGE_Window::showPhysicsDebug)
            pathOpener.debugRender(renderX, renderY);

        if(!m_viewportFader.isNull())
        {
            GlRenderer::renderRect(0.f,
                                   0.f,
                                   static_cast<float>(viewportRect.width()),
                                   static_cast<float>(viewportRect.height()),
                                   0.f, 0.f, 0.f,
                                   static_cast<float>(m_viewportFader.fadeRatio()));
        }

        //Restore viewport
        GlRenderer::resetViewport();
    }

    if(common_setup.points_en)
    {
        FontManager::printText(fmt::format("{0}",points),
                               common_setup.points_x,
                               common_setup.points_y,
                               common_setup.points_fontID,
                               common_setup.points_rgba.Red(),
                               common_setup.points_rgba.Green(),
                               common_setup.points_rgba.Blue(),
                               common_setup.points_rgba.Alpha());
    }

    if(common_setup.health_en)
    {
        FontManager::printText(fmt::format("{0}", health),
                               common_setup.health_x,
                               common_setup.health_y,
                               common_setup.health_fontID,
                               common_setup.health_rgba.Red(),
                               common_setup.health_rgba.Green(),
                               common_setup.health_rgba.Blue(),
                               common_setup.health_rgba.Alpha());
    }

    if(common_setup.lives_en)
    {
        FontManager::printText(fmt::format("{0}", lives),
                               common_setup.lives_x,
                               common_setup.lives_y,
                               common_setup.lives_fontID,
                               common_setup.lives_rgba.Red(),
                               common_setup.lives_rgba.Green(),
                               common_setup.lives_rgba.Blue(),
                               common_setup.lives_rgba.Alpha());
    }

    if(common_setup.coin_en)
    {
        FontManager::printText(fmt::format("{0}", coins),
                               common_setup.coin_x,
                               common_setup.coin_y,
                               common_setup.coin_fontID,
                               common_setup.coin_rgba.Red(),
                               common_setup.coin_rgba.Green(),
                               common_setup.coin_rgba.Blue(),
                               common_setup.coin_rgba.Alpha());
    }

    if(common_setup.star_en)
    {
        FontManager::printText(fmt::format("{0}", stars),
                               common_setup.star_x,
                               common_setup.star_y,
                               common_setup.star_fontID,
                               common_setup.star_rgba.Red(),
                               common_setup.star_rgba.Green(),
                               common_setup.star_rgba.Blue(),
                               common_setup.star_rgba.Alpha());
    }

    FontManager::printText(fmt::format("{0}", levelTitle),
                           common_setup.title_x,
                           common_setup.title_y,
                           common_setup.title_fontID,
                           common_setup.title_rgba.Red(),
                           common_setup.title_rgba.Green(),
                           common_setup.title_rgba.Blue(),
                           common_setup.title_rgba.Alpha()
                          );

    if(PGE_Window::showDebugInfo)
    {
        FontManager::printText(fmt::format("X={0} Y={1}", posX, posY), 300, 10);
        {
            PGE_Point grid = m_indexTable.applyGrid(Maths::lRound(posX), Maths::lRound(posY));
            FontManager::printText(fmt::format("TILE X={0} Y={1}", grid.x(), grid.y()), 300, 45);
        }
        FontManager::printText(fmt::format("TICK-SUB: {0}, Vizible items={1};", uTickf, _itemsToRender.size()), 10, 100);
        FontManager::printText(fmt::format("Delays E={0} R={1} P={2} [{3}]",
                                            debug_event_delay,
                                            debug_render_delay,
                                            debug_phys_delay,
                                            debug_total_delay), 10, 120);

        if(m_doExit)
            FontManager::printText(fmt::format("Exit delay {0}, {1}",
                                                exitWorldDelay,
                                                uTickf), 10, 140, 0, 1.0, 0, 0, 1.0);
    }

renderBlack:
    Scene::render();

    if(isPauseMenu) _pauseMenu.render();
}

void WorldScene::onKeyboardPressedSDL(SDL_Keycode sdl_key, Uint16)
{
    if(m_doExit) return;

    if(isPauseMenu) _pauseMenu.processKeyEvent(sdl_key);

    switch(sdl_key)
    {
    // Check which
    case SDLK_ESCAPE: // ESC
    case SDLK_RETURN:// Enter
    {
        if(isPauseMenu || m_doExit || lock_controls)
        {
            if(pathOpeningInProcess)
                pathOpener.skipAnimation();
            break;
        }

        isPauseMenu = true;
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
    player1Controller->update();
    controls_1 = player1Controller->keys;
}

int WorldScene::exec()
{
    worldIsContinues = true;
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
    controls_1 = Controller::noKeys();

    if(m_isRunning) update();

    /*****************************************************/

    while(m_isRunning)
    {
        start_common = SDL_GetTicks();

        if(PGE_Window::showDebugInfo) start_events = SDL_GetTicks();

        processEvents();

        if(PGE_Window::showDebugInfo) stop_events = SDL_GetTicks();

        if(PGE_Window::showDebugInfo)
            debug_event_delay = static_cast<int>(stop_events - start_events);

        start_physics = SDL_GetTicks();
        update();
        stop_physics = SDL_GetTicks();

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
            doUpdate_render = frameSkip ? uTickf + (stop_render - start_render) : 0;

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
            SDL_Delay(uTick - (SDL_GetTicks() - start_common));

        if(PGE_Window::showDebugInfo)
            debug_total_delay = static_cast<int>(SDL_GetTicks() - start_common);
    }

    return exitWorldCode;
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

    for(std::vector<WorldScene_misc_img>::iterator it = imgs.begin(); it != imgs.end(); it++)
        it->a.manualTick(ticks);

    for(std::vector<WorldScene_Portrait>::iterator it = portraits.begin(); it != portraits.end(); it++)
        it->update(ticks);
}

bool WorldScene::isExit()
{
    return !worldIsContinues;
}

void WorldScene::setExiting(int delay, int reason)
{
    exitWorldDelay = delay;
    exitWorldCode = reason;
    m_doExit = true;
}

bool WorldScene::loadFile(std::string filePath)
{
    data.meta.ReadFileValid = false;

    if(!Files::fileExists(filePath))
    {
        errorMsg += "File not exist\n\n";
        errorMsg += filePath;
        return false;
    }

    if(!FileFormats::OpenWorldFile(filePath, data))
        errorMsg += "Bad file format\n";

    return data.meta.ReadFileValid;
}

std::string WorldScene::getLastError()
{
    return errorMsg;
}

void WorldScene::jump()
{
    posX = jumpToXY.x();
    posY = jumpToXY.y();
    cameraMover.setPos(posX, posY);

    if(gameState)
    {
        gameState->game_state.worldPosX = Maths::lRound(posX);
        gameState->game_state.worldPosY = Maths::lRound(posY);
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
    std::string musPath = ConfigManager::getWldMusic(musicID, data.meta.path + "/" + customMusicFile);

    if(musPath.empty())
        return;

    PGE_MusPlayer::openFile(musPath);

    if(fade)
        PGE_MusPlayer::fadeIn(fadeLen);
    else
        PGE_MusPlayer::play();

    if(gameState)
    {
        gameState->game_state.musicID = static_cast<unsigned int>(musicID);
        gameState->game_state.musicFile = customMusicFile;
    }
}

void WorldScene::mapwalker_refreshDirection()
{
    switch(walk_direction)
    {
    case Walk_Left:
        mapwalker_ani.setFrameSequance(mapwalker_setup.wld_frames_left);
        break;

    case Walk_Right:
        mapwalker_ani.setFrameSequance(mapwalker_setup.wld_frames_right);
        break;

    case Walk_Up:
        mapwalker_ani.setFrameSequance(mapwalker_setup.wld_frames_up);
        break;

    case Walk_Down:
        mapwalker_ani.setFrameSequance(mapwalker_setup.wld_frames_down);
        break;

    default:
        break;
    }
}

bool WorldScene::isVizibleOnScreen(PGE_RectF &rect)
{
    PGE_RectF screen(0, 0, viewportRect.width(), viewportRect.height());
    double renderX = posX + 16 - (viewportRect.width() / 2);
    double renderY = posY + 16 - (viewportRect.height() / 2);
    screen.setPos(renderX, renderY);

    if(screen.collideRect(rect))
        return true;

    return false;
}
