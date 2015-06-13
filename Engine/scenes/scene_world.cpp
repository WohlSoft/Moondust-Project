/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
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

#include "scene_world.h"
#include <audio/SdlMusPlayer.h>
#include <audio/pge_audio.h>
#include <graphics/window.h>
#include <graphics/gl_renderer.h>
#include <fontman/font_manager.h>
#include <PGE_File_Formats/file_formats.h>
#include <common_features/graphics_funcs.h>
#include <common_features/logger.h>
#include <common_features/maths.h>
#include <controls/controller_keyboard.h>
#include <controls/controller_joystick.h>
#include <gui/pge_msgbox.h>
#include <data_configs/config_manager.h>
#include <settings/global_settings.h>

#include <QHash>
#include <unordered_map>

WorldScene::WorldScene()
    : Scene(World), luaEngine(this)
{
    wld_events.abort();

    exitWorldCode=WldExit::EXIT_error;
    exitWorldDelay=2000;
    worldIsContinues=true;
    isPauseMenu=false;

    gameState=NULL;

    isInit=false;

    debug_render_delay=0;
    debug_phys_delay=0;
    debug_event_delay=0;

    mapwalker_img_h = ConfigManager::default_grid;
    mapwalker_offset_x=0;
    mapwalker_offset_y=0;

    /*********Controller********/
    player1Controller = AppSettings.openController(1);
    /*********Controller********/

    initPauseMenu1();

    frameSkip=AppSettings.frameSkip;

    /***********Number of Players*****************/
    numOfPlayers=1;
    /***********Number of Players*****************/

    /*********Fader*************/
    fader.setFull();
    /*********Fader*************/

    move_speed = 125/(float)PGE_Window::TicksPerSecond;
    move_steps_count=0;

    ConfigManager::setup_WorldMap.initFonts();

    common_setup = ConfigManager::setup_WorldMap;

    if(common_setup.backgroundImg.isEmpty())
        backgroundTex.w=0;
    else
        GlRenderer::loadTextureP(backgroundTex, common_setup.backgroundImg);

    imgs.clear();
    for(int i=0; i<common_setup.AdditionalImages.size(); i++)
    {
        if(common_setup.AdditionalImages[i].imgFile.isEmpty()) continue;

        WorldScene_misc_img img;
        GlRenderer::loadTextureP(img.t, common_setup.AdditionalImages[i].imgFile);

        img.x = common_setup.AdditionalImages[i].x;
        img.y = common_setup.AdditionalImages[i].y;
        img.a.construct(common_setup.AdditionalImages[i].animated,
                        common_setup.AdditionalImages[i].frames,
                        common_setup.AdditionalImages[i].framedelay);
        img.frmH = (img.t.h / common_setup.AdditionalImages[i].frames);

        imgs.push_back(img);
    }

    viewportRect.setX(common_setup.viewport_x);
    viewportRect.setY(common_setup.viewport_y);
    viewportRect.setWidth(common_setup.viewport_w);
    viewportRect.setHeight(common_setup.viewport_h);

    posX=0;
    posY=0;
    levelTitle = "Hello!";
    health = 3;
    points = 0;
    stars  = 0;
    coins  = 0;

    jumpTo=false;

    walk_direction=Walk_Idle;
    lock_controls=false;
    ignore_paths=false;
    allow_left=false;
    allow_up=false;
    allow_right=false;
    allow_down=false;
    _playStopSnd=false;
    _playDenySnd=false;

    data = FileFormats::dummyWldDataArray();
}

WorldScene::~WorldScene()
{
    PGE_MusPlayer::MUS_stopMusic();
    wld_events.abort();
    _indexTable.clean();
    wldItems.clear();
    _itemsToRender.clear();

    wld_tiles.clear();
    wld_sceneries.clear();
    wld_paths.clear();
    wld_levels.clear();
    wld_musicboxes.clear();

    GlRenderer::deleteTexture(backgroundTex);

    //destroy textures
    qDebug() << "clear world textures";
    while(!textures_bank.isEmpty())
    {
        GlRenderer::deleteTexture(textures_bank[0]);
        textures_bank.pop_front();
    }

    for(int i=0;i<imgs.size();i++)
    {
        GlRenderer::deleteTexture(imgs[i].t);
    }
    imgs.clear();

    ConfigManager::unloadLevelConfigs();
    ConfigManager::unloadWorldConfigs();

    delete player1Controller;
}

