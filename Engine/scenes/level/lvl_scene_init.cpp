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

#include "../scene_level.h"
#include <data_configs/config_manager.h>
#include <settings/global_settings.h>
#include <common_features/logger.h>
#include <gui/pge_msgbox.h>

#include <QDebug>


bool LevelScene::setEntrance(int entr)
{
    player_defs.clear();
    int plr=1;
    foreach(int xxx, gameState->game_state.currentCharacter)
    {
        LVL_PlayerDef def;
        def.setPlayerID(plr);
        def.setCharacterID(xxx);
        for(int j=0; j<gameState->game_state.characterStates.size(); j++)
        {
            if(gameState->game_state.characterStates[j].id==xxx)
            {
                def.setState(gameState->game_state.characterStates[j].state);
                break;
            }
        }
        player_defs[plr] = def;
        plr++;
    }

    if((entr<=0) || (entr>data.doors.size()))
    {
        isWarpEntrance=false;
        bool found=false;
        for(int i=0; i<data.players.size(); i++)
        {
            if(data.players[i].w==0 && data.players[i].h==0)
                continue; //Skip empty points
            LVL_PlayerDef d = player_defs[data.players[i].id];
            cameraStart.setX( data.players[i].x+(data.players[i].w/2)-(d.width()/2) );
            cameraStart.setY( data.players[i].y+data.players[i].h-d.height() );
            found=true;
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
        for(int i=0;i<data.doors.size(); i++)
        {
            if(data.doors[i].array_id==(unsigned int)entr)
            {
                isWarpEntrance = true;
                startWarp = data.doors[i];

                cameraStartDirected = (startWarp.type==1);
                cameraStartDirection = startWarp.odirect;

                for(int i=1; i<=numberOfPlayers; i++)
                {
                    LVL_PlayerDef d = player_defs[i];
                    int w=d.width(), h=d.height();
                    EventQueueEntry<LevelScene >event3;
                    event3.makeCaller([this, i, w, h]()->void{
                                          PlayerPoint newPoint = getStartLocation(i);
                                          newPoint.id = i;
                                          newPoint.x=startWarp.ox;
                                          newPoint.y=startWarp.oy;
                                          newPoint.w = w;
                                          newPoint.h = h;
                                          newPoint.direction=1;
                                          this->addPlayer(newPoint, true, startWarp.type, startWarp.odirect);
                                          isWarpEntrance=false;
                                      }, 1000);
                    system_events.events.push_back(event3);
                }
                return true;
            }
        }
    }

    isWarpEntrance=false;

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
        if(this->isWarpEntrance) {
            //Construct spawn point with basing on camera position
            point.x=cameraStart.x();
            point.y=cameraStart.x();
            point.w=20;
            point.h=60;
        } else if(!data.sections.isEmpty()) {
            //Construct point based on first section boundary coordinates
            point.x=data.sections[0].size_left+20;
            point.y=data.sections[0].size_top+60;
            point.w = 20;
            point.h = 60;
        } else {
            //Construct null player point
            point.x=0;
            point.y=0;
            point.w=0;
            point.h=0;
        }
        point.direction=1;
        point.id=playerID;
        return point;
    }

    foreach(PlayerPoint p, data.players)
    {   //Return player ID specific spawn point
        if(p.id==(unsigned)playerID)
        {
            //Must not be null!
            if((p.w!=0)&&(p.h!=0))
                return p;
        }
    }

    if(playerID<=data.players.size())
    {
        PlayerPoint p = data.players[playerID-1];
        //Return spawn point by array index if not out of range [Not null]
        if((p.w!=0)&&(p.h!=0))
        {
            p.id=playerID;
            return data.players[playerID-1];
        }
    }

    foreach(PlayerPoint p, data.players)
    {   //Return first not null point
        if((p.w!=0)&&(p.h!=0))
        {
            p.id=playerID;
            return p;
        }
    }
    //Return first presented point entry even if null
    return data.players.first();
}





