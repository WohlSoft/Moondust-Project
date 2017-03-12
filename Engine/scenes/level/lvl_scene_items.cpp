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
#include "../../data_configs/config_manager.h"

#include <audio/pge_audio.h>

//static double zCounter = 0;
#include <PGE_File_Formats/file_formats.h>

void LevelScene::placeBlock(LevelBlock &blockData)
{
    if(blockData.id <= 0)
        return;

    if(!ConfigManager::lvl_block_indexes.contains(blockData.id))
        return;

    LVL_Block *block;
    block = new LVL_Block(this);

    if(!block) throw("Out of memory [new LVL_Block place]");

    block->data = blockData;
    block->init();
    blocks.push_back(block);
}

LVL_Block *LevelScene::spawnBlock(LevelBlock blockData)
{
    if(blockData.id <= 0) return NULL;

    if(!ConfigManager::lvl_block_indexes.contains(blockData.id))
        return nullptr;

    LVL_Block *block;
    blockData.meta.array_id = ++data.blocks_array_id;
    block = new LVL_Block(this);

    if(!block) throw("Out of memory [new LVL_Block spawn]");

    block->data = blockData;
    block->init();
    blocks.push_back(block);
    return block;
}

void LevelScene::destroyBlock(LVL_Block *&_block)
{
    std::remove(blocks.begin(), blocks.end(), _block);
    delete _block;
    _block = nullptr;
}

void LevelScene::placeBGO(LevelBGO& bgoData)
{
    if(bgoData.id <= 0) return;

    if(!ConfigManager::lvl_bgo_indexes.contains(bgoData.id))
        return;

    LVL_Bgo *bgo;
    bgo = new LVL_Bgo(this);

    if(!bgo) throw("Out of memory [new LVL_Bgo place]");

    bgo->data = bgoData;
    bgo->init();
    bgos.push_back(bgo);
}

LVL_Bgo *LevelScene::spawnBGO(LevelBGO bgoData)
{
    if(bgoData.id <= 0) return NULL;

    if(!ConfigManager::lvl_bgo_indexes.contains(bgoData.id))
        return NULL;

    bgoData.meta.array_id = ++data.blocks_array_id;
    LVL_Bgo *bgo;
    bgo = new LVL_Bgo(this);

    if(!bgo) throw("Out of memory [new LVL_Bgo] spawn");

    bgo->data = bgoData;
    bgo->init();
    bgos.push_back(bgo);
    return bgo;
}

void LevelScene::placeNPC(LevelNPC& npcData)
{
    if(npcData.id <= 0)
        return;

    if(!ConfigManager::lvl_npc_indexes.contains(npcData.id))
        return;

    obj_npc *curNpcData = &ConfigManager::lvl_npc_indexes[npcData.id];
    LVL_Npc *npc = luaEngine.createLuaNpc(npcData.id);

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
    if(npcData.id <= 0) return NULL;

    if(!ConfigManager::lvl_npc_indexes.contains(npcData.id))
        return NULL;

    obj_npc *curNpcData = &ConfigManager::lvl_npc_indexes[npcData.id];
    LVL_Npc *npc = luaEngine.createLuaNpc(npcData.id);

    if(!npc)
        return NULL;

    npcData.meta.array_id = ++data.npc_array_id;
    npc->setScenePointer(this);
    npc->setup = curNpcData;
    npc->reSpawnable = reSpawnable;
    npc->data = npcData;
    npc->init();
    npc->m_spawnedGeneratorType = sp_type;
    npc->m_spawnedGeneratorDirection = sp_dir;

    switch(sp_type)
    {
    case GENERATOR_WARP:
        switch(sp_dir)
        {
        case SPAWN_DOWN:
            npc->setWarpSpawn(LVL_Npc::WARP_TOP);
            break;

        case SPAWN_UP:
            npc->setWarpSpawn(LVL_Npc::WARP_BOTTOM);
            break;

        case SPAWN_LEFT:
            npc->setWarpSpawn(LVL_Npc::WARP_RIGHT);
            break;

        case SPAWN_RIGHT:
            npc->setWarpSpawn(LVL_Npc::WARP_LEFT);
            break;
        }

        break;

    case GENERATOR_PROJECTILE:
        switch(sp_dir)
        {
        case SPAWN_DOWN:
            npc->setSpeedX(0.0);
            npc->setSpeedY(ConfigManager::g_setup_npc.projectile_speed);
            break;

        case SPAWN_UP:
            npc->setSpeedX(0.0);
            npc->setSpeedY(-ConfigManager::g_setup_npc.projectile_speed);
            break;

        case SPAWN_LEFT:
            npc->setSpeedX(-ConfigManager::g_setup_npc.projectile_speed);
            npc->setSpeedY(0.0);
            break;

        case SPAWN_RIGHT:
            npc->setSpeedX(ConfigManager::g_setup_npc.projectile_speed);
            npc->setSpeedY(0.0);
            break;
        }

        PGE_Audio::playSound(ConfigManager::g_setup_npc.projectile_sound_id);
        break;

    default:
        break;
    }

    npc->Activate();
    active_npcs.push_back(npc);
    npcs.push_back(npc);
    return npc;
}

