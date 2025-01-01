/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2025 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "config_manager.h"
#include "config_manager_private.h"
#include <graphics/gl_renderer.h>

int  ConfigManager::getBlockTexture(unsigned long blockID)
{
    if(!lvl_block_indexes.contains(blockID))
        return -1;

    obj_block *blkSetup = &lvl_block_indexes[blockID];

    if(blkSetup->isInit)
    {
        if(blkSetup->textureArrayId < int32_t(level_textures.size()))
            return blkSetup->textureArrayId;
        else
            return -1;
    }
    else
    {
        std::string imgFile = Dir_Blocks.getCustomFile(blkSetup->setup.image_n);
        std::string maskFile = Dir_Blocks.getCustomFile(blkSetup->setup.mask_n);
        std::string maskFallback = Dir_Blocks.getMaskFallbackFile(blkSetup->setup.image_n);
        int id = static_cast<int>(level_textures.size());
        blkSetup->textureArrayId = id;
        level_textures.emplace_back();
        GlRenderer::loadTextureP(level_textures[id],
                                 imgFile,
                                 maskFile,
                                 maskFallback
                                );
        blkSetup->image = &(level_textures[id]);
        blkSetup->textureID = level_textures[id].texture;
        blkSetup->isInit = true;

        //Also, load and init animator
        if(blkSetup->setup.animated)
        {
            int frameFirst = 0;
            int frameLast  = -1;
            //switch(blkSetup->setup.algorithm)
            //{
            //    case 1: // Invisible block
            //    {
            //        frameFirst = 0;
            //        frameLast = 0;
            //        break;
            //    }
            //    default: //Default block
            //    {
            //        frameFirst = 0;
            //        frameLast = -1;
            //        break;
            //    }
            //}
            //calculate height of frame
            blkSetup->setup.frame_h =
                static_cast<unsigned int>(round(static_cast<double>(level_textures[id].h)
                                                / static_cast<double>(blkSetup->setup.frames)));
            //store animated texture value back
            level_textures[id].frame_h = static_cast<int>(blkSetup->setup.frame_h);
            SimpleAnimator animator(
                true,
                static_cast<int>(blkSetup->setup.frames),
                static_cast<int>(blkSetup->setup.framespeed),
                frameFirst,
                frameLast,
                blkSetup->setup.animation_rev,
                blkSetup->setup.animation_bid
            );

            if(!blkSetup->setup.frame_sequence.empty())
                animator.setFrameSequance(blkSetup->setup.frame_sequence);

            Animator_Blocks.push_back(animator);
            blkSetup->animator_ID = static_cast<int>(Animator_Blocks.size() - 1);
        }

        return id;
    }
}








int  ConfigManager::getBgoTexture(unsigned long bgoID)
{
    if(!lvl_bgo_indexes.contains(bgoID))
        return -1;

    obj_bgo *bgoSetup = &lvl_bgo_indexes[bgoID];

    if(bgoSetup->isInit)
    {
        if(bgoSetup->textureArrayId < int32_t(level_textures.size()))
            return bgoSetup->textureArrayId;
        else
            return -1;
    }
    else
    {
        std::string imgFile = Dir_BGO.getCustomFile(bgoSetup->setup.image_n);
        std::string maskFile = Dir_BGO.getCustomFile(bgoSetup->setup.mask_n);
        std::string maskFallback = Dir_BGO.getMaskFallbackFile(bgoSetup->setup.image_n);
        int id = static_cast<int>(level_textures.size());
        bgoSetup->textureArrayId = id;
        level_textures.emplace_back();
        GlRenderer::loadTextureP(level_textures[id],
                                 imgFile,
                                 maskFile,
                                 maskFallback
                                );
        bgoSetup->image = &(level_textures[id]);
        bgoSetup->textureID = level_textures[id].texture;
        bgoSetup->isInit = true;

        //Also, load and init animator
        if(bgoSetup->setup.animated)
        {
            int frameFirst = 0;
            int frameLast = -1;
            //calculate height of frame
            bgoSetup->setup.frame_h =
                static_cast<unsigned int>(round(static_cast<double>(level_textures[id].h)
                                                / static_cast<double>(bgoSetup->setup.frames)));
            //store animated texture value back
            level_textures[id].frame_h = static_cast<int>(bgoSetup->setup.frame_h);
            SimpleAnimator animator(
                true,
                static_cast<int>(bgoSetup->setup.frames),
                static_cast<int>(bgoSetup->setup.framespeed),
                frameFirst,
                frameLast,
                false,
                false
            );

            if(!bgoSetup->setup.frame_sequence.empty())
                animator.setFrameSequance(bgoSetup->setup.frame_sequence);

            Animator_BGO.push_back(animator);
            bgoSetup->animator_ID = static_cast<int>(Animator_BGO.size() - 1);
        }

        return id;
    }
}


