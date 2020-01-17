/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#include "../scene_level.h"
#include "../../data_configs/config_manager.h"

#include <audio/pge_audio.h>

//static double zCounter = 0;
#include <PGE_File_Formats/file_formats.h>

void LevelScene::restoreDestroyedBlocks(bool smoke)
{
    //Restore all destroyed and modified blocks into their initial states
    for(LVL_BlocksSet::iterator it = m_blocksDestroyed.begin();
        it != m_blocksDestroyed.end();
        it++)
    {
        LVL_Block *blk = *it;
        SDL_assert(blk);
        if(!blk)
            continue;
        bool wasInvisible = !blk->isVisible() || blk->m_destroyed;
        blk->setVisible(true);
        blk->init(true);//Force re-initialize block from initial data
        if(smoke && wasInvisible)
        {
            PGE_Phys_Object::Momentum momentum = blk->m_momentum_relative;
            LVL_LayerEngine::Layer &lyrn = m_layers.getLayer(blk->data.layer);//Get block's original layer
            momentum.x -= lyrn.m_subtree.m_offsetX;
            momentum.y -= lyrn.m_subtree.m_offsetY;
            m_layers.spawnSmokeAt(momentum.centerX(), momentum.centerY());
        }
    }
    m_blocksDestroyed.clear();
}

void LevelScene::placeBlock(LevelBlock &blockData)
{
    if(blockData.id <= 0)
        return;

    if(!ConfigManager::lvl_block_indexes.contains(blockData.id))
        return;

    LVL_Block *block;
    block = new LVL_Block(this);
    SDL_assert_release(block);
    if(!block)
    {
        pLogFatal("Out of memory [new LVL_Block place]");
        abort();
    }

    block->data = blockData;
    block->init();
    m_itemsBlocks.insert(block);
}

LVL_Block *LevelScene::spawnBlock(const LevelBlock &blockData)
{
    if(blockData.id <= 0)
        return NULL;

    if(!ConfigManager::lvl_block_indexes.contains(blockData.id))
        return nullptr;

    LVL_Block *block;
    block = new LVL_Block(this);
    SDL_assert_release(block);
    if(!block)
    {
        pLogFatal("Out of memory [new LVL_Block spawn]");
        abort();
    }

    block->data = blockData;
    block->data.meta.array_id = ++m_data.blocks_array_id;
    block->init();
    m_itemsBlocks.insert(block);
    return block;
}

void LevelScene::placeBGO(LevelBGO& bgoData)
{
    if(bgoData.id <= 0) return;

    if(!ConfigManager::lvl_bgo_indexes.contains(bgoData.id))
        return;

    LVL_Bgo *bgo;
    bgo = new LVL_Bgo(this);
    SDL_assert_release(bgo);
    if(!bgo)
    {
        pLogFatal("Out of memory [new LVL_Bgo place]");
        abort();
    }

    bgo->data = bgoData;
    bgo->init();
    m_itemsBgo.insert(bgo);
}

LVL_Bgo *LevelScene::spawnBGO(const LevelBGO &bgoData)
{
    if(bgoData.id <= 0) return NULL;

    if(!ConfigManager::lvl_bgo_indexes.contains(bgoData.id))
        return NULL;

    LVL_Bgo *bgo;
    bgo = new LVL_Bgo(this);
    SDL_assert_release(bgo);
    if(!bgo)
    {
        pLogFatal("Out of memory [new LVL_Bgo] spawn");
        abort();
    }

    bgo->data = bgoData;
    bgo->data.meta.array_id = ++m_data.blocks_array_id;
    bgo->init();
    m_itemsBgo.insert(bgo);
    return bgo;
}

void LevelScene::placeNPC(LevelNPC& npcData)
{
    if(npcData.id <= 0)
        return;

    if(!ConfigManager::lvl_npc_indexes.contains(npcData.id))
        return;

    obj_npc *curNpcData = &ConfigManager::lvl_npc_indexes[npcData.id];
    LVL_Npc *npc = m_luaEngine.createLuaNpc(npcData.id);
    SDL_assert_release(npc);
    if(!npc)
        return;
    npc->setScenePointer(this);
    npc->setup = curNpcData;
    npc->data  = npcData;
    npc->init();
    m_itemsNpc.insert(npc);
}

