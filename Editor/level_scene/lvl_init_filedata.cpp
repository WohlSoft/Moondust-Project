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

#include "lvlscene.h"
#include "../edit_level/leveledit.h"


// //////////////////////////Apply used sections///////////////////////////////////////
void LvlScene::makeSectionBG(QProgressDialog &progress)
//void LvlScene::makeSectionBG(int x, int y, int w, int h)
{
    int i, total=0;
    WriteToLog(QtDebugMsg, QString("Applay Backgrounds"));

    //Load Backgrounds
    for(i=0; i<LvlData->sections.size(); i++)
    {
        setSectionBG(LvlData->sections[i]);

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
    for(i=0; i<LvlData->blocks.size(); i++)
    {
        //Add block to scene
        placeBlock(LvlData->blocks[i]);

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
    for(i=0; i<LvlData->bgo.size(); i++)
    {
        //add BGO to scene
        placeBGO(LvlData->bgo[i]);

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

    for(i=0; i<LvlData->npc.size(); i++)
    {
        //add NPC to scene
        placeNPC(LvlData->npc[i]);

        if(progress.wasCanceled())
            //progress.setValue(progress.value()+1);
        /*else*/ return;
    }

}

// ///////////////////////SET Waters/////////////////////////////////
void LvlScene::setWaters(QProgressDialog &progress)
{
    int i=0;

    for(i=0; i<LvlData->water.size(); i++)
    {
        //add Water to scene
        placeWater(LvlData->water[i]);

        if(progress.wasCanceled())
            //progress.setValue(progress.value()+1);
        /*else*/ return;
    }

}



// ///////////////////////SET Doors/////////////////////////////////
void LvlScene::setDoors(QProgressDialog &progress)
{
    int i=0;

    for(i=0; i<LvlData->doors.size(); i++)
    {
        //add Doors points to scene
        placeDoor(LvlData->doors[i]);

        if(progress.wasCanceled())
            //progress.setValue(progress.value()+1);
        /*else*/ return;
    }
}

// ////////////////////////SET Player Points/////////////////////////////////
void LvlScene::setPlayerPoints()
{
    int i=0;

    for(i=0; i<LvlData->players.size(); i++)
    {
       placePlayerPoint(LvlData->players[i], true);
    }
}

