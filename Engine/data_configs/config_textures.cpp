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

#include "config_manager.h"
#include "../common_features/graphics_funcs.h"

long  ConfigManager::getBlockTexture(long blockID)
{
    if(!lvl_block_indexes.contains(blockID))
    {
        return -1;
    }
    obj_block *blkSetup = &lvl_block_indexes[blockID];

    if(blkSetup->isInit)
    {

        if(blkSetup->textureArrayId < level_textures.size())
            return blkSetup->textureArrayId;
        else
            return -1;
    }
    else
    {
        QString imgFile = Dir_Blocks.getCustomFile(blkSetup->image_n);
        QString maskFile = Dir_Blocks.getCustomFile(blkSetup->mask_n);

        PGE_Texture texture;
        texture.w = 0;
        texture.h = 0;
        texture.texture = 0;
        texture.texture_layout = NULL;
        texture.format = 0;
        texture.nOfColors = 0;

        long id = level_textures.size();

        blkSetup->textureArrayId = id;

        level_textures.push_back(texture);

        GraphicsHelps::loadTexture( level_textures[id],
             imgFile,
             maskFile
             );

        blkSetup->image = &(level_textures[id]);
        blkSetup->textureID = level_textures[id].texture;
        blkSetup->isInit = true;

        //Also, load and init animator
        if(blkSetup->animated)
        {
            int frameFirst;
            int frameLast;

            switch(blkSetup->algorithm)
            {
                case 1: // Invisible block
                {
                    frameFirst = 0;
                    frameLast = 0;
                    break;
                }
                case 3: //Player's character block
                {
                    frameFirst = 0;
                    frameLast = 1;
                    break;
                }
                case 4: //Player's character switch
                {
                    frameFirst = 0;
                    frameLast = 3;
                    break;
                }
                default: //Default block
                {
                    frameFirst = 0;
                    frameLast = -1;
                    break;
                }
            }


            SimpleAnimator animator(
                            true,
                            blkSetup->frames,
                            blkSetup->framespeed,
                            frameFirst,
                            frameLast,
                            blkSetup->animation_rev,
                            blkSetup->animation_bid
                        );

            Animator_Blocks.push_back(animator);
            blkSetup->animator_ID = Animator_Blocks.size()-1;
        }
        return id;
    }
}








long  ConfigManager::getBgoTexture(long bgoID)
{
    if(!lvl_bgo_indexes.contains(bgoID))
    {
        return -1;
    }

    obj_bgo* bgoSetup=&lvl_bgo_indexes[bgoID];

    if(bgoSetup->isInit)
    {

        if(bgoSetup->textureArrayId < level_textures.size())
            return bgoSetup->textureArrayId;
        else
            return -1;
    }
    else
    {
        QString imgFile = Dir_BGO.getCustomFile(bgoSetup->image_n);
        QString maskFile = Dir_BGO.getCustomFile(bgoSetup->mask_n);

        PGE_Texture texture;
        texture.w = 0;
        texture.h = 0;
        texture.texture = 0;
        texture.texture_layout = NULL;
        texture.format = 0;
        texture.nOfColors = 0;

        long id = level_textures.size();

        bgoSetup->textureArrayId = id;

        level_textures.push_back(texture);

        GraphicsHelps::loadTexture( level_textures[id],
             imgFile,
             maskFile
             );

        bgoSetup->image = &(level_textures[id]);
        bgoSetup->textureID = level_textures[id].texture;
        bgoSetup->isInit = true;

        //Also, load and init animator
        if(bgoSetup->animated)
        {
            int frameFirst = 0;
            int frameLast = -1;

            //calculate height of frame
            bgoSetup->frame_h =
                    (int)round(double(level_textures[id].h)
                               /double(bgoSetup->frames));

            //store animated texture value back
            level_textures[id].h = bgoSetup->frame_h;

            SimpleAnimator animator(
                            true,
                            bgoSetup->frames,
                            bgoSetup->framespeed,
                            frameFirst,
                            frameLast,
                            false,
                            false
                        );

            Animator_BGO.push_back(animator);
            bgoSetup->animator_ID = Animator_BGO.size()-1;

        }

        return id;
    }
}



