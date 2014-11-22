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

#include "config_manager.h"
#include "../common_features/graphics_funcs.h"

long  ConfigManager::getBlockTexture(long blockID)
{
    if(!lvl_block_indexes.contains(blockID))
    {
        return -1;
    }

    if(lvl_block_indexes[blockID].isInit)
    {

        if(lvl_block_indexes[blockID].textureArrayId < level_textures.size())
            return lvl_block_indexes[blockID].textureArrayId;
        else
            return -1;
    }
    else
    {
        QString imgFile = Dir_Blocks.getCustomFile(lvl_block_indexes[blockID].image_n);
        QString maskFile = Dir_Blocks.getCustomFile(lvl_block_indexes[blockID].mask_n);

        PGE_Texture texture;
        texture.w = 0;
        texture.h = 0;
        texture.texture = 0;
        texture.texture_layout = NULL;
        texture.format = 0;
        texture.nOfColors = 0;

        long id = level_textures.size();

        lvl_block_indexes[blockID].textureArrayId = id;

        level_textures.push_back(texture);

        GraphicsHelps::loadTexture( level_textures[id],
             imgFile,
             maskFile
             );

        lvl_block_indexes[blockID].image = &(level_textures[id]);
        lvl_block_indexes[blockID].textureID = level_textures[id].texture;
        lvl_block_indexes[blockID].isInit = true;

        //Also, load and init animator
        if(lvl_block_indexes[blockID].animated)
        {
            int frameFirst;
            int frameLast;

            switch(lvl_block_indexes[blockID].algorithm)
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
                            lvl_block_indexes[blockID].frames,
                            lvl_block_indexes[blockID].framespeed,
                            frameFirst,
                            frameLast,
                            lvl_block_indexes[blockID].animation_rev,
                            lvl_block_indexes[blockID].animation_bid
                        );

            Animator_Blocks.push_back(animator);
            lvl_block_indexes[blockID].animator_ID = Animator_Blocks.size()-1;

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

    if(lvl_bgo_indexes[bgoID].isInit)
    {

        if(lvl_bgo_indexes[bgoID].textureArrayId < level_textures.size())
            return lvl_bgo_indexes[bgoID].textureArrayId;
        else
            return -1;
    }
    else
    {
        QString imgFile = Dir_BGO.getCustomFile(lvl_bgo_indexes[bgoID].image_n);
        QString maskFile = Dir_BGO.getCustomFile(lvl_bgo_indexes[bgoID].mask_n);

        PGE_Texture texture;
        texture.w = 0;
        texture.h = 0;
        texture.texture = 0;
        texture.texture_layout = NULL;
        texture.format = 0;
        texture.nOfColors = 0;

        long id = level_textures.size();

        lvl_bgo_indexes[bgoID].textureArrayId = id;

        level_textures.push_back(texture);

        GraphicsHelps::loadTexture( level_textures[id],
             imgFile,
             maskFile
             );

        lvl_bgo_indexes[bgoID].image = &(level_textures[id]);
        lvl_bgo_indexes[bgoID].textureID = level_textures[id].texture;
        lvl_bgo_indexes[bgoID].isInit = true;

        //Also, load and init animator
        if(lvl_bgo_indexes[bgoID].animated)
        {
            int frameFirst = 0;
            int frameLast = -1;

            //calculate height of frame
            lvl_bgo_indexes[bgoID].frame_h =
                    (int)round(double(level_textures[id].h)
                               /double(lvl_bgo_indexes[bgoID].frames));

            //store animated texture value back
            level_textures[id].h = lvl_bgo_indexes[bgoID].frame_h;

            SimpleAnimator animator(
                            true,
                            lvl_bgo_indexes[bgoID].frames,
                            lvl_bgo_indexes[bgoID].framespeed,
                            frameFirst,
                            frameLast,
                            false,
                            false
                        );

            Animator_BGO.push_back(animator);
            lvl_bgo_indexes[bgoID].animator_ID = Animator_BGO.size()-1;

        }

        return id;
    }
}










long  ConfigManager::getBGTexture(long bgID, bool isSecond)
{
    if(!lvl_bg_indexes.contains(bgID))
    {
        return -1;
    }

    if( (lvl_bg_indexes[bgID].isInit && !isSecond) || (lvl_bg_indexes[bgID].second_isInit && isSecond) )
    {

        if(isSecond)
        {
            if(lvl_bg_indexes[bgID].second_textureArrayId < level_textures.size())
                return lvl_bg_indexes[bgID].second_textureArrayId;
            else
                return -1;
        }
        else
        {
            if(lvl_bg_indexes[bgID].textureArrayId < level_textures.size())
                return lvl_bg_indexes[bgID].textureArrayId;
            else
                return -1;
        }
    }
    else
    {
        QString imgFile ="";

        if(isSecond)
            imgFile = Dir_BG.getCustomFile(lvl_bg_indexes[bgID].second_image_n);
        else
            imgFile = Dir_BG.getCustomFile(lvl_bg_indexes[bgID].image_n);

        PGE_Texture texture;
        texture.w = 0;
        texture.h = 0;
        texture.texture = 0;
        texture.texture_layout = NULL;
        texture.format = 0;
        texture.nOfColors = 0;

        long id = level_textures.size();

        if(isSecond)
            lvl_bg_indexes[bgID].second_textureArrayId = id;
        else
            lvl_bg_indexes[bgID].textureArrayId = id;

        level_textures.push_back(texture);

        GraphicsHelps::loadTexture( level_textures[id], imgFile );

        if(isSecond)
        {
            lvl_bg_indexes[bgID].second_image = &(level_textures[id]);
            lvl_bg_indexes[bgID].second_textureID = level_textures[id].texture;
            lvl_bg_indexes[bgID].second_isInit = true;
            lvl_bg_indexes[bgID].second_Color_upper = level_textures[id].ColorUpper;
            lvl_bg_indexes[bgID].second_Color_lower = level_textures[id].ColorLower;
        }
        else
        {
            lvl_bg_indexes[bgID].image = &(level_textures[id]);
            lvl_bg_indexes[bgID].textureID = level_textures[id].texture;
            lvl_bg_indexes[bgID].Color_upper = level_textures[id].ColorUpper;
            lvl_bg_indexes[bgID].Color_lower = level_textures[id].ColorLower;
            lvl_bg_indexes[bgID].isInit = true;
        }

        //Also, load and init animator
        if(lvl_bg_indexes[bgID].animated && !isSecond)
        {
            int frameFirst = 0;
            int frameLast = -1;

            //calculate height of frame
            lvl_bg_indexes[bgID].frame_h =
                    (int)round(double(level_textures[id].h)
                               /double(lvl_bg_indexes[bgID].frames));

            //store animated texture value back
            level_textures[id].h = lvl_bg_indexes[bgID].frame_h;

            SimpleAnimator animator(
                            true,
                            lvl_bg_indexes[bgID].frames,
                            128, //lvl_bg_indexes[bgID].framespeed, //Ouch, forgot made framespeed value for background :P Will add later
                            frameFirst,
                            frameLast,
                            false,
                            false
                        );

            Animator_BG.push_back(animator);
            lvl_bg_indexes[bgID].animator_ID = Animator_BG.size()-1;
        }

        return id;
    }
}
