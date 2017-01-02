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

#include "../scene_level.h"
#include <data_configs/config_manager.h>
#include <settings/global_settings.h>
#include <common_features/logger.h>
#include <Utils/maths.h>
#include <gui/pge_msgbox.h>

#include <QDebug>
#include <QFileInfo>
#include <functional>
#include <algorithm>

bool LevelScene::setEntrance(unsigned long entr)
{
    player_defs.clear();
    unsigned int plr = 1;

    for(unsigned long &xxx : gameState->game_state.currentCharacter)
    {
        LVL_PlayerDef def;
        def.setPlayerID(static_cast<int>(plr));
        def.setCharacterID(xxx);

        for(int j = 0; j < gameState->game_state.characterStates.size(); j++)
        {
            if(gameState->game_state.characterStates[j].id == xxx)
            {
                def.setState(gameState->game_state.characterStates[j].state);
                break;
            }
        }

        player_defs[plr] = def;
        plr++;
    }

    if((entr == 0) || (entr > static_cast<unsigned long>(data.doors.size())))
    {
        isWarpEntrance = false;
        bool found = false;

        for(int i = 0, j = 0; (i < data.players.size()) && (j < numberOfPlayers); i++)
        {
            if(data.players[i].w == 0 && data.players[i].h == 0)
                continue; //Skip empty points

            LVL_PlayerDef d = player_defs[data.players[i].id];
            cameraStart.setX(data.players[i].x + (data.players[i].w / 2) - (d.width() / 2));
            cameraStart.setY(data.players[i].y +  data.players[i].h    -  d.height());
            found = true;
            j++;
            break;
        }

        if(!found)
        {
            exitLevelCode = LvlExit::EXIT_Error;
            PGE_MsgBox msgBox(NULL, "ERROR:\nCan't start level without player's start point.\nPlease set a player's start point and start level again.",
                              PGE_MsgBox::msg_error);
            msgBox.exec();
        }

        //Find available start points
        return found;
    }
    else
    {
        for(int i = 0; i < data.doors.size(); i++)
        {
            if(data.doors[i].meta.array_id == static_cast<unsigned int>(entr))
            {
                isWarpEntrance = true;
                startWarp = data.doors[i];
                cameraStartDirected = (startWarp.type == 1);
                cameraStartDirection = startWarp.odirect;

                for(int i = 1; i <= numberOfPlayers; i++)
                {
                    LVL_PlayerDef d = player_defs[static_cast<unsigned int>(i)];
                    int w = d.width(), h = d.height();
                    EventQueueEntry<LevelScene >event3;
                    event3.makeCaller([this, i, w, h]()->void
                    {
                        PlayerPoint newPoint = getStartLocation(i);
                        newPoint.id = static_cast<unsigned>(i);
                        newPoint.x = startWarp.ox;
                        newPoint.y = startWarp.oy;
                        newPoint.w = w;
                        newPoint.h = h;
                        newPoint.direction = 1;
                        this->addPlayer(newPoint, true,
                        startWarp.type, startWarp.odirect,
                        startWarp.cannon_exit, startWarp.cannon_exit_speed);
                        isWarpEntrance = false;
                    }, 1000);
                    system_events.events.push_back(event3);
                }

                return true;
            }
        }
    }

    isWarpEntrance = false;
    PGE_MsgBox msgBox(NULL, "ERROR:\nTarget section is not found.\nMayby level is empty.",
                      PGE_MsgBox::msg_error);
    msgBox.exec();
    //Error, sections is not found
    exitLevelCode = LvlExit::EXIT_Error;
    return false;
}

PlayerPoint LevelScene::getStartLocation(int playerID)
{
    //If no placed player star points
    if(data.players.isEmpty())
    {
        PlayerPoint point;

        if(this->isWarpEntrance)
        {
            //Construct spawn point with basing on camera position
            point.x = static_cast<long>(cameraStart.x());
            point.y = static_cast<long>(cameraStart.x());
            point.w = 20;
            point.h = 60;
        }
        else if(!data.sections.isEmpty())
        {
            //Construct point based on first section boundary coordinates
            point.x = data.sections[0].size_left + 20;
            point.y = data.sections[0].size_top + 60;
            point.w = 20;
            point.h = 60;
        }
        else
        {
            //Construct null player point
            point.x = 0;
            point.y = 0;
            point.w = 0;
            point.h = 0;
        }

        point.direction = 1;
        point.id = playerID;
        return point;
    }

    for(PlayerPoint &p : data.players)
    {
        //Return player ID specific spawn point
        if(p.id == unsigned(playerID))
        {
            //Must not be null!
            if((p.w != 0) && (p.h != 0))
                return p;
        }
    }

    if(playerID <= data.players.size())
    {
        PlayerPoint p = data.players[playerID - 1];

        //Return spawn point by array index if not out of range [Not null]
        if((p.w != 0) && (p.h != 0))
        {
            p.id = playerID;
            return data.players[playerID - 1];
        }
    }

    for(PlayerPoint &p : data.players)
    {
        //Return first not null point
        if((p.w != 0) && (p.h != 0))
        {
            p.id = playerID;
            return p;
        }
    }

    //Return first presented point entry even if null
    return data.players.first();
}