int  ConfigManager::getNpcTexture(unsigned long npcID)
{
    if(!lvl_npc_indexes.contains(npcID))
        return -1;

    obj_npc *npcSetup = &lvl_npc_indexes[npcID];

    if(npcSetup->isInit)
    {
        if(npcSetup->textureArrayId < int32_t(level_textures.size()))
            return npcSetup->textureArrayId;
        else
            return -1;
    }
    else
    {
        std::string imgFile  = Dir_NPC.getCustomFile(npcSetup->setup.image_n);
        std::string maskFile = Dir_NPC.getCustomFile(npcSetup->setup.mask_n);
        std::string maskFallback = Dir_NPC.getMaskFallbackFile(npcSetup->setup.image_n);
        int id = static_cast<int>(level_textures.size());
        npcSetup->textureArrayId = id;
        level_textures.emplace_back();
        GlRenderer::loadTextureP(level_textures[id],
                                 imgFile,
                                 maskFile,
                                 maskFallback
                                );
        npcSetup->image = &(level_textures[id]);
        npcSetup->textureID = level_textures[id].texture;
        npcSetup->isInit = true;

        //Also, load and init animator
        if((npcSetup->setup.shared_ani) && ((npcSetup->setup.frames > 1) || (npcSetup->setup.framestyle > 0)))
        {
            AdvNpcAnimator animator(*(npcSetup->image), *npcSetup);
            Animator_NPC.push_back(animator);
            Animator_NPC.back().start();
            npcSetup->animator_ID = static_cast<int>(Animator_NPC.size() - 1);
        }

        return id;
    }
}




int ConfigManager::getEffectTexture(unsigned long effectID)
{
    if(!lvl_effects_indexes.contains(effectID))
        return -1;

    obj_effect *effSetup = &lvl_effects_indexes[effectID];

    if(effSetup->isInit)
    {
        if(effSetup->textureArrayId < int32_t(level_textures.size()))
            return effSetup->textureArrayId;
        else
            return -1;
    }
    else
    {
        std::string imgFile = Dir_EFFECT.getCustomFile(effSetup->image_n);
        std::string maskFile = Dir_EFFECT.getCustomFile(effSetup->mask_n);
        std::string maskFallback = Dir_EFFECT.getMaskFallbackFile(effSetup->image_n);
        int id = static_cast<int>(level_textures.size());
        effSetup->textureArrayId = id;
        level_textures.emplace_back();
        GlRenderer::loadTextureP(level_textures[id],
                                 imgFile,
                                 maskFile,
                                 maskFallback
                                );
        effSetup->image = &(level_textures[id]);
        effSetup->textureID = level_textures[id].texture;
        effSetup->isInit = true;
        effSetup->frame_h = level_textures[id].h;
        return id;
    }
}


void ConfigManager::resetPlayableTexuresState()
{
    for(unsigned long i = 0; i < playable_characters.size(); i++)
    {
        obj_player &pl = playable_characters[i];

        for(unsigned long j = 0; j < pl.states.size(); j++)
        {
            obj_player_state &st = pl.states[j];
            st.isInit = false;
            st.textureArrayId = 0;
            st.animator_ID = 0;
        }
    }
}

void ConfigManager::resetPlayableTexuresStateWld()
{
    for(unsigned long i = 0; i < playable_characters.size(); i++)
    {
        obj_player &pl = playable_characters[i];
        pl.isInit_wld = false;
        pl.textureArrayId_wld = 0;
        pl.animator_ID_wld = 0;
    }
}