long ConfigManager::getEffectTexture(long effectID)
{
    if(!lvl_effects_indexes.contains(effectID))
    {
        return -1;
    }

    obj_effect*effSetup=&lvl_effects_indexes[effectID];

    if(effSetup->isInit)
    {

        if(effSetup->textureArrayId < level_textures.size())
            return effSetup->textureArrayId;
        else
            return -1;
    }
    else
    {
        QString imgFile = Dir_EFFECT.getCustomFile(effSetup->image_n);
        QString maskFile = Dir_EFFECT.getCustomFile(effSetup->mask_n);

        PGE_Texture texture;
        texture.w = 0;
        texture.h = 0;
        texture.texture = 0;
        texture.texture_layout = NULL;
        texture.format = 0;
        texture.nOfColors = 0;

        long id = level_textures.size();

        effSetup->textureArrayId = id;

        level_textures.push_back(texture);

        GraphicsHelps::loadTexture( level_textures[id],
             imgFile,
             maskFile
             );

        effSetup->image = &(level_textures[id]);
        effSetup->textureID = level_textures[id].texture;
        effSetup->isInit = true;

        //Also, load and init animator
        if(effSetup->animated)
        {
            int frameFirst = 0;
            int frameLast = -1;

            //calculate height of frame
            effSetup->frame_h =
                    (int)round(double(level_textures[id].h)
                               /double(effSetup->frames));

            //store animated texture value back
            level_textures[id].h = effSetup->frame_h;

            SimpleAnimator animator(
                            true,
                            effSetup->frames,
                            effSetup->framespeed,
                            frameFirst,
                            frameLast,
                            false,
                            false
                        );

            Animator_EFFECT.push_back(animator);
            effSetup->animator_ID = Animator_EFFECT.size()-1;
        }

        return id;
    }
}


void ConfigManager::resetPlayableTexuresState()
{
    for(QHash<int, obj_player >::iterator x=playable_characters.begin() ; x!=playable_characters.end() ; x++ )
    {
        for(QHash<int, obj_player_state >::iterator y=(*x).states.begin() ; y!=(*x).states.end() ; y++ )
        {
            (*y).isInit=false;
            (*y).textureArrayId=0;
            (*y).animator_ID=0;
        }
    }
}

void ConfigManager::resetPlayableTexuresStateWld()
{
    for(QHash<int, obj_player >::iterator x=playable_characters.begin() ; x!=playable_characters.end() ; x++ )
    {
        (*x).isInit_wld=false;
        (*x).textureArrayId_wld=0;
        (*x).animator_ID_wld=0;
    }
}

long  ConfigManager::getLvlPlayerTexture(long playerID, int stateID)
{
    if(!playable_characters.contains(playerID))
    {
        return -1;
    }

    obj_player &plr = playable_characters[playerID];

    if(!plr.states.contains(stateID))
    {
        return -1;
    }
    obj_player_state &state = plr.states[stateID];
    if(state.isInit)
    {

        if(state.textureArrayId < level_textures.size())
            return state.textureArrayId;
        else
            return -1;
    }
    else
    {
        bool isDefault=false;
        QString imgFile = Dir_PlayerLvl.getCustomFile(state.image_n, &isDefault);
        if(isDefault)
            imgFile = playerLvlPath+plr.sprite_folder+"/"+state.image_n;

        QString maskFile = Dir_PlayerLvl.getCustomFile(state.mask_n, &isDefault);
        if(isDefault)
            maskFile  = playerLvlPath+plr.sprite_folder+"/"+state.mask_n;

        PGE_Texture texture;
        texture.w = 0;
        texture.h = 0;
        texture.texture = 0;
        texture.texture_layout = NULL;
        texture.format = 0;
        texture.nOfColors = 0;

        long id = level_textures.size();

        state.textureArrayId = id;

        level_textures.push_back(texture);

        GraphicsHelps::loadTexture( level_textures[id],
             imgFile,
             maskFile
             );

        state.image = &(level_textures[id]);
        state.textureID = level_textures[id].texture;
        state.isInit = true;

        //Store size of one frame
        plr.frame_width = level_textures[id].w/plr.matrix_width;
        plr.frame_height = level_textures[id].h/plr.matrix_height;

        return id;
    }
}