LVL_Npc *LevelScene::spawnNPC(const LevelNPC &npcData,
                              NpcSpawnType sp_type,
                              NpcSpawnDirection sp_dir,
                              bool reSpawnable)
{
    if(npcData.id <= 0)
        return nullptr;

    if(!ConfigManager::lvl_npc_indexes.contains(npcData.id))
        return nullptr;

    obj_npc *curNpcData = &ConfigManager::lvl_npc_indexes[npcData.id];
    LVL_Npc *npc = m_luaEngine.createLuaNpc(npcData.id);
    SDL_assert_release(npc);
    if(!npc)
        return nullptr;
    npc->setScenePointer(this);
    npc->setup = curNpcData;
    npc->reSpawnable = reSpawnable;
    npc->data = npcData;
    npc->data.meta.array_id = ++m_data.npc_array_id;
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
    m_npcActive.insert(npc);
    m_itemsNpc.insert(npc);
    return npc;
}

void LevelScene::toggleSwitch(unsigned int switch_id)
{
    PGE_Audio::playSoundByRole(obj_sound_role::BlockSwitch);

    SwitchBlocksMap::iterator i = m_switchBlocks.find(switch_id);
    if(i != m_switchBlocks.end())
    {
        LVL_BlocksVector &list = i->second;
        for(size_t x = 0; x < list.size(); x++)
        {
            if((list[x]->setup->setup.switch_Block) && (list[x]->setup->setup.switch_ID == switch_id))
                list[x]->transformTo(list[x]->setup->setup.switch_transform, 2);
            else
            {
                list.erase(list.begin() + LVL_BlocksArray::difference_type(x));
                x--; //Remove blocks which are not fits into specific Switch-ID
            }
        }
        //Change state of the switch to allow lua script see this
        m_switchStates[static_cast<size_t>(switch_id)] = !m_switchStates[static_cast<size_t>(switch_id)];
    }
}

bool LevelScene::lua_switchState(uint32_t switch_id)
{
    if(switch_id >= m_switchStates.size())
        return false;
    else
        return m_switchStates[switch_id];
}


bool LevelScene::addPlayer(PlayerPoint playerData, bool byWarp, int warpType, int warpDirect, bool cannon, double cannon_speed)
{
    LVL_Player *player = nullptr;

    if(m_luaEngine.isValid())
    {
        player = m_luaEngine.createLuaPlayer();

        if(player == nullptr)
            player = new LVL_Player(this);
    }
    else
        player = new LVL_Player(this);

    if(!player)
    {
        pLogFatal("Out of memory [new LVL_Player] addPlayer");
        abort();
    }

    player->m_scene = this;

    if(m_itemsPlayers.size() == 0)
        player->camera = &m_cameras.front();
    else if(m_itemsPlayers.size() == 1)
        player->camera = &m_cameras.back();

    int sID = findNearestSection(playerData.x, playerData.y);
    LVL_Section *sct = getSection(sID);

    if(!sct)
    {
        m_errorMsg = "Can't initialize playable character: Section is not found!";
        delete player;
        return false;
    }

    player->setParentSection(sct);
    player->z_index = zOrder.player;
    player->m_global_state = ((static_cast<unsigned>(m_playerStates.size()) > (playerData.id - 1)) ?
                            &m_playerStates[static_cast<int>(playerData.id - 1)] : nullptr);
    player->setPlayerPointInfo(playerData);
    if(!player->init())
    {
        if(!player->m_isLuaPlayer)
            delete player;//Don't delete lua player
        m_errorMsg = "Failed to initialize playable character!\nSee log file for more information!";
        m_fader.setFade(10, 1.0, 1.0);
        setExiting(0, LvlExit::EXIT_Error);
        return false;
    }
    m_itemsPlayers.push_back(player);

    if(playerData.id == 1)
        m_player1Controller->registerInControl(player);
    else if(playerData.id == 2)
        m_player2Controller->registerInControl(player);

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

    return true;
}