bool LevelScene::loadConfigs()
{
    bool success=true;

    QString musIni=data.path+"/music.ini";
    QString sndIni=data.path+"/sounds.ini";
    if(ConfigManager::music_lastIniFile!=musIni)
    {
        ConfigManager::loadDefaultMusics();
        ConfigManager::loadMusic(data.path+"/", musIni, true);
        loaderStep();
    }

    if(ConfigManager::sound_lastIniFile!=sndIni)
    {
        ConfigManager::loadDefaultSounds();
            loaderStep();
        ConfigManager::loadSound(data.path+"/", sndIni, true);
            loaderStep();
        if(ConfigManager::soundIniChanged())
            ConfigManager::buildSoundIndex();
    }

    //Load INI-files
        loaderStep();
    success = ConfigManager::loadLevelBlocks(); //!< Blocks
        loaderStep();
    success = ConfigManager::loadLevelBGO();    //!< BGO
        loaderStep();
    success = ConfigManager::loadLevelNPC();  //!< NPC
        loaderStep();
    success = ConfigManager::loadLevelBackG();  //!< Backgrounds
        loaderStep();
    success = ConfigManager::loadLevelEffects();  //!< Effects

    //Set paths
    ConfigManager::Dir_Blocks.setCustomDirs(data.path, data.filename, ConfigManager::PathLevelBlock() );
    ConfigManager::Dir_BGO.setCustomDirs(data.path, data.filename, ConfigManager::PathLevelBGO() );
    ConfigManager::Dir_NPC.setCustomDirs(data.path, data.filename, ConfigManager::PathLevelNPC() );
    ConfigManager::Dir_NPCScript.setCustomDirs(data.path, data.filename, ConfigManager::PathLevelNPCScript() );
    ConfigManager::Dir_BG.setCustomDirs(data.path, data.filename, ConfigManager::PathLevelBG() );
    ConfigManager::Dir_EFFECT.setCustomDirs(data.path, data.filename, ConfigManager::PathLevelEffect() );
    ConfigManager::Dir_PlayerLvl.setCustomDirs(data.path, data.filename, ConfigManager::PathLevelPlayable() );

    if(!success) exitLevelCode = LvlExit::EXIT_Error;

    return success;
}

