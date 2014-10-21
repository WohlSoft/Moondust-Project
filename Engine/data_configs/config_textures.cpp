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
                    frameFirst = 5;
                    frameLast = 6;
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


            SimpleAnimator * animator = new SimpleAnimator
                        (
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

            SimpleAnimator * animator = new SimpleAnimator
                        (
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