void LevelScene::toggleSwitch(unsigned int switch_id)
{
    PGE_Audio::playSoundByRole(obj_sound_role::BlockSwitch);

    SwitchBlocksMap::iterator i = switch_blocks.find(switch_id);
    if(i != switch_blocks.end())
    {
        std::vector<LVL_Block *> &list = i->second;

        for(size_t x = 0; x < list.size(); x++)
        {
            if((list[x]->setup->setup.switch_Block) && (list[x]->setup->setup.switch_ID == switch_id))
                list[x]->transformTo(list[x]->setup->setup.switch_transform, 2);
            else
            {
                list.erase(list.begin() + x);
                x--; //Remove blocks which are not fits into specific Switch-ID
            }
        }

        //Change state of the switch to allow lua script see this
        switch_states[static_cast<size_t>(switch_id)] = !switch_states[static_cast<size_t>(switch_id)];
    }
}

bool LevelScene::lua_switchState(uint32_t switch_id)
{
    if(switch_id >= switch_states.size())
        return false;
    else
        return switch_states[switch_id];
}



void LevelScene::addPlayer(PlayerPoint playerData, bool byWarp, int warpType, int warpDirect, bool cannon, double cannon_speed)
{
    LVL_Player *player = nullptr;

    if(luaEngine.isValid())
    {
        player = luaEngine.createLuaPlayer();

        if(player == nullptr)
            player = new LVL_Player(this);
    }
    else
        player = new LVL_Player(this);

    if(!player)
        throw(std::runtime_error("Out of memory [new LVL_Player] addPlayer"));

    player->m_scene = this;

    if(players.size() == 0)
        player->camera = &cameras.front();
    else if(players.size() == 1)
        player->camera = &cameras.back();

    int sID = findNearestSection(playerData.x, playerData.y);
    LVL_Section *sct = getSection(sID);

    if(!sct)
    {
        delete player;
        return;
    }

    player->setParentSection(sct);
    player->z_index = zOrder.player;
    player->global_state = ((static_cast<unsigned>(player_states.size()) > (playerData.id - 1)) ?
                            &player_states[static_cast<int>(playerData.id - 1)] : nullptr);
    player->setPlayerPointInfo(playerData);
    if(!player->init())
    {
        delete player;
        errorMsg = "Failed to initialize playable character!\nSee log file for more information!";
        m_fader.setFade(10, 1.0, 1.0);
        setExiting(0, LvlExit::EXIT_Error);
        return;
    }
    players.push_back(player);

    if(playerData.id == 1)
        player1Controller->registerInControl(player);
    else if(playerData.id == 2)
        player2Controller->registerInControl(player);

    if(byWarp)
    {
        player->setPaused(true);
        player->WarpTo(playerData.x, playerData.y, warpType, warpDirect, cannon, cannon_speed);

        if(warpType == 2)
            PGE_Audio::playSoundByRole(obj_sound_role::WarpDoor);
        else if(warpType == 0)
            PGE_Audio::playSoundByRole(obj_sound_role::PlayerMagic);
    }
    else
        player->camera->changeSection(sct, true);
}