void WorldScene::setGameState(EpisodeState *_state)
{
    if(!_state) return;
    gameState = _state;

    numOfPlayers=_state->numOfPlayers;

    points = 1000;//gameState->game_state.points;
    coins  = 55;//gameState->game_state.coins;
    stars  = 5;//gameState->game_state.gottenStars;

    PlayerState x = gameState->getPlayerState(1);
    health = x._chsetup.health;


    gameState->replay_on_fail = data.restartlevel;
    if(gameState->episodeIsStarted && !data.HubStyledWorld)
    {
        posX = gameState->game_state.worldPosX;
        posY = gameState->game_state.worldPosY;
        updateAvailablePaths();
        updateCenter();
    }
    else
    {
        gameState->episodeIsStarted=true;
        gameState->WorldPath = data.path;

        //Detect gamestart and set position on them
        for(long i=0; i<data.levels.size(); i++)
        {
            if(data.levels[i].gamestart)
            {
                posX=data.levels[i].x;
                posY=data.levels[i].y;
                gameState->game_state.worldPosX=posX;
                gameState->game_state.worldPosY=posY;
                break;
            }
        }

        //open Intro level
        if(!data.IntroLevel_file.isEmpty())
        {
            //Fix file extension
            if((!data.IntroLevel_file.endsWith(".lvlx", Qt::CaseInsensitive))&&
               (!data.IntroLevel_file.endsWith(".lvl", Qt::CaseInsensitive)))
                    data.IntroLevel_file.append(".lvl");

            QString introLevelFile = gameState->WorldPath+"/"+data.IntroLevel_file;
            WriteToLog(QtDebugMsg, "Opening intro level: "+introLevelFile);

            if(QFileInfo(introLevelFile).exists())
            {
                LevelData checking = FileFormats::OpenLevelFile(introLevelFile);
                if(checking.ReadFileValid)
                {
                    WriteToLog(QtDebugMsg, "File valid, do exit!");
                    gameState->LevelFile = introLevelFile;
                    gameState->LevelPath = checking.path;
                    if(data.HubStyledWorld)
                    {
                        gameState->LevelFile_hub = checking.path;
                        gameState->LevelTargetWarp = gameState->game_state.last_hub_warp ;
                    }
                    else
                    {
                        gameState->LevelTargetWarp = 0;
                    }
                    gameState->isHubLevel = data.HubStyledWorld;

                    //Jump to the intro/hub level
                    doExit=true;
                    exitWorldCode=WldExit::EXIT_beginLevel;
                    return;
                }
                else
                {
                    WriteToLog(QtDebugMsg, "File invalid");
                }
            }
        }
    }
}