bool LevelScene::loadConfigs()
{
    bool success = true;
    QString musIni = data.meta.path + "/music.ini";
    QString sndIni = data.meta.path + "/sounds.ini";

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
    ConfigManager::Dir_Blocks.setCustomDirs(data.meta.path, data.meta.filename, ConfigManager::PathLevelBlock());
    ConfigManager::Dir_BGO.setCustomDirs(data.meta.path, data.meta.filename, ConfigManager::PathLevelBGO());
    ConfigManager::Dir_NPC.setCustomDirs(data.meta.path, data.meta.filename, ConfigManager::PathLevelNPC());
    ConfigManager::Dir_NPCScript.setCustomDirs(data.meta.path, data.meta.filename, ConfigManager::PathLevelNPCScript());
    ConfigManager::Dir_PlayerScript.setCustomDirs(data.meta.path, data.meta.filename, ConfigManager::PathLevelPlayerScript());
    ConfigManager::Dir_BG.setCustomDirs(data.meta.path, data.meta.filename, ConfigManager::PathLevelBG());
    ConfigManager::Dir_EFFECT.setCustomDirs(data.meta.path, data.meta.filename, ConfigManager::PathLevelEffect());
    ConfigManager::Dir_PlayerLvl.setCustomDirs(data.meta.path, data.meta.filename, ConfigManager::PathLevelPlayable());
    //Load INI-files
    success = ConfigManager::loadLevelBlocks(); //!< Blocks

    if(!success)
    {
        exitLevelCode = LvlExit::EXIT_Error;
        goto abortInit;
    }

    success = ConfigManager::loadLevelBGO();    //!< BGO

    if(!success)
    {
        exitLevelCode = LvlExit::EXIT_Error;
        goto abortInit;
    }

    success = ConfigManager::loadLevelNPC();  //!< NPC

    if(!success)
    {
        exitLevelCode = LvlExit::EXIT_Error;
        goto abortInit;
    }

    success = ConfigManager::loadLevelBackG();  //!< Backgrounds

    if(!success)
    {
        exitLevelCode = LvlExit::EXIT_Error;
        goto abortInit;
    }

    success = ConfigManager::loadLevelEffects();  //!< Effects

    if(!success)
    {
        exitLevelCode = LvlExit::EXIT_Error;
        goto abortInit;
    }

    //Validate all playable characters until use game state!
    if(gameState)
    {
        for(int i = 1; i <= numberOfPlayers; i++)
        {
            PlayerState st = gameState->getPlayerState(i);

            if(!ConfigManager::playable_characters.contains(st.characterID))
            {
                //% "Invalid playable character ID"
                _errorString = qtTrId("ERROR_LVL_UNKNOWN_PL_CHARACTER") + " "
                               + QString::number(st.characterID);
                errorMsg = _errorString;
                success = false;
                break;
            }
            else if(!ConfigManager::playable_characters[st.characterID].states.contains(st.stateID))
            {
                //% "Invalid playable character state ID"
                _errorString = qtTrId("ERROR_LVL_UNKNOWN_PL_STATE") + " "
                               + QString::number(st.stateID);
                errorMsg = _errorString;
                success = false;
                break;
            }
        }
    }

    if(!success) exitLevelCode = LvlExit::EXIT_Error;

abortInit:
    return success;
}

