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


#include "../lvl_scene.h"

void LvlScene::getConfig_Block(unsigned long item_id, long &array_index, long &animator_id, bool *ok)
{
    bool noimage=true, found=false;
    long j, item_i=0;
    long animator=0;

    //Check Index exists
    if(item_id < index_blocks.size())
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

    if(ok)
    {
        *ok = noimage;
    }
}