bool WorldScene::init()
{
    luaEngine.setLuaScriptPath(ConfigManager::PathScript());
    luaEngine.setCoreFile(ConfigManager::setup_WorldMap.luaFile);
    luaEngine.setErrorReporterFunc([this](const QString& errorMessage, const QString& stacktrace){
        qWarning() << "Lua-Error: ";
        qWarning() << "Error Message: " << errorMessage;
        qWarning() << "Stacktrace: \n" << stacktrace;
        PGE_MsgBox msgBox(this, QString("A lua error has been thrown: \n") + errorMessage + "\n\nMore details in the log!", PGE_MsgBox::msg_error);
        msgBox.exec();
    });
    luaEngine.init();

    _indexTable.clean();
    wldItems.clear();
    _itemsToRender.clear();

    wld_tiles.clear();
    wld_sceneries.clear();
    wld_paths.clear();
    wld_levels.clear();
    wld_musicboxes.clear();

    if(doExit) return true;

    if(!loadConfigs())
        return false;

    int player_portrait_step=0;
    int player_portrait_x=common_setup.portrait_x;
    int player_portrait_y=common_setup.portrait_y;

    if(numOfPlayers>1)
    {
        player_portrait_step=30;
        player_portrait_x=player_portrait_x-(numOfPlayers*30)/2;
    }

    players.clear();
    for(int i=1; i<=numOfPlayers;i++)
    {
        PlayerState state;
        if(gameState)
        {
            state = gameState->getPlayerState(i);
            players.push_back(state);
        }
        else
        {
            state.characterID=1;
            state.stateID=1;
            state._chsetup=FileFormats::dummySavCharacterState();
            players.push_back(state);
        }

        if(common_setup.points_en)
        {
            WorldScene_Portrait portrait(state.characterID, state.stateID,
                                                             player_portrait_x,
                                                             player_portrait_y,
                                                             common_setup.portrait_animation,
                                                             common_setup.portrait_frame_delay,
                                                             common_setup.portrait_direction);
            portraits.push_back(portrait);
            player_portrait_x +=player_portrait_step;
        }
    }


    PlayerState player_state;
    if(gameState)
    {
        player_state = gameState->getPlayerState(1);
    }
    mapwalker_setup = ConfigManager::playable_characters[player_state.characterID];
    long tID = ConfigManager::getWldPlayerTexture(player_state.characterID, player_state.stateID);
        if(tID<0)
            return false;

    mapwalker_texture = ConfigManager::world_textures[tID];
    mapwalker_img_h = mapwalker_texture.h/mapwalker_setup.wld_frames;
    mapwalker_ani.construct(true, mapwalker_setup.wld_frames, mapwalker_setup.wld_framespeed);
    mapwalker_ani.setFrameSequance(mapwalker_setup.wld_frames_down);
    mapwalker_offset_x = (ConfigManager::default_grid/2)-(mapwalker_texture.w/2);
    mapwalker_offset_y = ConfigManager::default_grid-mapwalker_img_h + mapwalker_setup.wld_offset_y;


    for(int i=0; i<data.tiles.size(); i++)
    {
        WldTileItem path(data.tiles[i]);
        if(!path.init())
            continue;
        wld_tiles << path;
        _indexTable.addNode(path.x, path.y, path.w, path.h, &(wld_tiles.last()));
    }

    for(int i=0; i<data.scenery.size(); i++)
    {
        WldSceneryItem path(data.scenery[i]);
        if(!path.init())
            continue;
        wld_sceneries << path;
        _indexTable.addNode(path.x, path.y, path.w, path.h, &(wld_sceneries.last()));
    }

    for(int i=0; i<data.paths.size(); i++)
    {
        WldPathItem path(data.paths[i]);
        if(!path.init())
            continue;
        wld_paths << path;
        _indexTable.addNode(path.x, path.y, path.w, path.h, &(wld_paths.last()));
    }

    for(int i=0; i<data.levels.size(); i++)
    {
        WldLevelItem path(data.levels[i]);
        if(!path.init())
            continue;
        wld_levels << path;
        _indexTable.addNode(path.x+path.offset_x, path.y+path.offset_y, path.texture.w, path.texture.h, &(wld_levels.last()));
    }

    for(int i=0; i<data.music.size(); i++)
    {
        WldMusicBoxItem path(data.music[i]);
        path.r=0.5f;
        path.g=0.5f;
        path.b=1.f;
        wld_musicboxes << path;
        _indexTable.addNode(path.x, path.y, path.w, path.h, &(wld_musicboxes.last()));
    }

    updateAvailablePaths();
    updateCenter();

    if(gameState)
        playMusic(gameState->game_state.musicID, gameState->game_state.musicFile, true, 200);

    isInit=true;

    return true;
}

bool WorldScene::loadConfigs()
{
    bool success=true;
    QString musIni=data.path+"/music.ini";
    QString sndIni=data.path+"/sounds.ini";
    if(ConfigManager::music_lastIniFile!=musIni)
    {
        ConfigManager::loadDefaultMusics();
        ConfigManager::loadMusic(data.path+"/", musIni, true);
    }
    if(ConfigManager::sound_lastIniFile!=sndIni)
    {
        ConfigManager::loadDefaultSounds();
        ConfigManager::loadSound(data.path+"/", sndIni, true);
        if(ConfigManager::soundIniChanged())
            ConfigManager::buildSoundIndex();
    }

    //Load INI-files
    success = ConfigManager::loadWorldTiles();   //!< Tiles
    success = ConfigManager::loadWorldScenery(); //!< Scenery
    success = ConfigManager::loadWorldPaths();   //!< Paths
    success = ConfigManager::loadWorldLevels();  //!< Levels

    //Set paths
    ConfigManager::Dir_Tiles.setCustomDirs(data.path, data.filename, ConfigManager::PathWorldTiles() );
    ConfigManager::Dir_Scenery.setCustomDirs(data.path, data.filename, ConfigManager::PathWorldScenery() );
    ConfigManager::Dir_WldPaths.setCustomDirs(data.path, data.filename, ConfigManager::PathWorldPaths() );
    ConfigManager::Dir_WldLevel.setCustomDirs(data.path, data.filename, ConfigManager::PathWorldLevels() );
    ConfigManager::Dir_PlayerLvl.setCustomDirs(data.path, data.filename, ConfigManager::PathLevelPlayable() );
    ConfigManager::Dir_PlayerWld.setCustomDirs(data.path, data.filename, ConfigManager::PathWorldPlayable() );

    if(!success) exitWorldCode = WldExit::EXIT_error;
    return success;
}