long  ConfigManager::getWldPlayerTexture(long playerID, int stateID)
{
    if(!playable_characters.contains(playerID))
    {
        return -1;
    }
    obj_player &plr = playable_characters[playerID];

    if(!plr.states.contains(stateID))
    {
        return -1;
    }

    if(plr.isInit_wld)
    {

        if(plr.textureArrayId_wld < world_textures.size())
            return plr.textureArrayId_wld;
        else
            return -1;
    }
    else
    {
        bool isDefault=false;
        QString imgFile = Dir_PlayerWld.getCustomFile(plr.image_wld_n, &isDefault);
        if(isDefault)
            imgFile = playerWldPath+plr.image_wld_n;

        QString maskFile = Dir_PlayerWld.getCustomFile(plr.mask_wld_n, &isDefault);
        if(isDefault)
            maskFile  = playerWldPath+plr.mask_wld_n;

        PGE_Texture texture;
        texture.w = 0;
        texture.h = 0;
        texture.texture = 0;
        texture.texture_layout = NULL;
        texture.format = 0;
        texture.nOfColors = 0;

        long id = world_textures.size();

        plr.textureArrayId_wld = id;

        world_textures.push_back(texture);

        GraphicsHelps::loadTexture( world_textures[id],
             imgFile,
             maskFile
             );

        plr.image_wld = &(world_textures[id]);
        plr.textureID_wld = world_textures[id].texture;
        plr.isInit_wld = true;

        return id;
    }
}




long  ConfigManager::getBGTexture(long bgID, bool isSecond)
{
    if(!lvl_bg_indexes.contains(bgID))
    {
        return -1;
    }

    obj_BG*bgSetup=&lvl_bg_indexes[bgID];

    if( (bgSetup->isInit && !isSecond) || (bgSetup->second_isInit && isSecond) )
    {

        if(isSecond)
        {
            if(bgSetup->second_textureArrayId < level_textures.size())
                return bgSetup->second_textureArrayId;
            else
                return -1;
        }
        else
        {
            if(bgSetup->textureArrayId < level_textures.size())
                return bgSetup->textureArrayId;
            else
                return -1;
        }
    }
    else
    {
        QString imgFile ="";

        if(isSecond)
            imgFile = Dir_BG.getCustomFile(bgSetup->second_image_n);
        else
            imgFile = Dir_BG.getCustomFile(bgSetup->image_n);

        PGE_Texture texture;
        texture.w = 0;
        texture.h = 0;
        texture.texture = 0;
        texture.texture_layout = NULL;
        texture.format = 0;
        texture.nOfColors = 0;

        long id = level_textures.size();

        if(isSecond)
            bgSetup->second_textureArrayId = id;
        else
            bgSetup->textureArrayId = id;

        level_textures.push_back(texture);

        GraphicsHelps::loadTexture( level_textures[id], imgFile );

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
        if(bgSetup->animated && !isSecond)
        {
            int frameFirst = 0;
            int frameLast = -1;

            //calculate height of frame
            bgSetup->frame_h =
                    (int)round(double(level_textures[id].h)
                               /double(bgSetup->frames));

            //store animated texture value back
            level_textures[id].h = bgSetup->frame_h;

            SimpleAnimator animator(
                            true,
                            bgSetup->frames,
                            bgSetup->framespeed, //bgSetup->framespeed, //Ouch, forgot made framespeed value for background :P Will add later...
                                                 //3 june 2015. I Finally implemented this sweet variable :D
                            frameFirst,
                            frameLast,
                            false,
                            false
                        );

            Animator_BG.push_back(animator);
            bgSetup->animator_ID = Animator_BG.size()-1;
        }

        return id;
    }
}















