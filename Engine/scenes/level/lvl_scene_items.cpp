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

#include <audio/pge_audio.h>

//static double zCounter = 0;






void LevelScene::placeBlock(LevelBlock blockData)
{
    LVL_Block * block;
    block = new LVL_Block();
    if(ConfigManager::lvl_block_indexes.contains(blockData.id))
        block->setup = &ConfigManager::lvl_block_indexes[blockData.id];
    else
    {
        //Wrong block!
        delete block;
        return;
    }

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
        bgo->setup = &ConfigManager::lvl_bgo_indexes[bgoData.id];
    else
    {
        //Wrong BGO!
        delete bgo;
        return;
    }

    bgo->data = bgoData;
    bgo->transformTo_x(bgoData.id);
    bgo->init();

    bgos.push_back(bgo);
}

void LevelScene::placeNPC(LevelNPC npcData)
{
    LVL_Npc * npc;
    npc = new LVL_Npc();
    if(ConfigManager::lvl_npc_indexes.contains(npcData.id))
        npc->setup = &ConfigManager::lvl_npc_indexes[npcData.id];
    else
    {
        //Wrong NPC!
        delete npc;
        return;
    }

    npc->data = npcData;
    npc->transformTo_x(npcData.id);
    npc->init();

    npcs.push_back(npc);
}






void LevelScene::addPlayer(PlayerPoint playerData, bool byWarp, int warpType, int warpDirect)
{
    LVL_Player * player;
    if(luaEngine.isValid()){
        player = luaEngine.createLuaPlayer();
    }else{
        player = new LVL_Player();
    }

    if(players.size()==0)
        player->camera = &cameras.first();
    else if(players.size()==1)
        player->camera = &cameras.last();

    int sID = findNearestSection(playerData.x, playerData.y);
    LVL_Section *sct = getSection(sID);
    if(!sct)
    {
        delete player;
        return;
    }
    player->setParentSection(sct);
    player->z_index = Z_Player;
    player->setPlayerPointInfo(playerData);
    player->init();
    players.push_back(player);

    if(players.size()==1)
        player1Controller->registerInControl(player);
    else if(players.size()==2)
        player2Controller->registerInControl(player);

    if(byWarp)
    {
        player->setPaused(true);
        player->WarpTo(playerData.x, playerData.y, warpType, warpDirect);
        if(warpType==2)
            PGE_Audio::playSoundByRole(obj_sound_role::WarpDoor);
        else if(warpType==0)
            PGE_Audio::playSoundByRole(obj_sound_role::PlayerMagic);
    }
}


void LevelScene::destroyBlock(LVL_Block *&_block)
{
    blocks.removeAll(_block);
    delete _block;
    _block = NULL;
}


void LevelScene::toggleSwitch(int switch_id)
{
    PGE_Audio::playSoundByRole(obj_sound_role::BlockSwitch);
    if(switch_blocks.contains(switch_id))
    {
        QList<LVL_Block*> &list = switch_blocks[switch_id];
        for(int x=0;x<list.size();x++)
        {
            if((list[x]->setup->switch_Block)&&(list[x]->setup->switch_ID==switch_id))
                list[x]->transformTo(list[x]->setup->switch_transform, 2);
            else
            {
                list.removeAt(x); x--; //Remove blocks which no more is a switch block
            }
        }
    }
}