void WorldScene::doMoveStep(double &posVal)
{
    move_steps_count+=move_speed;
    if(move_steps_count>=ConfigManager::default_grid)
    {
        move_steps_count=0;
        posVal=Maths::roundTo(posVal, ConfigManager::default_grid);
    }
    if((long(posVal)==posVal)&&(long(posVal)%ConfigManager::default_grid==0))
    {
        walk_direction=0; _playStopSnd=true; updateAvailablePaths(); updateCenter();
    }
}

void WorldScene::setDir(int dr)
{
    walk_direction=dr;
    move_steps_count=ConfigManager::default_grid-move_steps_count;
    mapwalker_refreshDirection();
}


void WorldScene::initPauseMenu1()
{
    _pauseMenu_opened=false;
    _pauseMenuID=1;
    _pauseMenu.setParentScene(this);
    _pauseMenu.construct("Pause", PGE_MenuBox::msg_info, PGE_Point(-1,-1),
                         ConfigManager::setup_menu_box.box_padding,
                         ConfigManager::setup_menu_box.sprite);
    _pauseMenu.clearMenu();
    QStringList items;
    items<<"Continue";
    items<<"Save and continue";
    items<<"Save and quit";
    items<<"Quit without saving";
    _pauseMenu.addMenuItems(items);
    _pauseMenu.setRejectSnd(obj_sound_role::MenuPause);
    _pauseMenu.setMaxMenuItems(4);
    isPauseMenu=false;
}

void WorldScene::initPauseMenu2()
{
    _pauseMenu_opened=false;
    _pauseMenuID=2;
    _pauseMenu.setParentScene(this);
    _pauseMenu.construct("Pause", PGE_MenuBox::msg_info, PGE_Point(-1,-1),
                         ConfigManager::setup_menu_box.box_padding,
                         ConfigManager::setup_menu_box.sprite);
    _pauseMenu.clearMenu();
    QStringList items;
    items<<"Continue";
    items<<"Quit";
    _pauseMenu.addMenuItems(items);
    _pauseMenu.setRejectSnd(obj_sound_role::MenuPause);
    _pauseMenu.setMaxMenuItems(4);
    isPauseMenu=false;
}

void WorldScene::processPauseMenu()
{
    if(!_pauseMenu_opened)
    {
        _pauseMenu.restart();
        _pauseMenu_opened=true;
        PGE_Audio::playSoundByRole(obj_sound_role::MenuPause);
    }
    else
    {
        _pauseMenu.update(uTickf);
        if(!_pauseMenu.isRunning())
        {
            if(_pauseMenuID==1)
            {
                switch(_pauseMenu.answer())
                {
                case PAUSE_Continue:
                    //do nothing!!
                break;
                case PAUSE_SaveCont:
                    //Save game state!
                    gameState->game_state.worldPosX=posX;
                    gameState->game_state.worldPosY=posY;
                    gameState->save();
                break;
                case PAUSE_SaveQuit:
                    //Save game state! and exit from episode
                    gameState->game_state.worldPosX=posX;
                    gameState->game_state.worldPosY=posY;
                    gameState->save();
                    setExiting(0, WldExit::EXIT_exitWithSave);
                    break;
                case PAUSE_Exit:
                    //Save game state! and exit from episode
                    setExiting(0, WldExit::EXIT_exitNoSave);
                break;
                default: break;
                }
            } else {
                switch(_pauseMenu.answer())
                {
                case PAUSE_2_Continue:
                    //do nothing!!
                break;
                case PAUSE_2_Exit:
                    //Save game state! and exit from episode
                    setExiting(0, WldExit::EXIT_exitNoSave);
                break;
                default: break;
                }
            }
            _pauseMenu_opened=false;
            isPauseMenu=false;
        }
    }
}