int  ConfigManager::getLvlPlayerTexture(unsigned long playerID, unsigned long stateID)
{
    if(!playable_characters.contains(playerID))
        return -1;

    obj_player &plr = playable_characters[playerID];

    if(!plr.states.contains(stateID))
        return -1;

    obj_player_state &state = plr.states[stateID];

    if(state.isInit)
    {
        if(state.textureArrayId < int32_t(level_textures.size()))
            return state.textureArrayId;
        else
            return -1;
    }
    else
    {
        bool isDefault = false;
        std::string imgFile = Dir_PlayerLvl.getCustomFile(state.image_n, &isDefault);

        if(isDefault || imgFile.empty())
            imgFile = playerLvlPath + plr.sprite_folder + "/" + state.image_n;

        std::string maskFile = Dir_PlayerLvl.getCustomFile(state.mask_n, &isDefault);
        std::string maskFallback = Dir_PlayerLvl.getMaskFallbackFile(state.image_n);

        if(isDefault || maskFile.empty())
            maskFile  = playerLvlPath + plr.sprite_folder + "/" + state.mask_n;

        int id = static_cast<int>(level_textures.size());
        state.textureArrayId = id;
        level_textures.emplace_back();
        GlRenderer::loadTextureP(level_textures[id],
                                 imgFile,
                                 maskFile,
                                 maskFallback
                                );
        state.image = &(level_textures[id]);
        state.textureID = level_textures[id].texture;
        state.isInit = true;
        //Store size of one frame
        plr.frame_width = level_textures[id].w / plr.matrix_width;
        plr.frame_height = level_textures[id].h / plr.matrix_height;
        return id;
    }
}


int  ConfigManager::getWldPlayerTexture(unsigned long playerID, unsigned long stateID)
{
    if(!playable_characters.contains(playerID))
        return -1;

    obj_player &plr = playable_characters[playerID];

    if(!plr.states.contains(stateID))
        return -1;

    if(plr.isInit_wld)
    {
        if(plr.textureArrayId_wld < int32_t(world_textures.size()))
            return plr.textureArrayId_wld;
        else
            return -1;
    }
    else
    {
        bool isDefault = false;
        std::string imgFile = Dir_PlayerWld.getCustomFile(plr.image_wld_n, &isDefault);

        if(isDefault || imgFile.empty())
            imgFile = playerWldPath + plr.image_wld_n;

        std::string maskFile = Dir_PlayerWld.getCustomFile(plr.mask_wld_n, &isDefault);
        std::string maskFallback = Dir_NPC.getMaskFallbackFile(plr.image_wld_n);

        if(isDefault || maskFile.empty())
            maskFile  = playerWldPath + plr.mask_wld_n;

        int id = static_cast<int>(world_textures.size());
        plr.textureArrayId_wld = id;
        world_textures.emplace_back();
        GlRenderer::loadTextureP(world_textures[id],
                                 imgFile,
                                 maskFile,
                                 maskFallback
                                );
        plr.image_wld = &(world_textures[id]);
        plr.textureID_wld = world_textures[id].texture;
        plr.isInit_wld = true;
        return id;
    }
}




