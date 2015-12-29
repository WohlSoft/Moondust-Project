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
#include <PGE_File_Formats/file_formats.h>
#include <QFileInfo>


void LevelScene::placeBlock(LevelBlock blockData)
{
    if(blockData.id<=0) return;
    if(!ConfigManager::lvl_block_indexes.contains(blockData.id))
        return;

    LVL_Block * block;
    block = new LVL_Block(this);
    if(!block) throw("Out of memory [new LVL_Block place]");
    block->data = blockData;
    block->init();
    blocks.push_back(block);
}

LVL_Block * LevelScene::spawnBlock(LevelBlock blockData)
{
    if(blockData.id<=0) return NULL;
    if(!ConfigManager::lvl_block_indexes.contains(blockData.id))
        return NULL;
    LVL_Block * block;
    blockData.array_id= ++data.blocks_array_id;
    block = new LVL_Block(this);
    if(!block) throw("Out of memory [new LVL_Block spawn]");
    block->data = blockData;
    block->init();
    blocks.push_back(block);
    return block;
}

void LevelScene::destroyBlock(LVL_Block *&_block)
{
    #if (QT_VERSION >= 0x050400)
    blocks.removeAll(_block);
    #else
    while(1)
    {
        const QVector<LVL_Block *>::const_iterator ce = blocks.cend(), cit = std::find(blocks.cbegin(), ce, _block);
        if (cit == ce)
            break;
        const QVector<LVL_Block *>::iterator e = blocks.end(), it = std::remove(blocks.begin() + (cit - blocks.cbegin()), e, _block);
        blocks.erase(it, e);
        break;
    }
    #endif
    delete _block;
    _block = NULL;
}

void LevelScene::placeBGO(LevelBGO bgoData)
{
    if(bgoData.id<=0) return;
    if(!ConfigManager::lvl_bgo_indexes.contains(bgoData.id))
        return;

    LVL_Bgo * bgo;
    bgo = new LVL_Bgo(this);
    if(!bgo) throw("Out of memory [new LVL_Bgo place]");
    bgo->data = bgoData;
    bgo->init();
    bgos.push_back(bgo);
}

LVL_Bgo* LevelScene::spawnBGO(LevelBGO bgoData)
{
    if(bgoData.id<=0) return NULL;
    if(!ConfigManager::lvl_bgo_indexes.contains(bgoData.id))
        return NULL;
    bgoData.array_id= ++data.blocks_array_id;
    LVL_Bgo * bgo;
    bgo = new LVL_Bgo(this);
    if(!bgo) throw("Out of memory [new LVL_Bgo] spawn");
    bgo->data = bgoData;
    bgo->init();
    bgos.push_back(bgo);
    return bgo;
}

void LevelScene::placeNPC(LevelNPC npcData)
{
    if(npcData.id<=0) return;
    if(!ConfigManager::lvl_npc_indexes.contains(npcData.id))
        return;

    obj_npc* curNpcData = &ConfigManager::lvl_npc_indexes[npcData.id];
    LVL_Npc * npc = luaEngine.createLuaNpc(npcData.id);
    if(!npc)
        return;

    npc->setScenePointer(this);
    npc->setup = curNpcData;
    npc->data  = npcData;
    npc->init();

    npcs.push_back(npc);
}

LVL_Npc *LevelScene::spawnNPC(LevelNPC npcData, NpcSpawnType sp_type, NpcSpawnDirection sp_dir, bool reSpawnable)
{
    if(npcData.id<=0) return NULL;
    if(!ConfigManager::lvl_npc_indexes.contains(npcData.id))
        return NULL;

    obj_npc* curNpcData = &ConfigManager::lvl_npc_indexes[npcData.id];
    LVL_Npc * npc = luaEngine.createLuaNpc(npcData.id);
    if(!npc)
        return NULL;

    npcData.array_id= ++data.npc_array_id;
    npc->setScenePointer(this);
    npc->setup = curNpcData;
    npc->reSpawnable=reSpawnable;
    npc->data = npcData;
    npc->init();
    switch(sp_type)
    {
        case GENERATOR_WARP:
            switch(sp_dir)
            {
                case SPAWN_DOWN:
                    npc->setWarpSpawn(LVL_Npc::WARP_TOP); break;
                case SPAWN_UP:
                    npc->setWarpSpawn(LVL_Npc::WARP_BOTTOM); break;
                case SPAWN_LEFT:
                    npc->setWarpSpawn(LVL_Npc::WARP_RIGHT); break;
                case SPAWN_RIGHT:
                    npc->setWarpSpawn(LVL_Npc::WARP_LEFT); break;
            }
            break;
        case GENERATOR_PROJECTILE:
            switch(sp_dir)
            {
                case SPAWN_DOWN:
                    npc->setSpeedX(0.0f); npc->setSpeedY(ConfigManager::marker_npc.projectile_speed); break;
                case SPAWN_UP:
                    npc->setSpeedX(0.0f); npc->setSpeedY(-ConfigManager::marker_npc.projectile_speed);  break;
                case SPAWN_LEFT:
                    npc->setSpeedX(-ConfigManager::marker_npc.projectile_speed); npc->setSpeedY(0.0f);  break;
                case SPAWN_RIGHT:
                    npc->setSpeedX(ConfigManager::marker_npc.projectile_speed); npc->setSpeedY(0.0f);  break;
            }
            PGE_Audio::playSound(ConfigManager::marker_npc.projectile_sound_id);
            break;
        default:
            break;
    }
    npc->Activate();
    active_npcs.push_back(npc);
    npcs.push_back(npc);
    return npc;
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
                list.removeAt(x); x--; //Remove blocks which is not fits into specific Switch-ID
            }
        }
    }
}



void LevelScene::addPlayer(PlayerPoint playerData, bool byWarp, int warpType, int warpDirect)
{
    LVL_Player * player;
    if(luaEngine.isValid()){
        player = luaEngine.createLuaPlayer();
        if(player == nullptr)
            player = new LVL_Player(this);
    }else{
        player = new LVL_Player(this);
    }

    if(!player) throw("Out of memory [new LVL_Player] addPlayer");
    player->_scene=this;

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

    if(playerData.id==1)
        player1Controller->registerInControl(player);
    else if(playerData.id==2)
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