void WorldScene::update()
{
    tickAnimations(uTickf);
    Scene::update();
    updateLua();

    if(doExit)
    {
        if(exitWorldCode==WldExit::EXIT_close)
        {
            fader.setFull();
            worldIsContinues=false;
            running=false;
        }
        else
        {
            if(fader.isNull()) fader.setFade(10, 1.0f, 0.01f);
            if(fader.isFull())
            {
                worldIsContinues=false;
                running=false;
            }
        }
    } else if(isPauseMenu) {
        processPauseMenu();
    } else {
        wld_events.processEvents(uTickf);

        if(walk_direction==Walk_Idle)
        {
            if(!lock_controls)
            {
                if(controls_1.left && (allow_left || ignore_paths))
                    walk_direction=Walk_Left;
                if(controls_1.right && (allow_right || ignore_paths))
                    walk_direction=Walk_Right;
                if(controls_1.up && (allow_up || ignore_paths))
                    walk_direction=Walk_Up;
                if(controls_1.down && (allow_down || ignore_paths))
                    walk_direction=Walk_Down;

                //If movement denied - play sound
                if((controls_1.left||controls_1.right||controls_1.up||controls_1.down)&&(walk_direction==Walk_Idle))
                {       _playStopSnd=false;
                        if(!_playDenySnd) { PGE_Audio::playSoundByRole(obj_sound_role::WorldDeny); _playDenySnd=true; }
                }
                else
                if (!controls_1.left&&!controls_1.right&&!controls_1.up&&!controls_1.down)
                    _playDenySnd=false;
            }
            if(walk_direction!=Walk_Idle)
            {
                _playDenySnd=false;
                _playStopSnd=false;
                gameState->LevelFile.clear();
                jumpTo=false;
                mapwalker_refreshDirection();
            }

            if(_playStopSnd) { PGE_Audio::playSoundByRole(obj_sound_role::WorldMove); _playStopSnd=false; }
        }
        else
        {
            mapwalker_ani.manualTick(uTickf);

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
            default: break;
            }
        }

        switch(walk_direction)
        {
            case Walk_Left:
                posX-=move_speed;
                doMoveStep(posX);
                break;
            case Walk_Right:
                posX+=move_speed;
                doMoveStep(posX);
                break;
            case Walk_Up:
                posY-=move_speed;
                doMoveStep(posY);
                break;
            case Walk_Down:
                posY+=move_speed;
                doMoveStep(posY);
                break;
            default: break;
        }

        _itemsToRender.clear();
        _indexTable.query(posX-(viewportRect.width()/2), posY-(viewportRect.height()/2), posX+(viewportRect.width()/2), posY+(viewportRect.height()/2), _itemsToRender, true);

        if(isPauseMenu)
        {
            PGE_MsgBox msgBox(this, "Hi guys!\nThis is a dummy world map. I think, it works fine!",
                              PGE_MsgBox::msg_info);

            if(!ConfigManager::setup_message_box.sprite.isEmpty())
                msgBox.loadTexture(ConfigManager::setup_message_box.sprite);
            msgBox.exec();
            isPauseMenu=false;
        }
    }




    if(controls_1.jump || controls_1.alt_jump)
    {
        if((!lock_controls) && (!isPauseMenu) && (gameState))
        {
            if(!gameState->LevelFile.isEmpty())
            {
                gameState->game_state.worldPosX=posX;
                gameState->game_state.worldPosY=posY;
                PGE_Audio::playSoundByRole(obj_sound_role::WorldEnterLevel);
                stopMusic(true, 300);
                lock_controls=true;
                setExiting(0, WldExit::EXIT_beginLevel);
            }
            else if(jumpTo)
            {
                //Create events
                EventQueueEntry<WorldScene >event1;
                event1.makeWaiterFlagT(this, &WorldScene::isOpacityFadding, true, 100);
                wld_events.events.push_back(event1);

                EventQueueEntry<WorldScene >event2;
                event2.makeCallerT(this, &WorldScene::jump, 100);
                wld_events.events.push_back(event2);

                EventQueueEntry<WorldScene >event3;
                event3.makeCaller([this]()->void{
                                      this->fader.setFade(10, 0.0f, 0.05);
                                      this->lock_controls=false;
                                  }, 0);
                wld_events.events.push_back(event3);

                this->lock_controls=true;
                PGE_Audio::playSoundByRole(obj_sound_role::WarpPipe);
                fader.setFade(10, 1.0f, 0.05);
            }
        }
    }
}


void WorldScene::fetchSideNodes(bool &side, QVector<WorldNode* > &nodes, float cx, float cy)
{
    side=false;
    foreach (WorldNode* x, nodes)
    {
        if(x->type==WorldNode::path)
        {
            side=x->collidePoint(cx, cy); break;
        }

        if(x->type==WorldNode::level)
        {
            side=x->collidePoint(cx, cy); break;
        }
    }
}