int  ConfigManager::getBGTexture(unsigned long bgID, bool isSecond)
{
    if(!lvl_bg_indexes.contains(bgID))
        return -1;

    obj_BG *bgSetup = &lvl_bg_indexes[bgID];

    if((bgSetup->isInit && !isSecond) || (bgSetup->second_isInit && isSecond))
    {
        if(isSecond)
        {
            if(bgSetup->second_textureArrayId < int32_t(level_textures.size()))
                return bgSetup->second_textureArrayId;
            else
                return -1;
        }
        else
        {
            if(bgSetup->textureArrayId < int32_t(level_textures.size()))
                return bgSetup->textureArrayId;
            else
                return -1;
        }
    }
    else
    {
        std::string imgFile;

        if(isSecond)
            imgFile = Dir_BG.getCustomFile(bgSetup->setup.second_image_n);
        else
            imgFile = Dir_BG.getCustomFile(bgSetup->setup.image_n);

        int id = static_cast<int>(level_textures.size());

        if(isSecond)
            bgSetup->second_textureArrayId = id;
        else
            bgSetup->textureArrayId = id;

        level_textures.emplace_back();
        GlRenderer::loadTextureP(level_textures[id], imgFile);

        if(isSecond)
        {
            bgSetup->second_image = &(level_textures[id]);
            bgSetup->second_textureID = level_textures[id].texture;
            bgSetup->second_isInit = true;
            bgSetup->second_Color_upper = level_textures[id].ColorUpper;
            bgSetup->second_Color_lower = level_textures[id].ColorLower;
        }
        else
        {
            bgSetup->image = &(level_textures[id]);
            bgSetup->textureID = level_textures[id].texture;
            bgSetup->Color_upper = level_textures[id].ColorUpper;
            bgSetup->Color_lower = level_textures[id].ColorLower;
            bgSetup->isInit = true;
        }

        //Also, load and init animator
        if(bgSetup->setup.animated && !isSecond)
        {
            int frameFirst = 0;
            int frameLast = -1;
            //calculate height of frame
            bgSetup->setup.frame_h =
                static_cast<unsigned int>(round(static_cast<double>(level_textures[id].h)
                                                / static_cast<double>(bgSetup->setup.frames)));
            //store animated texture value back
            level_textures[id].frame_h = static_cast<int>(bgSetup->setup.frame_h);
            SimpleAnimator animator(
                true,
                static_cast<int>(bgSetup->setup.frames),
                static_cast<int>(bgSetup->setup.framespeed), //bgSetup->framespeed, //Ouch, forgot made framespeed value for background :P Will add later...
                //3 june 2015. I Finally implemented this sweet variable :D
                frameFirst,
                frameLast,
                false,
                false
            );
            Animator_BG.push_back(animator);
            bgSetup->animator_ID = static_cast<int>(Animator_BG.size() - 1);
        }

        return id;
    }
}


int ConfigManager::getBGLayerTexture(uint64_t bgID, size_t layerId)
{
    if(!lvl_bg_indexes.contains(bgID))
        return -1;
    obj_BG *bgSetup = &lvl_bg_indexes[bgID];

    if(bgSetup->texturePerLayer.size() <= layerId)
        return -1;

    obj_BG::TextureId &tID = bgSetup->texturePerLayer[layerId];
    if(tID.textureArrayId >= 0)
    {
        if(tID.textureArrayId < int64_t(level_textures.size()))
            return (int)tID.textureArrayId;
        else
            return -1;
    }
    else
    {
        std::string imgFile;
        BgSetup::BgLayer &layer = bgSetup->setup.layers[layerId];

        imgFile = Dir_BG.getCustomFile(layer.image);

        size_t id = level_textures.size();

        tID.textureArrayId = static_cast<int64_t>(id);

        level_textures.emplace_back();
        GlRenderer::loadTextureP(level_textures[id], imgFile);

        tID.image          = &(level_textures[id]);
        tID.textureId      = level_textures[id].texture;

        //Also, load and init animator
        if(layer.animated)
        {
            int frameFirst  = 0;
            int frameLast   = -1;
            //calculate height of frame
            layer.frame_h =
                static_cast<unsigned int>(round(static_cast<double>(level_textures[id].h)
                                                / static_cast<double>(layer.frames)));
            //store animated texture value back
            level_textures[id].frame_h = static_cast<int>(layer.frame_h);
            SimpleAnimator animator(
                true,
                static_cast<int>(layer.frames),
                static_cast<int>(layer.framespeed), //bgSetup->framespeed, //Ouch, forgot made framespeed value for background :P Will add later...
                //3 june 2015. I Finally implemented this sweet variable :D
                frameFirst,
                frameLast,
                false,
                false
            );

            if(!layer.frame_sequence.empty())
                animator.setFrameSequance(layer.frame_sequence);

            Animator_BG.push_back(animator);
            tID.animatorId = int64_t(Animator_BG.size() - 1);
        }

        return (int)id;
    }
}






/****************************World map items*****************************/

