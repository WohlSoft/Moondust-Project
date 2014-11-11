/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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


#include <QDebug>


bool LevelScene::setEntrance(int entr)
{

    if(entr<=0)
    {
        isWarpEntrance=false;
        bool found=false;
        for(int i=0;i<data.players.size(); i++)
        {
            if(data.players[i].w==0 && data.players[i].h==0)
                continue; //Skip empty points
            cameraStart.setX(data.players[i].x);
            cameraStart.setY(data.players[i].y);
            found=true;
            break;
        }

        if(!found) exitLevelCode = EXIT_Error;

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

                //cameraStart.setX(data.doors[i].ox);
                //cameraStart.setY(data.doors[i].oy);

                int playerW = 24;
                int playerH = 54;

                switch(startWarp.type)
                {
                case 0:
                case 2:
                    cameraStart.setX(startWarp.ox+16-playerW/2);
                    cameraStart.setY(startWarp.oy+32-playerH);
                    break;
                case 1:
                    switch(startWarp.odirect)
                    {
                        case 2://right
                            cameraStart.setX(startWarp.ox);
                            cameraStart.setY(startWarp.oy+32-playerH);
                            break;
                        case 1://down
                            cameraStart.setX(startWarp.ox+16-playerW/2);
                            cameraStart.setY(startWarp.oy);
                            break;
                        case 4://left
                            cameraStart.setX(startWarp.ox+32-playerW);
                            cameraStart.setY(startWarp.oy+32-playerH);
                            break;
                        case 3://up
                            cameraStart.setX(startWarp.ox+16-playerW/2);
                            cameraStart.setY(startWarp.oy+32-playerH);
                            break;
                        default:
                            break;
                    }
                    break;
                }

                return true;
            }

        }
    }

    isWarpEntrance=false;

    //Error, sections is not found
    exitLevelCode = EXIT_Error;
    return false;
}





bool LevelScene::loadConfigs()
{
    bool success=true;

    //Load INI-files
        loaderStep();
    success = ConfigManager::loadLevelBlocks(); //!< Blocks
        loaderStep();
    success = ConfigManager::loadLevelBGO();    //!< BGO
        loaderStep();
    success = ConfigManager::loadLevelBackG();  //!< Backgrounds

    //Set paths
    ConfigManager::Dir_Blocks.setCustomDirs(data.path, data.filename, ConfigManager::PathLevelBlock() );
    ConfigManager::Dir_BGO.setCustomDirs(data.path, data.filename, ConfigManager::PathLevelBGO() );
    ConfigManager::Dir_BG.setCustomDirs(data.path, data.filename, ConfigManager::PathLevelBG() );

    if(!success) exitLevelCode = EXIT_Error;

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

    qDebug()<<"Create cameras";

    loaderStep();

    //Init Cameras
    PGE_LevelCamera* camera;
    camera = new PGE_LevelCamera();
    camera->setWorld(world);
    camera->changeSection(data.sections[sID]);
    camera->isWarp = data.sections[sID].IsWarp;
    camera->section = &(data.sections[sID]);

    camera->init(
                    (float)cameraStart.x(),
                    (float)cameraStart.y(),
                    (float)PGE_Window::Width, (float)PGE_Window::Height
                );
    camera->setPos(cameraStart.x()-camera->w()/2 + 12,
                   cameraStart.y()-camera->h()/2 + 27);

    cameras.push_back(camera);

    LVL_Background * CurrentBackground = new LVL_Background(cameras.last());

    if(ConfigManager::lvl_bg_indexes.contains(camera->BackgroundID))
    {
        CurrentBackground->setBg(ConfigManager::lvl_bg_indexes[camera->BackgroundID]);
        qDebug() << "Backgroubnd ID:" << camera->BackgroundID;
    }
    else
        CurrentBackground->setNone();

    backgrounds.push_back(CurrentBackground);


    //Init data

    qDebug()<<"Init blocks";
    //blocks
    for(int i=0; i<data.blocks.size(); i++)
    {
        loaderStep();
        placeBlock(data.blocks[i]);
    }

    qDebug()<<"Init BGOs";
    //BGO
    for(int i=0; i<data.bgo.size(); i++)
    {
        loaderStep();
        placeBGO(data.bgo[i]);
    }

    qDebug()<<"Init Warps";
    //BGO
    for(int i=0; i<data.doors.size(); i++)
    {
        loaderStep();

        LVL_Warp * warpP;
        warpP = new LVL_Warp();
        warpP->worldPtr = world;
        warpP->data = data.doors[i];
        warpP->init();
        warps.push_back(warpP);
    }


    qDebug() << "textures " << ConfigManager::level_textures.size();


    qDebug()<<"Add players";

    int getPlayers = numberOfPlayers;
    int players_count=0;
    if(!isWarpEntrance) //Dont place players if entered through warp
        for(players_count=0; players_count<data.players.size() && getPlayers>0 ; players_count++)
        {
            loaderStep();

            int i = players_count;
            if(data.players[i].w==0 && data.players[i].h==0) continue;

            addPlayer(data.players[i]);

            getPlayers--;
        }

    if(players_count<0 && !isWarpEntrance)
    {
        qDebug()<<"No defined players!";
        return false;
    }

    //start animation
    for(int i=0; i<ConfigManager::Animator_Blocks.size(); i++)
        ConfigManager::Animator_Blocks[i].start();

    for(int i=0; i<ConfigManager::Animator_BGO.size(); i++)
        ConfigManager::Animator_BGO[i].start();

    for(int i=0; i<ConfigManager::Animator_BG.size(); i++)
        ConfigManager::Animator_BG[i].start();

    stopLoaderAnimation();
    isInit = true;

    return true;
}