/****************************World map items*****************************/

long  ConfigManager::getTileTexture(long tileID)
{
    if(!wld_tiles.contains(tileID))
    {
        return -1;
    }

    obj_w_tile* tileSetup=&wld_tiles[tileID];

    if(tileSetup->isInit)
    {

        if(tileSetup->textureArrayId < world_textures.size())
            return tileSetup->textureArrayId;
        else
            return -1;
    }
    else
    {
        QString imgFile = Dir_Tiles.getCustomFile(tileSetup->image_n);
        QString maskFile = Dir_Tiles.getCustomFile(tileSetup->mask_n);

        PGE_Texture texture;
        texture.w = 0;
        texture.h = 0;
        texture.texture = 0;
        texture.texture_layout = NULL;
        texture.format = 0;
        texture.nOfColors = 0;

        long id = world_textures.size();

        tileSetup->textureArrayId = id;

        world_textures.push_back(texture);

        GraphicsHelps::loadTexture( world_textures[id],
             imgFile,
             maskFile
             );

        tileSetup->image = &(world_textures[id]);
        tileSetup->textureID = world_textures[id].texture;
        tileSetup->isInit = true;

        //Also, load and init animator
        if(tileSetup->animated)
        {
            int frameFirst = 0;
            int frameLast = -1;

            //calculate height of frame
            tileSetup->frame_h =
                    (int)round(double(world_textures[id].h)
                               /double(tileSetup->frames));

            //store animated texture value back
            world_textures[id].h = tileSetup->frame_h;

            SimpleAnimator animator(
                            true,
                            tileSetup->frames,
                            tileSetup->framespeed,
                            frameFirst,
                            frameLast,
                            false,
                            false
                        );

            Animator_Tiles.push_back(animator);
            tileSetup->animator_ID = Animator_Tiles.size()-1;
        }

        return id;
    }
}


long  ConfigManager::getSceneryTexture(long sceneryID)
{
    if(!wld_scenery.contains(sceneryID))
    {
        return -1;
    }

    obj_w_scenery* scenerySetup=&wld_scenery[sceneryID];

    if(scenerySetup->isInit)
    {

        if(scenerySetup->textureArrayId < world_textures.size())
            return scenerySetup->textureArrayId;
        else
            return -1;
    }
    else
    {
        QString imgFile = Dir_Scenery.getCustomFile(scenerySetup->image_n);
        QString maskFile = Dir_Scenery.getCustomFile(scenerySetup->mask_n);

        PGE_Texture texture;
        texture.w = 0;
        texture.h = 0;
        texture.texture = 0;
        texture.texture_layout = NULL;
        texture.format = 0;
        texture.nOfColors = 0;

        long id = world_textures.size();

        scenerySetup->textureArrayId = id;

        world_textures.push_back(texture);

        GraphicsHelps::loadTexture( world_textures[id],
             imgFile,
             maskFile
             );

        scenerySetup->image = &(world_textures[id]);
        scenerySetup->textureID = world_textures[id].texture;
        scenerySetup->isInit = true;

        //Also, load and init animator
        if(scenerySetup->animated)
        {
            int frameFirst = 0;
            int frameLast = -1;

            //calculate height of frame
            scenerySetup->frame_h =
                    (int)round(double(world_textures[id].h)
                               /double(scenerySetup->frames));

            //store animated texture value back
            world_textures[id].h = scenerySetup->frame_h;

            SimpleAnimator animator(
                            true,
                            scenerySetup->frames,
                            scenerySetup->framespeed,
                            frameFirst,
                            frameLast,
                            false,
                            false
                        );

            Animator_Scenery.push_back(animator);
            scenerySetup->animator_ID = Animator_Scenery.size()-1;
        }

        return id;
    }
}