bool LevelScene::init_items()
{
    //Global script path
    luaEngine.setLuaScriptPath(ConfigManager::PathScript());
    //Episode path
    luaEngine.appendLuaScriptPath(data.meta.path);
    //Level custom path
    luaEngine.appendLuaScriptPath(data.meta.path + "/" + data.meta.filename);
    luaEngine.setFileSearchPath(data.meta.path + "/" + data.meta.filename);
    luaEngine.setCoreFile(":/script/maincore_level.lua");
    luaEngine.setUserFile(ConfigManager::setup_Level.luaFile);
    luaEngine.setNpcBaseClassPath(":/script/npcs/maincore_npc.lua");
    luaEngine.setPlayerBaseClassPath(":/script/player/maincore_player.lua");
    luaEngine.setErrorReporterFunc([this](const QString & errorMessage, const QString & stacktrace)->void
    {
        pLogWarning("Lua-Error: ");
        pLogWarning("Error Message: %s", errorMessage.toUtf8().data());
        pLogWarning("Stacktrace: \n%s", stacktrace.toUtf8().data());
        _errorString = QString("A lua error has been thrown: \n") + errorMessage + "\n\nMore details in the log!";
        //return false;
    });
    luaEngine.init();

    if(luaEngine.shouldShutdown())
        return false;

    for(unsigned long i = 1; i < ConfigManager::lvl_npc_indexes.size(); i++)
    {
        obj_npc &npc = ConfigManager::lvl_npc_indexes[i];
        QString scriptPath = ConfigManager::Dir_NPCScript.getCustomFile(npc.setup.algorithm_script);

        if((!scriptPath.isEmpty()) && (QFileInfo(scriptPath).exists()))
            luaEngine.loadNPCClass(npc.setup.id, scriptPath);
    }

    for(unsigned long i = 1; i < ConfigManager::playable_characters.size(); i++)
    {
        obj_player &player = ConfigManager::playable_characters[i];
        QString scriptPath = ConfigManager::Dir_PlayerScript.getCustomFile(player.script);

        if((!scriptPath.isEmpty()) && (QFileInfo(scriptPath).exists()))
            luaEngine.loadPlayerClass(player.id, scriptPath);
    }

    zCounter = 0.0L;
    D_pLogDebug("Build sections");

    for(int i = 0; i < data.sections.size(); i++)
    {
        LVL_Section sct;
        sections.push_back(sct);
        sections.last().setData(data.sections[i]);
        sections.last().setMusicRoot(data.meta.path);
    }

    D_pLogDebug("Create cameras");

    //quit from game if window was closed
    if(!isLevelContinues) return false;

    for(int i = 0; i < numberOfPlayers; i++)
    {
        int width  = PGE_Window::Width;
        int height = PGE_Window::Height / numberOfPlayers;
        LVL_PlayerDef d = player_defs[static_cast<const unsigned int>(i + 1)];

        if(isWarpEntrance)
        {
            cameraStart.setX(startWarp.ox + 16 - (d.width() / 2));
            cameraStart.setY(startWarp.oy + 32 - d.height());

            if(cameraStartDirected)
            {
                switch(startWarp.odirect)
                {
                case 2://right
                    cameraStart.setX(startWarp.ox);
                    cameraStart.setY(startWarp.oy + 32 - d.height());
                    break;

                case 1://down
                    cameraStart.setX(startWarp.ox + 16 - d.width() / 2);
                    cameraStart.setY(startWarp.oy);
                    break;

                case 4://left
                    cameraStart.setX(startWarp.ox + 32 - d.width());
                    cameraStart.setY(startWarp.oy + 32 - d.height());
                    break;

                case 3://up
                    cameraStart.setX(startWarp.ox + 16 - d.width() / 2);
                    cameraStart.setY(startWarp.oy + 32 - d.height());
                    break;

                default:
                    break;
                }
            }
        }

        int sID = findNearestSection(Maths::lRound(cameraStart.x()),
                                     Maths::lRound(cameraStart.y()));
        LVL_Section *t_sct = getSection(sID);

        if(!t_sct)
        {
            /*% "Fatal error: Impossible to find start section.\n"
                "Did you placed player start point (or entrance warp point) too far off of the section(s)?" */
            _errorString = qtTrId("LVL_ERROR_NOSECTIONS");
            errorMsg = _errorString;
            return false;
        }

        double x = cameraStart.x();
        double y = cameraStart.y();
        //Init Cameras
        PGE_LevelCamera camera(this);
        camera.init(x, y, width, height);
        camera.playerID = (i + 1);
        camera.setRenderPos(0.0, (height * i));
        camera.setRenderObjectsCacheEnabled(i == 0);
        camera.changeSection(t_sct, true);
        camera.setPos(x - camera.w() / 2 + d.width() / 2,
                      y - camera.h() / 2 + d.height() / 2);
        cameras.push_back(camera);
        lua_LevelPlayerState luaPlState(this, (i + 1));
        player_states.push_back(luaPlState);
    }

    //Init data
    //blocks
    for(int i = 0; i < data.blocks.size(); i++)
    {
        if(!isLevelContinues)
            return false;//!< quit from game if window was closed

        placeBlock(data.blocks[i]);
    }

    //Build character switchers and configure switches and filters
    character_switchers.refreshState();

    //BGO
    for(int i = 0; i < data.bgo.size(); i++)
    {
        if(!isLevelContinues) return false;//!< quit from game if window was closed

        placeBGO(data.bgo[i]);
    }

    //NPC
    for(int i = 0; i < data.npc.size(); i++)
    {
        if(!isLevelContinues) return false;//!< quit from game if window was closed

        placeNPC(data.npc[i]);
    }

    //BGO
    for(int i = 0; i < data.doors.size(); i++)
    {
        if(!isLevelContinues) return false;//!< quit from game if window was closed

        //Don't put contactable points for "level entrance" points
        if(data.doors[i].lvl_i) continue;

        LVL_Warp *warpP;
        LevelDoor door = data.doors[i];
        bool two_way_opposite = false;
place_door_again:
        warpP = new LVL_Warp(this);
        warpP->data = door;
        warpP->init();
        int sID = findNearestSection(Maths::lRound(warpP->posX()),
                                     Maths::lRound(warpP->posY()));
        LVL_Section *sct = getSection(sID);

        if(sct)
            warpP->setParentSection(sct);

        warpP->_syncPosition();
        warps.push_back(warpP);

        if(!two_way_opposite && door.two_way)//Place opposite entrance point
        {
            two_way_opposite = true;
            std::swap(door.idirect,  door.odirect);
            std::swap(door.ix,       door.ox);
            std::swap(door.iy,       door.oy);
            std::swap(door.length_i, door.length_o);
            goto place_door_again;
        }
    }

    //BGO
    for(int i = 0; i < data.physez.size(); i++)
    {
        if(!isLevelContinues) return false;//!< quit from game if window was closed

        LVL_PhysEnv *physesP;
        physesP = new LVL_PhysEnv(this);
        physesP->data = data.physez[i];
        physesP->init();
        physesP->_syncPosition();
        physenvs.push_back(physesP);
    }

    D_pLogDebug("Total textures loaded: %d", ConfigManager::level_textures.size());
    int added_players = 0;

    if(!isWarpEntrance) //Dont place players if entered through warp
        for(int i = 1; i <= numberOfPlayers; i++)
        {
            if(!isLevelContinues) return false;//!< quit from game if window was closed

            PlayerPoint startPoint = getStartLocation(i);
            startPoint.id = static_cast<unsigned int>(i);

            //Don't place player if point is null!
            if(startPoint.w == 0 && startPoint.h == 0) continue;

            addPlayer(startPoint);
            added_players++;
        }

    if(added_players <= 0 && !isWarpEntrance)
    {
        _errorString = "No defined players!";
        errorMsg = _errorString;
        return false;
    }

    D_pLogDebug("Apply layers");

    for(int i = 0; i < data.layers.size(); i++)
    {
        if(data.layers[i].hidden)
            layers.hide(data.layers[i].name, false);
    }

    D_pLogDebug("Apply Events");

    for(int i = 0; i < data.events.size(); i++)
        events.addSMBX64Event(data.events[i]);

    isInit = true;
    return true;
}