bool LevelScene::init()
{
    luaEngine.setLuaScriptPath(ConfigManager::PathScript());
    luaEngine.setCoreFile(ConfigManager::setup_Level.luaFile);
    luaEngine.setErrorReporterFunc([this](const QString& errorMessage, const QString& stacktrace){
        WriteToLog(QtWarningMsg, "Lua-Error: ");
        WriteToLog(QtWarningMsg, "Error Message: "+errorMessage);
        WriteToLog(QtWarningMsg, "Stacktrace: \n"+stacktrace);
        PGE_MsgBox msgBox(this, QString("A lua error has been thrown: \n") + errorMessage + "\n\nMore details in the log!", PGE_MsgBox::msg_error);
        msgBox.exec();
    });
    luaEngine.init();

    if(luaEngine.shouldShutdown())
        return false;

    for(QMap<long, obj_npc>::const_iterator it = ConfigManager::lvl_npc_indexes.cbegin(); it != ConfigManager::lvl_npc_indexes.cend(); ++it){
        QString scriptPath = ConfigManager::Dir_NPCScript.getCustomFile((*it).algorithm_script);
        if( (!scriptPath.isEmpty()) && (QFileInfo(scriptPath).exists()) )
        {
            luaEngine.loadNPCClass((*it).id, scriptPath);
        }
    }


    qDebug()<<"Build sections";
    for(int i=0;i<data.sections.size();i++)
    {
        LVL_Section sct;
        sections.push_back(sct);
        sections.last().setData(data.sections[i]);
        sections.last().setMusicRoot(data.path);
    }

    qDebug()<<"Create cameras";
    loaderStep();
    //quit from game if window was closed
    if(!isLevelContinues) return false;

    for(int i=0; i<numberOfPlayers; i++)
    {
        int width  = PGE_Window::Width;
        int height = PGE_Window::Height/numberOfPlayers;

        LVL_PlayerDef d = player_defs[i+1];
        if(isWarpEntrance)
        {
            cameraStart.setX( startWarp.ox+16-(d.width()/2) );
            cameraStart.setY( startWarp.oy+32-d.height() );
            if(cameraStartDirected)
            {
                switch(startWarp.odirect)
                {
                    case 2://right
                        cameraStart.setX(startWarp.ox);
                        cameraStart.setY(startWarp.oy+32-d.height());
                        break;
                    case 1://down
                        cameraStart.setX(startWarp.ox+16-d.width()/2);
                        cameraStart.setY(startWarp.oy);
                        break;
                    case 4://left
                        cameraStart.setX(startWarp.ox+32-d.width());
                        cameraStart.setY(startWarp.oy+32-d.height());
                        break;
                    case 3://up
                        cameraStart.setX(startWarp.ox+16-d.width()/2);
                        cameraStart.setY(startWarp.oy+32-d.height());
                        break;
                    default:
                        break;
                }
            }
        }

        int sID = findNearestSection(cameraStart.x(), cameraStart.y());
        LVL_Section *t_sct = getSection(sID);
        int x=cameraStart.x();
        int y=cameraStart.y();
        //Init Cameras
        PGE_LevelCamera camera;
        camera.init(
                        (float)x,
                        (float)y,
                        (float)width, (float)height
                    );
        camera.changeSection(t_sct);
        camera.setPos(x-camera.w()/2 + d.width()/2,
                      y-camera.h()/2 + d.height()/2);
        cameras.push_back(camera);
    }

    //Init data

    qDebug()<<"Init blocks";
    //blocks
    for(int i=0; i<data.blocks.size(); i++)
    {
        loaderStep();
        if(!isLevelContinues) return false;//!< quit from game if window was closed
        placeBlock(data.blocks[i]);
    }

    qDebug()<<"Init BGOs";
    //BGO
    for(int i=0; i<data.bgo.size(); i++)
    {
        loaderStep();
        if(!isLevelContinues) return false;//!< quit from game if window was closed
        placeBGO(data.bgo[i]);
    }

    qDebug()<<"Init NPCs";

    //NPC
    for(int i=0; i<data.npc.size(); i++)
    {
        loaderStep();
        if(!isLevelContinues) return false;//!< quit from game if window was closed
        placeNPC(data.npc[i]);
    }


    qDebug()<<"Init Warps";
    //BGO
    for(int i=0; i<data.doors.size(); i++)
    {
        loaderStep();
        if(!isLevelContinues) return false;//!< quit from game if window was closed

        //Don't put contactable points for "level entrance" points
        if(data.doors[i].lvl_i) continue;

        LVL_Warp * warpP;
        warpP = new LVL_Warp();
        //warpP->worldPtr = world;
        warpP->data = data.doors[i];
        warpP->init();

        int sID = findNearestSection(warpP->posX(), warpP->posY());
        LVL_Section *sct = getSection(sID);
        if(sct)
        {
            warpP->setParentSection(sct);
        }
        warpP->_syncPosition();

        warps.push_back(warpP);
    }

    qDebug()<<"Init Physical environments";
    //BGO
    for(int i=0; i<data.physez.size(); i++)
    {
        loaderStep();
        if(!isLevelContinues) return false;//!< quit from game if window was closed

        LVL_PhysEnv * physesP;
        physesP = new LVL_PhysEnv();
        //physesP->worldPtr = world;
        physesP->data = data.physez[i];
        physesP->init();
        physesP->_syncPosition();
        physenvs.push_back(physesP);
    }

    qDebug() << "Total textures loaded: " << ConfigManager::level_textures.size();

    qDebug() << "Add players";

    int added_players=0;
    if(!isWarpEntrance) //Dont place players if entered through warp
        for(int i=1; i <= numberOfPlayers; i++)
        {
            loaderStep();
            if(!isLevelContinues) return false;//!< quit from game if window was closed
            PlayerPoint startPoint = getStartLocation(i);
            startPoint.id=i;

            //Don't place player if point is null!
            if(startPoint.w==0 && startPoint.h==0) continue;

            addPlayer(startPoint);
            added_players++;
        }

    if(added_players<=0 && !isWarpEntrance)
    {
        qDebug()<<"No defined players!";
        return false;
    }

    stopLoaderAnimation();

    isInit = true;
    return true;
}



