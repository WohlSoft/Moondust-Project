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

#include <common_features/graphics_funcs.h>
#include <editing/edit_level/level_edit.h>
#include <file_formats/file_formats.h>

#include "../../../../defines.h"

#include "../lvl_scene.h"


///
/// \brief LvlScene::buildAnimators
///
/// Build animators without custom graphics
///
void LvlScene::buildAnimators()
{
    int i;
    for(i=0; i<pConfigs->main_bgo.size(); i++) //Add user images
    {
        SimpleAnimator * aniBGO = new SimpleAnimator(
                         ((pConfigs->main_bgo[i].image.isNull())?
                                uBgoImg:
                               pConfigs->main_bgo[i].image),
                              pConfigs->main_bgo[i].animated,
                              pConfigs->main_bgo[i].frames,
                              pConfigs->main_bgo[i].framespeed
                              );

        animates_BGO.push_back( aniBGO );
        if(pConfigs->main_bgo[i].id < (unsigned int)index_blocks.size())
        {
            index_bgo[pConfigs->main_bgo[i].id].ai = animates_BGO.size()-1;
        }
    }

    for(i=0; i<pConfigs->main_block.size(); i++) //Add user images
    {
        #ifdef _DEBUG_
        WriteToLog(QtDebugMsg, QString("Block Animator ID: %1").arg(i));
        #endif

        int frameFirst;
        int frameLast;

        switch(pConfigs->main_block[i].algorithm)
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

        SimpleAnimator * aniBlock = new SimpleAnimator(
                         ((pConfigs->main_block[i].image.isNull())?
                                uBgoImg:
                                pConfigs->main_block[i].image),
                                pConfigs->main_block[i].animated,
                                pConfigs->main_block[i].frames,
                                pConfigs->main_block[i].framespeed, frameFirst, frameLast,
                                pConfigs->main_block[i].animation_rev,
                                pConfigs->main_block[i].animation_bid
                              );

        animates_Blocks.push_back( aniBlock );
        if(pConfigs->main_block[i].id < (unsigned int)index_blocks.size())
        {
            index_blocks[pConfigs->main_block[i].id].ai = animates_Blocks.size()-1;
        }
    }

    for(i=0; i<pConfigs->main_npc.size(); i++) //Add user images
    {
        AdvNpcAnimator * aniNPC = new AdvNpcAnimator(
                         ((pConfigs->main_npc[i].image.isNull())?
                                uNpcImg:
                               pConfigs->main_npc[i].image),
                              pConfigs->main_npc[i]
                              );

        animates_NPC.push_back( aniNPC );
        if(pConfigs->main_npc[i].id < (unsigned int)index_npc.size())
        {
            index_npc[pConfigs->main_npc[i].id].ai = animates_NPC.size()-1;
        }
    }

}



////////////////////////////////////Animator////////////////////////////////
void LvlScene::startAnimation()
{
    long q = LvlData->blocks.size();
    q+= LvlData->bgo.size();
    q+= LvlData->npc.size();

    if( q > GlobalSettings::animatorItemsLimit )
    {
        WriteToLog(QtWarningMsg,
           QString("Can't start animation: too many items on map: %1").arg(q));
        return;
    }

    foreach(SimpleAnimator * bgoA, animates_BGO)
    {
        bgoA->start();
    }
    foreach(SimpleAnimator * blockA, animates_Blocks)
    {
        blockA->start();
    }
    foreach(AdvNpcAnimator * npcA, animates_NPC)
    {
        npcA->start();
    }
}

void LvlScene::stopAnimation()
{
    foreach(SimpleAnimator * bgoA, animates_BGO)
    {
        bgoA->stop();
    }
    foreach(SimpleAnimator * blockA, animates_Blocks)
    {
        blockA->stop();
    }
    foreach(AdvNpcAnimator * npcA, animates_NPC)
    {
        npcA->stop();
    }

    update();
}