bool LevelScene::init()
{
    isInitFinished = false;
    isInitFailed = false;
    #if 0
    SDL_GL_MakeCurrent(PGE_Window::window, PGE_Window::glcontext_background);
    initializer_thread = SDL_CreateThread(init_thread, "LevelInitializer", this);
    setLoaderAnimation(62);

    while(!isInitFinished)
    {
        drawLoader();
        PGE_Window::rePaint();
        //SDL_PumpEvents();
        SDL_Delay(20);
    }

    stopLoaderAnimation();
    SDL_GL_MakeCurrent(PGE_Window::window, PGE_Window::glcontext);
    #else
    //Load everything without loading animation, in the main thread
    //(because in the threaded loading some issues are appearence)
    init_thread(this);
    #endif

    if(isInitFailed)
        PGE_MsgBox::error(_errorString);

    return !isInitFailed;
}


int LevelScene::init_thread(void *self)
{
    SDL_GL_MakeCurrent(PGE_Window::window, PGE_Window::glcontext);
    LevelScene *_self = static_cast<LevelScene *>(self);

    if(!_self)
        _self->isInitFailed = true;
    else if(!_self->loadConfigs())
        _self->isInitFailed = true;
    else if(!_self->init_items())
        _self->isInitFailed = true;

    _self->isInitFinished = true;
    return 0;
}