long  ConfigManager::getWldPathTexture(long pathID)
{
    if(!wld_paths.contains(pathID))
    {
        return -1;
    }

    obj_w_path* pathSetup=&wld_paths[pathID];

    if(pathSetup->isInit)
    {

        if(pathSetup->textureArrayId < world_textures.size())
            return pathSetup->textureArrayId;
        else
            return -1;
    }
    else
    {
        QString imgFile = Dir_WldPaths.getCustomFile(pathSetup->image_n);
        QString maskFile = Dir_WldPaths.getCustomFile(pathSetup->mask_n);

        PGE_Texture texture;
        texture.w = 0;
        texture.h = 0;
        texture.texture = 0;
        texture.texture_layout = NULL;
        texture.format = 0;
        texture.nOfColors = 0;

        long id = world_textures.size();

        pathSetup->textureArrayId = id;

        world_textures.push_back(texture);

        GraphicsHelps::loadTexture( world_textures[id],
             imgFile,
             maskFile
             );

        pathSetup->image = &(world_textures[id]);
        pathSetup->textureID = world_textures[id].texture;
        pathSetup->isInit = true;

        //Also, load and init animator
        if(pathSetup->animated)
        {
            int frameFirst = 0;
            int frameLast = -1;

            //calculate height of frame
            pathSetup->frame_h =
                    (int)round(double(world_textures[id].h)
                               /double(pathSetup->frames));

            //store animated texture value back
            world_textures[id].h = pathSetup->frame_h;

            SimpleAnimator animator(
                            true,
                            pathSetup->frames,
                            pathSetup->framespeed,
                            frameFirst,
                            frameLast,
                            false,
                            false
                        );

            Animator_WldPaths.push_back(animator);
            pathSetup->animator_ID = Animator_WldPaths.size()-1;
        }

        return id;
    }
}



long  ConfigManager::getWldLevelTexture(long levelID)
{
    if(!wld_levels.contains(levelID))
    {
        return -1;
    }

    obj_w_level* lvlSetup=&wld_levels[levelID];

    if(lvlSetup->isInit)
    {

        if(lvlSetup->textureArrayId < world_textures.size())
            return lvlSetup->textureArrayId;
        else
            return -1;
    }
    else
    {
        QString imgFile = Dir_WldLevel.getCustomFile(lvlSetup->image_n);
        QString maskFile = Dir_WldLevel.getCustomFile(lvlSetup->mask_n);

        PGE_Texture texture;
        texture.w = 0;
        texture.h = 0;
        texture.texture = 0;
        texture.texture_layout = NULL;
        texture.format = 0;
        texture.nOfColors = 0;

        long id = world_textures.size();

        lvlSetup->textureArrayId = id;

        world_textures.push_back(texture);

        GraphicsHelps::loadTexture( world_textures[id],
             imgFile,
             maskFile
             );

        lvlSetup->image = &(world_textures[id]);
        lvlSetup->textureID = world_textures[id].texture;
        lvlSetup->isInit = true;

        //Also, load and init animator
        if(lvlSetup->animated)
        {
            int frameFirst = 0;
            int frameLast = -1;

            //calculate height of frame
            lvlSetup->frame_h =
                    (int)round(double(world_textures[id].h)
                               /double(lvlSetup->frames));

            //store animated texture value back
            world_textures[id].h = lvlSetup->frame_h;

            SimpleAnimator animator(
                            true,
                            lvlSetup->frames,
                            lvlSetup->framespeed,
                            frameFirst,
                            frameLast,
                            false,
                            false
                        );

            Animator_WldLevel.push_back(animator);
            lvlSetup->animator_ID = Animator_WldLevel.size()-1;
        }

        return id;
    }
}



