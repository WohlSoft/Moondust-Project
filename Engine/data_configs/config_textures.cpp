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
    obj_block *blkSetup = lvl_block_indexes[blockID];

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

    obj_bgo* bgoSetup=lvl_bgo_indexes[bgoID];

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

    obj_effect*effSetup=lvl_effects_indexes[effectID];

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

    if(!playable_characters[playerID].states.contains(stateID))
    {
        return -1;
    }

    if(playable_characters[playerID].states[stateID].isInit)
    {

        if(playable_characters[playerID].states[stateID].textureArrayId < level_textures.size())
            return playable_characters[playerID].states[stateID].textureArrayId;
        else
            return -1;
    }
    else
    {
        bool isDefault=false;
        QString imgFile = Dir_PlayerLvl.getCustomFile(playable_characters[playerID].states[stateID].image_n, &isDefault);
        if(isDefault)
            imgFile = playerLvlPath+playable_characters[playerID].sprite_folder+"/"+playable_characters[playerID].states[stateID].image_n;

        QString maskFile = Dir_PlayerLvl.getCustomFile(playable_characters[playerID].states[stateID].mask_n, &isDefault);
        if(isDefault)
            maskFile  = playerLvlPath+playable_characters[playerID].sprite_folder+"/"+playable_characters[playerID].states[stateID].mask_n;

        PGE_Texture texture;
        texture.w = 0;
        texture.h = 0;
        texture.texture = 0;
        texture.texture_layout = NULL;
        texture.format = 0;
        texture.nOfColors = 0;

        long id = level_textures.size();

        playable_characters[playerID].states[stateID].textureArrayId = id;

        level_textures.push_back(texture);

        GraphicsHelps::loadTexture( level_textures[id],
             imgFile,
             maskFile
             );

        playable_characters[playerID].states[stateID].image = &(level_textures[id]);
        playable_characters[playerID].states[stateID].textureID = level_textures[id].texture;
        playable_characters[playerID].states[stateID].isInit = true;

        //Store size of one frame
        playable_characters[playerID].frame_width = level_textures[id].w/playable_characters[playerID].matrix_width;
        playable_characters[playerID].frame_height = level_textures[id].h/playable_characters[playerID].matrix_height;

        return id;
    }
}





long  ConfigManager::getBGTexture(long bgID, bool isSecond)
{
    if(!lvl_bg_indexes.contains(bgID))
    {
        return -1;
    }

    obj_BG*bgSetup=lvl_bg_indexes[bgID];

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
                            128, //bgSetup->framespeed, //Ouch, forgot made framespeed value for background :P Will add later
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
