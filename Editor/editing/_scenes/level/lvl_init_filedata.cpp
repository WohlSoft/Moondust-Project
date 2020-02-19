/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <editing/edit_level/level_edit.h>

#include "lvl_scene.h"
#include "items/item_npc.h"

// //////////////////////////Apply used sections///////////////////////////////////////
void LvlScene::makeSectionBG(QProgressDialog &progress)
//void LvlScene::makeSectionBG(int x, int y, int w, int h)
{
    int i, total=0;
    LogDebug(QString("Applay Backgrounds"));

    //Load Backgrounds
    for(i=0; i<m_data->sections.size(); i++)
    {
        setSectionBG(m_data->sections[i]);

        total++;

        if(progress.wasCanceled())
            /*progress.setValue(progress.value()+1);
        else*/ return;
    }

}


// ///////////////////SET Block Objects/////////////////////////////////////////////
void LvlScene::setBlocks(QProgressDialog &progress)
{
    int i=0;

    //Applay images to objects
    for(i=0; i<m_data->blocks.size(); i++)
    {
        //Add block to scene
        placeBlock(m_data->blocks[i]);

        if(progress.wasCanceled())
            //progress.setValue(progress.value()+1);
        /*else*/ return;
    }
}


// ///////////////////SET BackGround Objects/////////////////////////////////////////////
void LvlScene::setBGO(QProgressDialog &progress)
{
    int i=0;

    //sortBGOArray(FileData.bgo); //Sort BGOs

    //Applay images to objects
    for(i=0; i<m_data->bgo.size(); i++)
    {
        //add BGO to scene
        placeBGO(m_data->bgo[i]);

        if(progress.wasCanceled())
            //progress.setValue(progress.value()+1);
        /*else*/ return;
    }

}


// ///////////////////////SET NonPlayble Characters and Items/////////////////////////////////
void LvlScene::setNPC(QProgressDialog &progress)
{
    int i=0;
    //QGraphicsItem *	box;

    for(i=0; i<m_data->npc.size(); i++)
    {
        LevelNPC &npc = m_data->npc[i];
        //add NPC to scene
        ItemNPC *n = placeNPC(npc);
        //FIXME: TEMPORARY, Remove this after removing of real-time PGE-FL <-> Scene synchronization
        Q_ASSERT(n);
        npc.contents        = n->m_data.contents;
        npc.special_data    = n->m_data.special_data;
        //------------------------------------------------------------------------------------------

        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        if(progress.wasCanceled())
            return;
    }

}

// ///////////////////////SET Waters/////////////////////////////////
void LvlScene::setWaters(QProgressDialog &progress)
{
    int i=0;

    for(i=0; i<m_data->physez.size(); i++)
    {
        //add Water to scene
        placeEnvironmentZone(m_data->physez[i]);

        if(progress.wasCanceled())
            //progress.setValue(progress.value()+1);
        /*else*/ return;
    }

}



// ///////////////////////SET Doors/////////////////////////////////
void LvlScene::setDoors(QProgressDialog &progress)
{
    int i=0;

    for(i=0; i<m_data->doors.size(); i++)
    {
        //add Doors points to scene
        placeDoor(m_data->doors[i]);

        if(progress.wasCanceled())
            //progress.setValue(progress.value()+1);
        /*else*/ return;
    }
}

// ////////////////////////SET Player Points/////////////////////////////////
void LvlScene::setPlayerPoints()
{
    int i=0;

    for(i=0; i<m_data->players.size(); i++)
    {
       placePlayerPoint(m_data->players[i], true);
    }
}

