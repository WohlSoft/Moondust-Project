/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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
    m_localConfigBGOs.clear();
    m_localConfigBGOs.allocateSlots(m_configs->main_bgo.total());
    for(int i=1; i<m_configs->main_bgo.size(); i++) //Add user images
    {
        obj_bgo *bgoD = &m_configs->main_bgo[i];
        obj_bgo t_bgo;
        bgoD->copyTo(t_bgo);

        SimpleAnimator * aniBGO = new SimpleAnimator(
                         ((t_bgo.cur_image->isNull()) ?
                                m_dummyBgoImg:
                               *t_bgo.cur_image),
                                t_bgo.setup.animated,
                                t_bgo.setup.frames,
                                t_bgo.setup.framespeed
                              );

        m_animatorsBGO.push_back( aniBGO );
        m_animationTimer.registerAnimation( aniBGO );
        t_bgo.animator_id = m_animatorsBGO.size()-1;
        m_localConfigBGOs.storeElement(i, t_bgo);
    }

    m_localConfigBlocks.clear();
    m_localConfigBlocks.allocateSlots(m_configs->main_block.total());
    for(int i=1; i<m_configs->main_block.size(); i++) //Add user images
    {
        obj_block *blockD = &m_configs->main_block[i];
        obj_block t_block;
        blockD->copyTo(t_block);
        #ifdef _DEBUG_
        WriteToLog(QtDebugMsg, QString("Block Animator ID: %1").arg(i));
        #endif

        SimpleAnimator * aniBlock = new SimpleAnimator(
                         ((t_block.cur_image->isNull())?
                                m_dummyBlockImg:
                                *t_block.cur_image),
                                t_block.setup.animated,
                                t_block.setup.frames,
                                t_block.setup.framespeed, 0, -1,
                                t_block.setup.animation_rev,
                                t_block.setup.animation_bid
                              );

        if(!t_block.setup.frame_sequence.isEmpty())
            aniBlock->setFrameSequance(t_block.setup.frame_sequence);

        m_animatorsBlocks.push_back( aniBlock );
        m_animationTimer.registerAnimation( aniBlock );
        t_block.animator_id = m_animatorsBlocks.size()-1;
        m_localConfigBlocks.storeElement(i, t_block);
    }

    m_localConfigNPCs.clear();
    m_localConfigNPCs.allocateSlots(m_configs->main_npc.total());
    for(int i=1; i<m_configs->main_npc.size(); i++) //Add user images
    {
        obj_npc &npcD = m_configs->main_npc[i];
        obj_npc t_npc;
        npcD.copyTo(t_npc);

        AdvNpcAnimator * aniNPC = new AdvNpcAnimator(
                         ((t_npc.cur_image->isNull())?
                                m_dummyNpcImg:
                               *t_npc.cur_image),
                              t_npc
                              );

        m_animatorsNPC.push_back( aniNPC );
        t_npc.animator_id = m_animatorsNPC.size()-1;
        m_localConfigNPCs.storeElement(i, t_npc);
    }
}



////////////////////////////////////Animator////////////////////////////////
void LvlScene::startAnimation()
{
    long q = m_data->blocks.size();
    q+= m_data->bgo.size();
    q+= m_data->npc.size();

    if( q > GlobalSettings::animatorItemsLimit )
    {
        LogWarning(QString("Can't start animation: too many items on map: %1").arg(q));
        return;
    }

    m_animationTimer.start(32);
//    foreach(SimpleAnimator * bgoA, animates_BGO)
//    {
//        bgoA->start();
//    }
//    foreach(SimpleAnimator * blockA, animates_Blocks)
//    {
//        blockA->start();
//    }
    foreach(AdvNpcAnimator * npcA, m_animatorsNPC)
    {
        npcA->start();
    }
    m_subWindow->setAutoUpdateTimer(31);
}

void LvlScene::stopAnimation()
{
    int i=0;
//    int size=animates_BGO.size();
//    for(i=0;i<size; i++)
//    {
//        animates_BGO[i]->stop();
//    }
//    size=animates_Blocks.size();
//    for(i=0;i<size; i++)
//    {
//        animates_Blocks[i]->stop();
//    }
    m_animationTimer.stop();
    int size=m_animatorsNPC.size();
    for(i=0;i<size; i++)
    {
        m_animatorsNPC[i]->stop();
    }
    m_subWindow->stopAutoUpdateTimer();
    update();
}
