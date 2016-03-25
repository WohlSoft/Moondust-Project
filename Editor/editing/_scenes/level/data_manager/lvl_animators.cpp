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
#include <PGE_File_Formats/file_formats.h>

#include "../../../../defines.h"

#include "../lvl_scene.h"


///
/// \brief LvlScene::buildAnimators
///
/// Build animators without custom graphics
///
void LvlScene::buildAnimators()
{
    uBGOs.clear();
    uBGOs.allocateSlots(pConfigs->main_bgo.total());
    for(int i=1; i<pConfigs->main_bgo.size(); i++) //Add user images
    {
        obj_bgo *bgoD = &pConfigs->main_bgo[i];
        obj_bgo t_bgo;
        bgoD->copyTo(t_bgo);

        SimpleAnimator * aniBGO = new SimpleAnimator(
                         ((t_bgo.cur_image->isNull()) ?
                                uBgoImg:
                               *t_bgo.cur_image),
                                t_bgo.animated,
                                t_bgo.frames,
                                t_bgo.framespeed
                              );

        animates_BGO.push_back( aniBGO );
        t_bgo.animator_id = animates_BGO.size()-1;
        uBGOs.storeElement(i, t_bgo);
    }

    uBlocks.clear();
    uBlocks.allocateSlots(pConfigs->main_block.total());
    for(int i=1; i<pConfigs->main_block.size(); i++) //Add user images
    {
        obj_block *blockD = &pConfigs->main_block[i];
        obj_block t_block;
        blockD->copyTo(t_block);
        #ifdef _DEBUG_
        WriteToLog(QtDebugMsg, QString("Block Animator ID: %1").arg(i));
        #endif

        int frameFirst;
        int frameLast;

        switch(t_block.algorithm)
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
                         ((t_block.cur_image->isNull())?
                                uBlockImg:
                                *t_block.cur_image),
                                t_block.animated,
                                t_block.frames,
                                t_block.framespeed, frameFirst, frameLast,
                                t_block.animation_rev,
                                t_block.animation_bid
                              );

        animates_Blocks.push_back( aniBlock );
        t_block.animator_id = animates_Blocks.size()-1;
        uBlocks.storeElement(i, t_block);
    }

    uNPCs.clear();
    uNPCs.allocateSlots(pConfigs->main_npc.total());
    for(int i=1; i<pConfigs->main_npc.size(); i++) //Add user images
    {
        obj_npc &npcD = pConfigs->main_npc[i];
        obj_npc t_npc;
        npcD.copyTo(t_npc);

        AdvNpcAnimator * aniNPC = new AdvNpcAnimator(
                         ((t_npc.cur_image->isNull())?
                                uNpcImg:
                               *t_npc.cur_image),
                              t_npc
                              );

        animates_NPC.push_back( aniNPC );
        t_npc.animator_id = animates_NPC.size()-1;
        uNPCs.storeElement(i, t_npc);
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
        LogWarning(QString("Can't start animation: too many items on map: %1").arg(q));
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
    _edit->setAutoUpdateTimer(31);
}

void LvlScene::stopAnimation()
{
    int i=0;
    int size=animates_BGO.size();
    for(i=0;i<size; i++)
    {
        animates_BGO[i]->stop();
    }
    size=animates_Blocks.size();
    for(i=0;i<size; i++)
    {
        animates_Blocks[i]->stop();
    }
    size=animates_NPC.size();
    for(i=0;i<size; i++)
    {
        animates_NPC[i]->stop();
    }
    _edit->stopAutoUpdateTimer();
    update();
}