void WorldScene::updateAvailablePaths()
{
    QVector<WorldNode* > nodes;

    long x,y;
    //left
    x=posX+_indexTable.grid_half()-_indexTable.grid();
    y=posY+_indexTable.grid_half();
    _indexTable.query(x,y, nodes);
    fetchSideNodes(allow_left, nodes, x,y);
    nodes.clear();

    //Right
    x=posX+_indexTable.grid_half()+_indexTable.grid();
    y=posY+_indexTable.grid_half();
    _indexTable.query(x,y, nodes);
    fetchSideNodes(allow_right, nodes, x, y);
    nodes.clear();

    //Top
    x=posX+_indexTable.grid_half();
    y=posY+_indexTable.grid_half()-_indexTable.grid();
    _indexTable.query(x, y, nodes);
    fetchSideNodes(allow_up, nodes, x, y);
    nodes.clear();

    //Bottom
    x=posX+_indexTable.grid_half();
    y=posY+_indexTable.grid_half()+_indexTable.grid();
    _indexTable.query(x,y, nodes);
    fetchSideNodes(allow_down, nodes, x, y);
    nodes.clear();
}

void WorldScene::updateCenter()
{
    if(gameState)
    {
        gameState->LevelFile.clear();
        gameState->LevelTargetWarp=0;
    }

    levelTitle.clear();
    jumpTo=false;

    QVector<WorldNode* > nodes;
    long px=posX+_indexTable.grid_half();
    long py=posY+_indexTable.grid_half();
    _indexTable.query(px, py, nodes);
    foreach (WorldNode* x, nodes)
    {
        /*************MusicBox***************/
        if(x->type==WorldNode::musicbox)
        {
            WldMusicBoxItem *y = dynamic_cast<WldMusicBoxItem*>(x);
            if(y && y->collidePoint(px, py))
            {
                if(isInit)
                    playMusic(y->data.id, y->data.music_file);
                else
                if(gameState)
                {
                    gameState->game_state.musicID   = y->data.id;
                    gameState->game_state.musicFile = y->data.music_file;
                }
            }
        }
        /*************MusicBox***************/

        /*************Level Point***************/
        if(x->type==WorldNode::level)
        {
            WldLevelItem *y = dynamic_cast<WldLevelItem*>(x);
            if(y && y->collidePoint(px, py))
            {
                levelTitle = y->data.title;
                if(!y->data.lvlfile.isEmpty())
                {
                    QString lvlPath=data.path+"/"+y->data.lvlfile;
                    LevelData head = FileFormats::OpenLevelFileHeader(lvlPath);
                    if(head.ReadFileValid)
                    {
                        if(!y->data.title.isEmpty())
                        {
                            levelTitle = y->data.title;
                        }
                        else if(!head.LevelName.isEmpty())
                        {
                            levelTitle = head.LevelName;
                        }
                        else if(!y->data.lvlfile.isEmpty())
                        {
                            levelTitle = y->data.lvlfile;
                        }

                        if(gameState)
                        {
                            gameState->LevelFile = lvlPath;
                            gameState->LevelTargetWarp = y->data.entertowarp;
                        }
                    }
                }
                else
                {
                    if( (y->data.gotox!=-1)&&(y->data.gotoy!=-1))
                    {
                        jumpToXY.setX(y->data.gotox);
                        jumpToXY.setY(y->data.gotoy);
                        jumpTo=true;
                    }
                }
            }
        }
        /*************Level Point***************/
    }
}