int  ConfigManager::getTileTexture(unsigned long tileID)
{
    if(!wld_tiles.contains(tileID))
        return -1;

    obj_w_tile *tileSetup = &wld_tiles[tileID];

    if(tileSetup->isInit)
    {
        if(tileSetup->textureArrayId < int32_t(world_textures.size()))
            return tileSetup->textureArrayId;
        else
            return -1;
    }
    else
    {
        std::string imgFile = Dir_Tiles.getCustomFile(tileSetup->setup.image_n);
        std::string maskFile = Dir_Tiles.getCustomFile(tileSetup->setup.mask_n);
        std::string maskFallback = Dir_Tiles.getMaskFallbackFile(tileSetup->setup.image_n);
        int id = static_cast<int>(world_textures.size());
        tileSetup->textureArrayId = id;
        world_textures.emplace_back();
        GlRenderer::loadTextureP(world_textures[id],
                                 imgFile,
                                 maskFile,
                                 maskFallback
                                );
        tileSetup->image        = &(world_textures[id]);
        tileSetup->textureID    = world_textures[id].texture;
        tileSetup->isInit       = true;

        //Also, load and init animator
        if(tileSetup->setup.animated)
        {
            int frameFirst = 0;
            int frameLast = -1;
            //calculate height of frame
            tileSetup->setup.frame_h =
                static_cast<unsigned int>(round(static_cast<double>(world_textures[id].h)
                                                / static_cast<double>(tileSetup->setup.frames)));
            //store animated texture value back
            world_textures[id].frame_h = static_cast<int>(tileSetup->setup.frame_h);
            SimpleAnimator animator(
                true,
                static_cast<int>(tileSetup->setup.frames),
                static_cast<int>(tileSetup->setup.framespeed),
                frameFirst,
                frameLast,
                false,
                false
            );

            if(!tileSetup->setup.frame_sequence.empty())
                animator.setFrameSequance(tileSetup->setup.frame_sequence);

            Animator_Tiles.push_back(animator);
            tileSetup->animator_ID = static_cast<int>(Animator_Tiles.size() - 1);
        }

        return id;
    }
}


int  ConfigManager::getSceneryTexture(unsigned long sceneryID)
{
    if(!wld_scenery.contains(sceneryID))
        return -1;

    obj_w_scenery *scenerySetup = &wld_scenery[sceneryID];

    if(scenerySetup->isInit)
    {
        if(scenerySetup->textureArrayId < int32_t(world_textures.size()))
            return scenerySetup->textureArrayId;
        else
            return -1;
    }
    else
    {
        std::string imgFile = Dir_Scenery.getCustomFile(scenerySetup->setup.image_n);
        std::string maskFile = Dir_Scenery.getCustomFile(scenerySetup->setup.mask_n);
        std::string maskFallback = Dir_Scenery.getMaskFallbackFile(scenerySetup->setup.image_n);
        int id = static_cast<int>(world_textures.size());
        scenerySetup->textureArrayId = id;
        world_textures.emplace_back();
        GlRenderer::loadTextureP(world_textures[id],
                                 imgFile,
                                 maskFile,
                                 maskFallback
                                );
        scenerySetup->image = &(world_textures[id]);
        scenerySetup->textureID = world_textures[id].texture;
        scenerySetup->isInit = true;

        //Also, load and init animator
        if(scenerySetup->setup.animated)
        {
            int frameFirst = 0;
            int frameLast = -1;
            //calculate height of frame
            scenerySetup->setup.frame_h =
                static_cast<unsigned int>(round(static_cast<double>(world_textures[id].h)
                                                / static_cast<double>(scenerySetup->setup.frames)));
            //store animated texture value back
            world_textures[id].frame_h = static_cast<int>(scenerySetup->setup.frame_h);
            SimpleAnimator animator(
                true,
                static_cast<int>(scenerySetup->setup.frames),
                static_cast<int>(scenerySetup->setup.framespeed),
                frameFirst,
                frameLast,
                false,
                false
            );

            if(!scenerySetup->setup.frame_sequence.empty())
                animator.setFrameSequance(scenerySetup->setup.frame_sequence);

            Animator_Scenery.push_back(animator);
            scenerySetup->animator_ID = static_cast<int>(Animator_Scenery.size() - 1);
        }

        return id;
    }
}


