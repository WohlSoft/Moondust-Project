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


//static double zCounter = 0;






void LevelScene::placeBlock(LevelBlock blockData)
{
    LVL_Block * block;
    block = new LVL_Block();
    if(ConfigManager::lvl_block_indexes.contains(blockData.id))
        block->setup = &(ConfigManager::lvl_block_indexes[blockData.id]);
    else
    {
        //Wrong block!
        delete block;
        return;
    }

    block->worldPtr = world;
    block->data = blockData;

    block->transformTo_x(blockData.id);
    block->init();
    blocks.push_back(block);

}







void LevelScene::placeBGO(LevelBGO bgoData)
{
    LVL_Bgo * bgo;
    bgo = new LVL_Bgo();
    if(ConfigManager::lvl_bgo_indexes.contains(bgoData.id))
        bgo->setup = &(ConfigManager::lvl_bgo_indexes[bgoData.id]);
    else
    {
        //Wrong BGO!
        delete bgo;
        return;
    }

    bgo->worldPtr = world;
    bgo->data = &(bgoData);

    double targetZ = 0;
    double zOffset = bgo->setup->zOffset;
    int zMode = bgo->data->z_mode;

    if(zMode==LevelBGO::ZDefault)
        zMode = bgo->setup->view;
    switch(zMode)
    {
        case LevelBGO::Background2:
            targetZ = Z_BGOBack2 + zOffset + bgo->data->z_offset; break;
        case LevelBGO::Background1:
            targetZ = Z_BGOBack1 + zOffset + bgo->data->z_offset; break;
        case LevelBGO::Foreground1:
            targetZ = Z_BGOFore1 + zOffset + bgo->data->z_offset; break;
        case LevelBGO::Foreground2:
            targetZ = Z_BGOFore2 + zOffset + bgo->data->z_offset; break;
        default:
            targetZ = Z_BGOBack1 + zOffset + bgo->data->z_offset; break;
    }

    bgo->z_index += targetZ;

    zCounter += 0.0000000000001;
    bgo->z_index += zCounter;

    long tID = ConfigManager::getBgoTexture(bgoData.id);
    if( tID >= 0 )
    {
        bgo->texId = ConfigManager::level_textures[tID].texture;
        bgo->texture = ConfigManager::level_textures[tID];
        bgo->animated = ConfigManager::lvl_bgo_indexes[bgoData.id].animated;
        bgo->animator_ID = ConfigManager::lvl_bgo_indexes[bgoData.id].animator_ID;
    }
    bgo->init();

    bgos.push_back(bgo);
}






void LevelScene::addPlayer(PlayerPoint playerData, bool byWarp)
{
    //if(effect==0) //Simple Appear
    //if(effect==1) //Entered from pipe
    //if(effect==2) //Entered from door

    LVL_Player * player;
    player = new LVL_Player();
    player->camera = cameras.last();
    player->worldPtr = world;
    player->z_index = Z_Player;
    player->data = playerData;

    player->initSize();

    if(byWarp)
    {
        if(cameraStartDirected)
        {
            switch(startWarp.odirect)
            {
                case 2://right
                    cameraStart.setX(startWarp.ox);
                    cameraStart.setY(startWarp.oy+32-player->height);
                    break;
                case 1://down
                    cameraStart.setX(startWarp.ox+16-player->width/2);
                    cameraStart.setY(startWarp.oy);
                    break;
                case 4://left
                    cameraStart.setX(startWarp.ox+32-player->width);
                    cameraStart.setY(startWarp.oy+32-player->height);
                    break;
                case 3://up
                    cameraStart.setX(startWarp.ox+16-player->width/2);
                    cameraStart.setY(startWarp.oy+32-player->height);
                    break;
                default:
                    break;
            }
        }
        else
        {
            cameraStart.setX(startWarp.ox+16-player->width/2);
            cameraStart.setY(startWarp.oy+32-player->height);
        }
        player->data.x = cameraStart.x();
        player->data.y = cameraStart.y();
    }

    player->init();
    players.push_back(player);

    if(player->playerID==1)
        keyboard1.registerInControl(player);
}


void LevelScene::destroyBlock(LVL_Block *_block)
{
    blocks.remove(blocks.indexOf(_block));
    delete _block;
    _block = NULL;
}



void LevelScene::toggleSwitch(int switch_id)
{
    if(switch_blocks.contains(switch_id))
    {
        for(int x=0;x<switch_blocks[switch_id].size();x++)
        {
            if((switch_blocks[switch_id][x]->setup->switch_Block)&&
                    (switch_blocks[switch_id][x]->setup->switch_ID==switch_id))
                switch_blocks[switch_id][x]->
                    transformTo(
                            switch_blocks[switch_id][x]->setup->switch_transform,
                            2);
        }
    }
}