void WorldScene::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //Reset modelview matrix
    glLoadIdentity();

    if(!isInit)
        goto renderBlack;

    if(backgroundTex.w>0)
        GlRenderer::renderTexture(&backgroundTex, PGE_Window::Width/2 - backgroundTex.w/2, PGE_Window::Height/2 - backgroundTex.h/2);

    for(QVector<WorldScene_misc_img>::iterator it = imgs.begin(); it!= imgs.end(); it++)
    {
        AniPos x(0,1); x = it->a.image();
        GlRenderer::renderTexture(&it->t,
                                  it->x,
                                  it->y,
                                  it->t.w,
                                  it->frmH, x.first, x.second);
    }

    for(QVector<WorldScene_Portrait>::iterator it = portraits.begin(); it!= portraits.end(); it++)
            it->render();

    //Viewport zone black background
    GlRenderer::renderRect(viewportRect.left(), viewportRect.top(), viewportRect.width(), viewportRect.height(), 0.f,0.f,0.f);

    {
        //Set small viewport
        GlRenderer::setViewport(viewportRect.left(), viewportRect.top(), viewportRect.width(), viewportRect.height());
        double renderX = posX+16-(viewportRect.width()/2);
        double renderY = posY+16-(viewportRect.height()/2);

        //Render items
        const int render_sz = _itemsToRender.size();
        WorldNode** render_obj = _itemsToRender.data();
        for(int i=0; i<render_sz; i++)
            render_obj[i]->render(render_obj[i]->x-renderX, render_obj[i]->y-renderY);

        //draw our "character"
        AniPos img(0,1); img = mapwalker_ani.image();
        GlRenderer::renderTexture(&mapwalker_texture,
                                  posX-renderX+mapwalker_offset_x,
                                  posY-renderY+mapwalker_offset_y,
                                  mapwalker_texture.w,
                                  mapwalker_img_h,
                                  img.first, img.second);

        //Restore viewport
        GlRenderer::resetViewport();
    }

    if(common_setup.points_en)
    {
        FontManager::printText(QString("%1")
                               .arg(points),
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
        FontManager::printText(QString("%1")
                               .arg(health),
                               common_setup.health_x,
                               common_setup.health_y,
                               common_setup.health_fontID,
                               common_setup.health_rgba.Red(),
                               common_setup.health_rgba.Green(),
                               common_setup.health_rgba.Blue(),
                               common_setup.health_rgba.Alpha());
    }

    if(common_setup.coin_en)
    {
        FontManager::printText(QString("%1")
                               .arg(coins),
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
        FontManager::printText(QString("%1")
                               .arg(stars),
                               common_setup.star_x,
                               common_setup.star_y,
                               common_setup.star_fontID,
                               common_setup.star_rgba.Red(),
                               common_setup.star_rgba.Green(),
                               common_setup.star_rgba.Blue(),
                               common_setup.star_rgba.Alpha());
    }

    FontManager::printText(QString("%1")
                           .arg(levelTitle),
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
        FontManager::printText(QString("X=%1 Y=%2")
                               .arg(posX)
                               .arg(posY), 300,10);

        {
        PGE_Point grid = _indexTable.applyGrid(posX, posY);
        FontManager::printText(QString("TILE X=%1 Y=%2")
                               .arg(grid.x())
                               .arg(grid.y()), 300,45);
        }

        FontManager::printText(QString("TICK-SUB: %1, Vizible items=%2;")
                               .arg(uTickf)
                               .arg(_itemsToRender.size()), 10,100);

        FontManager::printText(QString("Delays E=%1 R=%2 P=%3")
                               .arg(debug_event_delay, 3, 10, QChar('0'))
                               .arg(debug_render_delay, 3, 10, QChar('0'))
                               .arg(debug_phys_delay, 3, 10, QChar('0')), 10,120);

        if(doExit)
            FontManager::printText(QString("Exit delay %1, %2")
                                   .arg(exitWorldDelay)
                                   .arg(uTickf), 10, 140, 0, 1.0, 0, 0, 1.0);
    }
    renderBlack:
    Scene::render();

    if(isPauseMenu) _pauseMenu.render();
}

void WorldScene::onKeyboardPressedSDL(SDL_Keycode sdl_key, Uint16)
{
    if(doExit) return;

    if(isPauseMenu) _pauseMenu.processKeyEvent(sdl_key);

    switch(sdl_key)
    { // Check which
        case SDLK_ESCAPE: // ESC
        case SDLK_RETURN:// Enter
            {
                if( isPauseMenu || doExit || lock_controls) break;
                isPauseMenu = true;
            }
        break;
        case SDLK_i:
            ignore_paths= !ignore_paths;
            if(ignore_paths)
                PGE_Audio::playSoundByRole(obj_sound_role::PlayerGrow);
            else
                PGE_Audio::playSoundByRole(obj_sound_role::PlayerShrink);
        break;
        default: break;
    }
}

LuaEngine *WorldScene::getLuaEngine()
{
    return &luaEngine;
}

void WorldScene::processEvents()
{
    player1Controller->update();
    controls_1 = player1Controller->keys;
    Scene::processEvents();
}

int WorldScene::exec()
{
    worldIsContinues=true;
    glClearColor(0.0, 0.0, 0.0, 1.0f);

    //World scene's Loop
 Uint32 start_render=0;
 Uint32 stop_render=0;
  float doUpdate_render=0;

 Uint32 start_physics=0;
 Uint32 stop_physics=0;

  Uint32 start_events=0;
  Uint32 stop_events=0;

  Uint32 start_common=0;

    running = !doExit;
    /****************Initial update***********************/
    //(Need to prevent accidental spawn of messagebox or pause menu with empty screen)
    controls_1 = Controller::noKeys();
    if(running) update();
    /*****************************************************/

    while(running)
    {
        start_common = SDL_GetTicks();

        if(PGE_Window::showDebugInfo) start_events = SDL_GetTicks();
        processEvents();
        if(PGE_Window::showDebugInfo) stop_events = SDL_GetTicks();
        if(PGE_Window::showDebugInfo) debug_event_delay=(stop_events-start_events);

        start_physics=SDL_GetTicks();
        update();
        stop_physics=SDL_GetTicks();
        if(PGE_Window::showDebugInfo) debug_phys_delay=(stop_physics-start_physics);

        stop_render=0;
        start_render=0;
        if(doUpdate_render<=0.f)
        {
            start_render = SDL_GetTicks();
            render();
            stop_render = SDL_GetTicks();
            doUpdate_render = frameSkip ? (stop_render-start_render) : 0;
            if(PGE_Window::showDebugInfo) debug_render_delay = stop_render-start_render;
        }
        doUpdate_render -= uTickf;
        if(stop_render < start_render) {stop_render=0; start_render=0; }

        glFlush();
        PGE_Window::rePaint();

        if( uTickf > (float)(SDL_GetTicks()-start_common) )
        {
            wait( uTickf-(float)(SDL_GetTicks()-start_common));
        }
    }
    return exitWorldCode;
}

void WorldScene::tickAnimations(float ticks)
{
    //tick animation
    for(QList<SimpleAnimator>::iterator it=ConfigManager::Animator_Tiles.begin(); it!=ConfigManager::Animator_Tiles.end(); it++ )
        it->manualTick(ticks);
    for(QList<SimpleAnimator>::iterator it=ConfigManager::Animator_Scenery.begin(); it!=ConfigManager::Animator_Scenery.end(); it++ )
        it->manualTick(ticks);
    for(QList<SimpleAnimator>::iterator it=ConfigManager::Animator_WldPaths.begin(); it!=ConfigManager::Animator_WldPaths.end(); it++ )
        it->manualTick(ticks);
    for(QList<SimpleAnimator>::iterator it=ConfigManager::Animator_WldLevel.begin(); it!=ConfigManager::Animator_WldLevel.end(); it++ )
        it->manualTick(ticks);

    for(QVector<WorldScene_misc_img>::iterator it = imgs.begin(); it!= imgs.end(); it++)
        it->a.manualTick(ticks);

    for(QVector<WorldScene_Portrait>::iterator it = portraits.begin(); it!= portraits.end(); it++)
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
    doExit = true;
}

bool WorldScene::loadFile(QString filePath)
{
    data.ReadFileValid = false;
    if(!QFileInfo(filePath).exists())
    {
        errorMsg += "File not exist\n\n";
        errorMsg += filePath;
        return false;
    }

    data = FileFormats::OpenWorldFile(filePath);
    if(!data.ReadFileValid)
        errorMsg += "Bad file format\n";
    return data.ReadFileValid;
}

QString WorldScene::getLastError()
{
    return errorMsg;
}

void WorldScene::jump()
{
    posX = jumpToXY.x();
    posY = jumpToXY.y();
    if(gameState)
    {
        gameState->game_state.worldPosX = posX;
        gameState->game_state.worldPosY = posY;
    }
    updateAvailablePaths();
    updateCenter();
}



void WorldScene::stopMusic(bool fade, int fadeLen)
{
    if(fade)
        PGE_MusPlayer::MUS_stopMusicFadeOut(fadeLen);
    else
        PGE_MusPlayer::MUS_stopMusic();
}

void WorldScene::playMusic(long musicID, QString customMusicFile, bool fade, int fadeLen)
{
    QString musPath = ConfigManager::getWldMusic(musicID, data.path+"/"+customMusicFile);
    if(musPath.isEmpty()) return;

    PGE_MusPlayer::MUS_openFile(musPath);
    if(fade)
        PGE_MusPlayer::MUS_playMusicFadeIn(fadeLen);
    else
        PGE_MusPlayer::MUS_playMusic();
    if(gameState)
    {
        gameState->game_state.musicID = musicID;
        gameState->game_state.musicFile = customMusicFile;
    }
}


void WorldScene::mapwalker_refreshDirection()
{
    switch(walk_direction)
    {
        case Walk_Left:  mapwalker_ani.setFrameSequance(mapwalker_setup.wld_frames_left); break;
        case Walk_Right: mapwalker_ani.setFrameSequance(mapwalker_setup.wld_frames_right); break;
        case Walk_Up:    mapwalker_ani.setFrameSequance(mapwalker_setup.wld_frames_up); break;
        case Walk_Down:  mapwalker_ani.setFrameSequance(mapwalker_setup.wld_frames_down); break;
        default: break;
    }
}


