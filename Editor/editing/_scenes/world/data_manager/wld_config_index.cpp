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

#include "../wld_scene.h"

void WldScene::getConfig_Tile(unsigned long item_id, long &array_index, long &animator_id, obj_w_tile &mergedSet, bool *ok)
{
    bool noimage=true, found=false;
    int j, item_i = 0;
    long animator=0;

    //Check Index exists
    if(item_id < (unsigned int)index_tiles.size())
    {
        j = index_tiles[item_id].i;
        item_i = j;
        animator = index_tiles[item_id].ai;

        if(j<pConfigs->main_wtiles.size())
        {
            if(pConfigs->main_wtiles[j].id == item_id)
            {
                found=true;noimage=false;
            }
        }
    }

    //if Index found
    if(!found)
    {
        for(j=0;j<pConfigs->main_wtiles.size();j++)
        {
            if(pConfigs->main_wtiles[j].id==item_id)
            {
                noimage=false;
                item_i = j;
                break;
                //if(!isUser)
            }
        }
    }

    if(noimage)
    {
        if(j >= pConfigs->main_wtiles.size())
        {
            j=0;
            item_i = j;
        }
    }

    array_index = item_i;
    animator_id = animator;
    mergedSet = pConfigs->main_wtiles[item_i];

    mergedSet.image = QPixmap(0,0);
    mergedSet.mask = QPixmap(0,0);

    if(ok)
    {
        *ok = noimage;
    }
}

void WldScene::getConfig_Scenery(unsigned long item_id, long &array_index, long &animator_id, obj_w_scenery &mergedSet, bool *ok)
{
    bool noimage=true, found=false;
    int j, item_i = 0;
    long animator=0;

    //Check Index exists
    if(item_id < (unsigned int)index_scenes.size())
    {
        j = index_scenes[item_id].i;
        item_i = j;
        animator = index_scenes[item_id].ai;

        if(j<pConfigs->main_wscene.size())
        {
            if(pConfigs->main_wscene[j].id == item_id)
            {
                found=true;noimage=false;
            }
        }
    }


    //if Index found
    if(!found)
    {
        for(j=0;j<pConfigs->main_wscene.size();j++)
        {
            if(pConfigs->main_wscene[j].id==item_id)
            {
                noimage=false;
                item_i = j;
                break;
            }
        }
    }

    if(noimage)
    {
        if(j >= pConfigs->main_wscene.size())
        {
            j=0;
            item_i = j;
        }
    }

    array_index = item_i;
    animator_id = animator;
    mergedSet = pConfigs->main_wscene[item_i];

    mergedSet.image = QPixmap(0,0);
    mergedSet.mask = QPixmap(0,0);

    if(ok)
    {
        *ok = noimage;
    }
}

void WldScene::getConfig_Path(unsigned long item_id, long &array_index, long &animator_id, obj_w_path &mergedSet, bool *ok)
{
    bool noimage=true, found=false;
    int j, item_i=0;
    long animator=0;

    //Check Index exists
    if(item_id < (unsigned int)index_paths.size())
    {
        j = index_paths[item_id].i;
        item_i = j;
        animator = index_paths[item_id].ai;

        if(j<pConfigs->main_wpaths.size())
        {
            if(pConfigs->main_wpaths[j].id == item_id)
            {
                found=true;noimage=false;
            }
        }
    }

    //if Index found
    if(!found)
    {
        for(j=0;j<pConfigs->main_wpaths.size();j++)
        {
            if(pConfigs->main_wpaths[j].id==item_id)
            {
                noimage=false;
                item_i = j;
                break;
                //if(!isUser)
            }
        }
    }

    if(noimage)
    {
        if(j >= pConfigs->main_wpaths.size())
        {
            j=0;
            item_i = 0;
        }
    }

    array_index = item_i;
    animator_id = animator;
    mergedSet = pConfigs->main_wpaths[item_i];

    mergedSet.image = QPixmap(0,0);
    mergedSet.mask = QPixmap(0,0);

    if(ok)
    {
        *ok = noimage;
    }
}

void WldScene::getConfig_Level(unsigned long item_id, long &array_index, long &animator_id, obj_w_level &mergedSet, bool *ok)
{
    bool noimage=true, found=false;
    int j, item_i = 0;
    long animator=0;

    //Check Index exists
    if(item_id < (unsigned int)index_levels.size())
    {
        j = index_levels[item_id].i;
        item_i = j;
        animator = index_levels[item_id].ai;

        if(j<pConfigs->main_wlevels.size())
        {
            if(pConfigs->main_wlevels[j].id == item_id)
            {
                found=true;noimage=false;
            }
        }
    }

    //if Index found
    if(!found)
    {
        for(j=0;j<pConfigs->main_wlevels.size();j++)
        {
            if(pConfigs->main_wlevels[j].id==item_id)
            {
                noimage=false;
                item_i = j;
                break;
                //if(!isUser)
            }
        }
    }

    if(noimage)
    {
        if(j >= pConfigs->main_wlevels.size())
        {
            j=0;
            item_i = j;
        }
    }

    array_index = item_i;
    animator_id = animator;
    mergedSet = pConfigs->main_wlevels[item_i];

    mergedSet.image = QPixmap(0,0);
    mergedSet.mask = QPixmap(0,0);

    if(ok)
    {
        *ok = noimage;
    }

}
