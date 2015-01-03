/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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


#include "../lvl_scene.h"

void LvlScene::getConfig_Block(unsigned long item_id, long &array_index, long &animator_id, obj_block &mergedSet, bool *ok)
{
    bool noimage=true, found=false;
    long j, item_i=0;
    long animator=0;

    //Check Index exists
    if(item_id < (unsigned long)index_blocks.size())
    {
        j = index_blocks[item_id].i;
        item_i = j;
        animator = index_blocks[item_id].ai;

        if(j<pConfigs->main_block.size())
        {
            if(pConfigs->main_block[j].id == item_id)
            {
                found=true;noimage=false;
            }
        }
    }

    //if Index found
    if(!found)
    {
        for(j=0;j<pConfigs->main_block.size();j++)
        {
            if(pConfigs->main_block[j].id == item_id)
            {
                noimage=false;
                item_i = j;
                break;
            }
        }
    }

    if(noimage)
    {
        if(j >= pConfigs->main_block.size())
        {
            j=0;
            item_i = j;
        }
    }

    array_index = item_i;
    animator_id = animator;

    mergedSet = pConfigs->main_block[item_i];

    mergedSet.image = QPixmap(0,0);
    mergedSet.mask = QPixmap(0,0);

    if(ok)
    {
        *ok = noimage;
    }
}





void LvlScene::getConfig_BGO(unsigned long item_id, long &array_index, long &animator_id, obj_bgo &mergedSet, bool *ok)
{
    bool noimage=true, found=false;
    long j, item_i=0;
    long animator=0;

    //Check Index exists
    if(item_id < (unsigned int)index_bgo.size())
    {
        j = index_bgo[item_id].i;
        item_i = j;
        animator = index_bgo[item_id].ai;
        if(j<pConfigs->main_bgo.size())
        {
        if(pConfigs->main_bgo[j].id == item_id)
            found=true;noimage=false;
        }
    }

    //if Index found
    if(!found)
    {
        for(j=0;j<pConfigs->main_bgo.size();j++)
        {
            if(pConfigs->main_bgo[j].id==item_id)
            {
                noimage=false;
                item_i = j;
                break;
            }
        }
    }

    if(noimage)
    {
        if(j >= pConfigs->main_bgo.size())
        {
            j=0;
            item_i = j;
        }
    }

    array_index = item_i;
    animator_id = animator;

    mergedSet = pConfigs->main_bgo[item_i];

    mergedSet.image = QPixmap(0,0);
    mergedSet.mask = QPixmap(0,0);

    if(ok)
    {
        *ok = noimage;
    }
}





void LvlScene::getConfig_NPC(unsigned long item_id, long &array_index, long &animator_id, obj_npc &mergedSet, bool *ok)
{
    int j, item_i = 0;
    bool noimage=true, found=false;
    bool isUser=false;
    bool isUserTxt=false;

    long animator=0;

    //Check Index exists
    if(item_id < (unsigned int)index_npc.size())
    {
        j = index_npc[item_id].gi;
        item_i = j;
        animator  = index_npc[item_id].ai;

        if(j<pConfigs->main_npc.size())
        {
            if(pConfigs->main_npc[j].id == item_id)
                found=true;
        }
    }

    //if Index found
    if(found)
    {   //get neccesary element directly
        if(index_npc[item_id].type==1)
        {
            isUser=true;
            if(uNPCs[index_npc[item_id].i].withImg)
            {
                noimage=false;
                tImg = uNPCs[index_npc[item_id].i].image;
            }

            if(uNPCs[index_npc[item_id].i].withTxt)
            {
                isUserTxt=true;
                mergedSet = uNPCs[index_npc[item_id].i].merged;
            }
            else
                mergedSet = pConfigs->main_npc[index_npc[item_id].gi];
        }

        if(!noimage)
        {
            tImg = pConfigs->main_npc[ index_npc[item_id].gi].image;
            noimage=false;
        }
    }
    else
    {
        //fetching arrays
        for(j=0;j<uNPCs.size();j++)
        {
            if(uNPCs[j].id==item_id)
            {
                if(uNPCs[j].withImg)
                {
                    noimage=false;
                    isUser=true;
                    tImg = uNPCs[j].image;
                }

                if(uNPCs[j].withTxt)
                {
                    isUserTxt=true;
                    mergedSet = uNPCs[j].merged;
                }
                break;
            }
        }

        for(j=0;j<pConfigs->main_npc.size();j++)
        {
            if(pConfigs->main_npc[j].id==item_id)
            {
                noimage=false;
                item_i = j;
                if(!isUser)
                    tImg = pConfigs->main_npc[j].image;
                if(!isUserTxt)
                    mergedSet = pConfigs->main_npc[item_i];
                break;
            }
        }
    }

    if((noimage)||(tImg.isNull()))
    {
        tImg=uNpcImg;
        if(j >= pConfigs->main_npc.size())
        {
            j=0;
            item_i = j;
            mergedSet = pConfigs->main_npc[item_i];
        }
    }

    array_index = item_i;
    animator_id = animator;

    if(ok)
    {
        *ok = noimage;
    }

}
