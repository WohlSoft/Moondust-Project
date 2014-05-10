/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include "lvlscene.h"
#include "../leveledit.h"

#include "item_block.h"
#include "item_bgo.h"


//Copy selected items into clipboard
LevelData LvlScene::copy(bool cut)
{

    //Get Selected Items
    QList<QGraphicsItem*> selectedList = selectedItems();

    LevelData copyData;

    if (!selectedList.isEmpty())
    {
        for (QList<QGraphicsItem*>::iterator it = selectedList.begin(); it != selectedList.end(); it++)
        {
            QString ObjType = (*it)->data(0).toString();

            if( ObjType == "Block")
            {
                ItemBlock* sourceBlock = (ItemBlock *)(*it);
                copyData.blocks.push_back(sourceBlock->blockData);
                if(cut){
                    sourceBlock->removeFromArray();
                    removeItem(*it);
                }
            }
            else
            if( ObjType == "BGO")
            {
                ItemBGO* sourceBGO = (ItemBGO *)(*it);
                copyData.bgo.push_back(sourceBGO->bgoData);
                if(cut){
                    sourceBGO->removeFromArray();
                    removeItem(*it);
                }
            }
            else
            if( ObjType == "NPC")
            {
                foreach (LevelNPC findInArr, LvlData->npc)
                {
                    if(findInArr.array_id==(unsigned)(*it)->data(2).toInt())
                    {
                        copyData.npc.push_back(findInArr);
                        if(cut){
                            //remove, will be later implemented as function in the future NPC class
                            for(int i=0; i<LvlData->npc.size(); i++)
                            {
                                if(LvlData->npc[i].array_id == findInArr.array_id)
                                {
                                    LvlData->npc.remove(i); break;
                                }
                            }
                            removeItem(*it);
                        }
                        break;
                    }
                }
            }

        }//fetch selected items

        if(cut)
        {
            LvlData->modified = true;
            addRemoveHistory(copyData);
        }
    }

    return copyData;
}

void LvlScene::paste(LevelData BufferIn, QPoint pos)
{
    long baseX, baseY;
    //set first base
    if(!BufferIn.blocks.isEmpty()){
        baseX = BufferIn.blocks[0].x;
        baseY = BufferIn.blocks[0].y;
    }else if(!BufferIn.bgo.isEmpty()){
        baseX = BufferIn.bgo[0].x;
        baseY = BufferIn.bgo[0].y;
    }else if(!BufferIn.npc.isEmpty()){
        baseX = BufferIn.npc[0].x;
        baseY = BufferIn.npc[0].y;
    }else{
        //nothing to paste
        return;
    }

    foreach (LevelBlock block, BufferIn.blocks) {
        if(block.x<baseX){
            baseX = block.x;
        }
        if(block.y<baseY){
            baseY = block.y;
        }
    }
    foreach (LevelBGO bgo, BufferIn.bgo){
        if(bgo.x<baseX){
            baseX = bgo.x;
        }
        if(bgo.y<baseY){
            baseY = bgo.y;
        }
    }
    foreach (LevelNPC npc, BufferIn.npc){
        if(npc.x<baseX){
            baseX = npc.x;
        }
        if(npc.y<baseY){
            baseY = npc.y;
        }
    }

    foreach (LevelBlock block, BufferIn.blocks){
        //Gen Copy of Block
        LevelBlock dumpBlock = block;
        dumpBlock.x = (long)pos.x() + block.x - baseX;
        dumpBlock.y = (long)pos.y() + block.y - baseY;
        LvlData->blocks_array_id++;
        dumpBlock.array_id = LvlData->blocks_array_id;
        placeBlock(dumpBlock);
        LvlData->blocks.push_back(dumpBlock);
    }
    foreach (LevelBGO bgo, BufferIn.bgo){
        //Gen Copy of BGO
        LevelBGO dumpBGO = bgo;
        dumpBGO.x = (long)pos.x() + bgo.x - baseX;
        dumpBGO.y = (long)pos.y() + bgo.y - baseY;
        LvlData->bgo_array_id++;
        dumpBGO.array_id = LvlData->bgo_array_id;
        placeBGO(dumpBGO);
        LvlData->bgo.push_back(dumpBGO);
    }
    foreach (LevelNPC npc, BufferIn.npc){
        //Gen Copy of NPC
        LevelNPC dumpNPC = npc;
        dumpNPC.x = (long)pos.x() + npc.x - baseX;
        dumpNPC.y = (long)pos.y() + npc.y - baseY;
        LvlData->npc_array_id++;
        dumpNPC.array_id = LvlData->npc_array_id;
        placeNPC(dumpNPC);
        LvlData->npc.push_back(dumpNPC);
    }

    LvlData->modified = true;
    //refresh Animation control
    if(opts.animationEnabled) stopAnimation();
    if(opts.animationEnabled) startBlockAnimation();

}