int  ConfigManager::getWldPathTexture(unsigned long pathID)
{
    if(!wld_paths.contains(pathID))
        return -1;

    obj_w_path *pathSetup = &wld_paths[pathID];

    if(pathSetup->isInit)
    {
        if(pathSetup->textureArrayId < int32_t(world_textures.size()))
            return pathSetup->textureArrayId;
        else
            return -1;
    }
    else
    {
        std::string imgFile     = Dir_WldPaths.getCustomFile(pathSetup->setup.image_n);
        std::string maskFile    = Dir_WldPaths.getCustomFile(pathSetup->setup.mask_n);
        std::string maskFallback = Dir_WldPaths.getMaskFallbackFile(pathSetup->setup.image_n);
        int id = static_cast<int>(world_textures.size());
        pathSetup->textureArrayId = id;
        world_textures.emplace_back();
        GlRenderer::loadTextureP(world_textures[id],
                                 imgFile,
                                 maskFile,
                                 maskFallback
                                );
        pathSetup->image = &(world_textures[id]);
        pathSetup->textureID = world_textures[id].texture;
        pathSetup->isInit = true;

        //Also, load and init animator
        if(pathSetup->setup.animated)
        {
            int frameFirst = 0;
            int frameLast = -1;
            //calculate height of frame
            pathSetup->setup.frame_h =
                static_cast<unsigned int>(round(static_cast<double>(world_textures[id].h)
                                                / static_cast<double>(pathSetup->setup.frames)));
            //store animated texture value back
            world_textures[id].frame_h = static_cast<int>(pathSetup->setup.frame_h);
            SimpleAnimator animator(
                true,
                static_cast<int>(pathSetup->setup.frames),
                static_cast<int>(pathSetup->setup.framespeed),
                frameFirst,
                frameLast,
                false,
                false
            );

            if(!pathSetup->setup.frame_sequence.empty())
                animator.setFrameSequance(pathSetup->setup.frame_sequence);

            Animator_WldPaths.push_back(animator);
            pathSetup->animator_ID = static_cast<int>(Animator_WldPaths.size() - 1);
        }

        return id;
    }
}



int  ConfigManager::getWldLevelTexture(unsigned long levelID)
{
    if(!wld_levels.contains(levelID))
        return -1;

    obj_w_level *lvlSetup = &wld_levels[levelID];

    if(lvlSetup->isInit)
    {
        if(lvlSetup->textureArrayId < int32_t(world_textures.size()))
            return lvlSetup->textureArrayId;
        else
            return -1;
    }
    else
    {
        std::string imgFile = Dir_WldLevel.getCustomFile(lvlSetup->setup.image_n);
        std::string maskFile = Dir_WldLevel.getCustomFile(lvlSetup->setup.mask_n);
        std::string maskFallback = Dir_WldLevel.getMaskFallbackFile(lvlSetup->setup.image_n);
        int id = static_cast<int>(world_textures.size());
        lvlSetup->textureArrayId = id;
        world_textures.emplace_back();
        GlRenderer::loadTextureP(world_textures[id],
                                 imgFile,
                                 maskFile,
                                 maskFallback
                                );
        lvlSetup->image = &(world_textures[id]);
        lvlSetup->textureID = world_textures[id].texture;
        lvlSetup->isInit = true;

        //Also, load and init animator
        if(lvlSetup->setup.animated)
        {
            int frameFirst = 0;
            int frameLast = -1;
            //calculate height of frame
            lvlSetup->setup.frame_h =
                static_cast<unsigned int>(round(static_cast<double>(world_textures[id].h)
                                                / static_cast<double>(lvlSetup->setup.frames)));
            //store animated texture value back
            world_textures[id].frame_h = static_cast<int>(lvlSetup->setup.frame_h);
            SimpleAnimator animator(
                true,
                static_cast<int>(lvlSetup->setup.frames),
                static_cast<int>(lvlSetup->setup.framespeed),
                frameFirst,
                frameLast,
                false,
                false
            );

            if(!lvlSetup->setup.frame_sequence.empty())
                animator.setFrameSequance(lvlSetup->setup.frame_sequence);

            Animator_WldLevel.push_back(animator);
            lvlSetup->animator_ID = static_cast<int>(Animator_WldLevel.size() - 1);
        }

        return id;
    }
}
