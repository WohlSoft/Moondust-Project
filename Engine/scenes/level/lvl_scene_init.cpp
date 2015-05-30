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
#include "../../data_configs/config_manager.h"
#include "../../physics/contact_listener.h"
#include <settings/global_settings.h>

#include "../../gui/pge_msgbox.h"

#include <QDebug>


bool LevelScene::setEntrance(int entr)
{
    player_defs.clear();

    foreach(int xxx, gameState->game_state.currentCharacter)
    {
        LVL_PlayerDef def;
        def.setCharacterID(xxx);
        if(gameState->game_state.characterStates.size()-1 > xxx-1)
            def.setState(gameState->game_state.characterStates[xxx].state+1);
        else
            def.setState(1);
        player_defs.push_back(def);
    }

    if((entr<=0) || (entr>data.doors.size()))
    {
        isWarpEntrance=false;
        bool found=false;
        for(int i=0;i<data.players.size(); i++)
        {
            if(data.players[i].w==0 && data.players[i].h==0)
                continue; //Skip empty points
            cameraStart.setX( data.players[i].x+(data.players[i].w/2)-(player_defs.first().width()/2) );
            cameraStart.setY( data.players[i].y+data.players[i].w-player_defs.first().height() );
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

                cameraStart.setX( startWarp.ox+16-(player_defs.first().width()/2) );
                cameraStart.setY( startWarp.oy+32-player_defs.first().height() );

                cameraStartDirected = (startWarp.type==1);
                cameraStartDirection = startWarp.odirect;
                if(cameraStartDirected)
                {
                    switch(startWarp.odirect)
                    {
                        case 2://right
                            cameraStart.setX(startWarp.ox);
                            cameraStart.setY(startWarp.oy+32-player_defs.first().height());
                            break;
                        case 1://down
                            cameraStart.setX(startWarp.ox+16-player_defs.first().width()/2);
                            cameraStart.setY(startWarp.oy);
                            break;
                        case 4://left
                            cameraStart.setX(startWarp.ox+32-player_defs.first().width());
                            cameraStart.setY(startWarp.oy+32-player_defs.first().height());
                            break;
                        case 3://up
                            cameraStart.setX(startWarp.ox+16-player_defs.first().width()/2);
                            cameraStart.setY(startWarp.oy+32-player_defs.first().height());
                            break;
                        default:
                            break;
                    }
                }

                for(int i=1; i<=numberOfPlayers; i++)
                {
                    EventQueueEntry<LevelScene >event3;
                    event3.makeCaller([this, i]()->void{
                                          PlayerPoint newPoint = getStartLocation(i);
                                          newPoint.id = i;
                                          newPoint.x=startWarp.ox;
                                          newPoint.y=startWarp.oy;
                                          newPoint.w = this->player_defs.first().width();
                                          newPoint.h = this->player_defs.first().height();
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
    success = ConfigManager::loadLevelBackG();  //!< Backgrounds
        loaderStep();
    success = ConfigManager::loadLevelEffects();  //!< Effects

    //Set paths
    ConfigManager::Dir_Blocks.setCustomDirs(data.path, data.filename, ConfigManager::PathLevelBlock() );
    ConfigManager::Dir_BGO.setCustomDirs(data.path, data.filename, ConfigManager::PathLevelBGO() );
    ConfigManager::Dir_BG.setCustomDirs(data.path, data.filename, ConfigManager::PathLevelBG() );
    ConfigManager::Dir_EFFECT.setCustomDirs(data.path, data.filename, ConfigManager::PathLevelEffect() );
    ConfigManager::Dir_PlayerLvl.setCustomDirs(data.path, data.filename, ConfigManager::PathLevelPlayable() );

    if(!success) exitLevelCode = LvlExit::EXIT_Error;

    return success;
}








bool LevelScene::init()
{
    //Load File

    //Set Entrance  (int entr=0)

    //Init Physics
    b2Vec2 gravity(0.0f, 150.0f);
    world = new b2World(gravity);
    world->SetAllowSleeping(true);

    PGEContactListener *contactListener;

    contactListener = new PGEContactListener();
    world->SetContactListener(contactListener);

    int sID = findNearSection(cameraStart.x(), cameraStart.y());

    qDebug()<<"Build sections";
    for(int i=0;i<data.sections.size();i++)
    {
        LVL_Section sct;
        sections.push_back(sct);
        sections.last().setData(data.sections[i]);
        sections.last().setMusicRoot(data.path);
    }

    LVL_Section *t_sct = getSection(sID);

    qDebug()<<"Create cameras";
    loaderStep();
    //quit from game if window was closed
    if(!isLevelContinues) return false;

    for(int i=0; i<numberOfPlayers; i++)
    {
        int x=cameraStart.x();
        int y=cameraStart.y();
        int width=PGE_Window::Width;
        int height=PGE_Window::Height/numberOfPlayers;

        //Init Cameras
        PGE_LevelCamera* camera;
        camera = new PGE_LevelCamera();
        camera->setWorld(world);
        camera->init(
                        (float)x,
                        (float)y,
                        (float)width, (float)height
                    );
        camera->changeSection(t_sct);
        camera->setPos(cameraStart.x()-camera->w()/2 + player_defs.first().width()/2,
                       cameraStart.y()-camera->h()/2 + player_defs.first().height()/2);

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
        if(!isLevelContinues) return false;
        placeBGO(data.bgo[i]);
    }

    qDebug()<<"Init NPCs";
    if(AppSettings.enableDummyNpc)
    {
        //NPC
        for(int i=0; i<data.npc.size(); i++)
        {
            loaderStep();
            if(!isLevelContinues) return false;//!< quit from game if window was closed
            if(!isLevelContinues) return false;
            placeNPC(data.npc[i]);
        }
    }
    else
    {
        //NPC
        for(int i=0; i<data.npc.size(); i++)
        {
            if(
               (data.npc[i].id!=11)&&

               (data.npc[i].id!=15)&&
               (data.npc[i].id!=86)&&
               (data.npc[i].id!=39)&&

               (data.npc[i].id!=41)&&
               (data.npc[i].id!=16)&&
               (data.npc[i].id!=97)&&
               (data.npc[i].id!=197)
                    )
                continue;
            loaderStep();
            if(!isLevelContinues) return false;//!< quit from game if window was closed
            if(!isLevelContinues) return false;
            placeNPC(data.npc[i]);
        }
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
        warpP->worldPtr = world;
        warpP->data = data.doors[i];
        warpP->init();

        int sID = findNearSection(warpP->posX(), warpP->posY());
        LVL_Section *sct = getSection(sID);
        if(sct)
        {
            warpP->setParentSection(sct);
            warpP->_syncBox2dWithPos();
        }

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
        physesP->worldPtr = world;
        physesP->data = data.physez[i];
        physesP->init();
        physenvs.push_back(physesP);
    }


    qDebug() << "textures " << ConfigManager::level_textures.size();


    qDebug()<<"Add players";

    int added_players=0;
    if(!isWarpEntrance) //Dont place players if entered through warp
        for(int i=1; i <= numberOfPlayers; i++)
        {
            loaderStep();
            if(!isLevelContinues) return false;//!< quit from game if window was closed
            PlayerPoint startPoint = getStartLocation(i);

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

    //Build switch blocks structure
    for(int i=0; i<blocks.size(); i++)
    {
        if(blocks[i]->setup->switch_Block)
        {
            if(!switch_blocks.contains(blocks[i]->setup->switch_ID) )
                switch_blocks[blocks[i]->setup->switch_ID].clear();
            switch_blocks[blocks[i]->setup->switch_ID].push_back(blocks[i]);
        }
    }

//    //start animation
//    for(int i=0; i<ConfigManager::Animator_Blocks.size(); i++)
//        ConfigManager::Animator_Blocks[i].start();
//    for(int i=0; i<ConfigManager::Animator_BGO.size(); i++)
//        ConfigManager::Animator_BGO[i].start();
//    for(int i=0; i<ConfigManager::Animator_BG.size(); i++)
//        ConfigManager::Animator_BG[i].start();

    stopLoaderAnimation();
    isInit = true;

    return true;
}



