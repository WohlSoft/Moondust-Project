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
#include <PGE_File_Formats/file_formats.h>

#include "lvl_history_manager.h"

//Copy selected items into clipboard
LevelData LvlScene::copy(bool cut)
{

    //Get Selected Items
    QList<QGraphicsItem *> selectedList = selectedItems();

    LevelData copyData;
    FileFormats::CreateLevelData(copyData);
    copyData.layers.clear();
    copyData.events.clear();

    if(!selectedList.isEmpty())
    {
        for(QList<QGraphicsItem *>::iterator it = selectedList.begin(); it != selectedList.end(); it++)
        {
            QString ObjType = (*it)->data(ITEM_TYPE).toString();

            if(ObjType == "Block")
            {
                ItemBlock *sourceBlock = (ItemBlock *)(*it);
                copyData.blocks.push_back(sourceBlock->m_data);
                if(cut)
                {
                    sourceBlock->removeFromArray();
                    delete(*it);
                }
            }
            else if(ObjType == "BGO")
            {
                ItemBGO *sourceBGO = (ItemBGO *)(*it);
                copyData.bgo.push_back(sourceBGO->m_data);
                if(cut)
                {
                    sourceBGO->removeFromArray();
                    delete(*it);
                }
            }
            else if(ObjType == "NPC")
            {
                ItemNPC *sourceNPC = (ItemNPC *)(*it);
                copyData.npc.push_back(sourceNPC->m_data);
                if(cut)
                {
                    sourceNPC->removeFromArray();
                    delete(*it);
                }
            }
            else if(ObjType == "Water")
            {
                ItemPhysEnv *sourceWater = (ItemPhysEnv *)(*it);
                copyData.physez.push_back(sourceWater->m_data);
                if(cut)
                {
                    sourceWater->removeFromArray();
                    delete(*it);
                }
            }

        }//fetch selected items

        if(cut)
        {
            m_data->meta.modified = true;
            m_history->addRemove(copyData);
            Debugger_updateItemList();
        }
    }

    FileFormats::arrayIdLevelSortBGOs(copyData);

    return copyData;
}

void LvlScene::paste(LevelData &BufferIn, QPoint pos)
{
    LevelData newData;
    long baseX, baseY;

    //set first base
    if(!BufferIn.blocks.isEmpty())
    {
        baseX = BufferIn.blocks[0].x;
        baseY = BufferIn.blocks[0].y;
    }
    else if(!BufferIn.bgo.isEmpty())
    {
        baseX = BufferIn.bgo[0].x;
        baseY = BufferIn.bgo[0].y;
    }
    else if(!BufferIn.npc.isEmpty())
    {
        baseX = BufferIn.npc[0].x;
        baseY = BufferIn.npc[0].y;
    }
    else if(!BufferIn.physez.isEmpty())
    {
        baseX = BufferIn.physez[0].x;
        baseY = BufferIn.physez[0].y;
    }
    else
    {
        //nothing to paste
        return;
    }

    for(LevelBlock &block : BufferIn.blocks)
    {
        if(block.x < baseX)
            baseX = block.x;
        if(block.y < baseY)
            baseY = block.y;
    }
    for(LevelBGO &bgo : BufferIn.bgo)
    {
        if(bgo.x < baseX)
            baseX = bgo.x;
        if(bgo.y < baseY)
            baseY = bgo.y;
    }
    for(LevelNPC &npc : BufferIn.npc)
    {
        if(npc.x < baseX)
            baseX = npc.x;
        if(npc.y < baseY)
            baseY = npc.y;
    }
    for(LevelPhysEnv &water : BufferIn.physez)
    {
        if(water.x < baseX)
            baseX = water.x;
        if(water.y < baseY)
            baseY = water.y;
    }

    for(LevelBlock &block : BufferIn.blocks)
    {
        //Gen Copy of Block
        LevelBlock dumpBlock = block;
        dumpBlock.x = (long)pos.x() + block.x - baseX;
        dumpBlock.y = (long)pos.y() + block.y - baseY;
        m_data->blocks_array_id++;
        dumpBlock.meta.array_id = m_data->blocks_array_id;

        placeBlock(dumpBlock);

        m_data->blocks.push_back(dumpBlock);
        newData.blocks.push_back(dumpBlock);
    }

    for(LevelBGO &bgo : BufferIn.bgo)
    {
        //Gen Copy of BGO
        LevelBGO dumpBGO = bgo;
        dumpBGO.x = (long)pos.x() + bgo.x - baseX;
        dumpBGO.y = (long)pos.y() + bgo.y - baseY;
        m_data->bgo_array_id++;
        dumpBGO.meta.array_id = m_data->bgo_array_id;

        placeBGO(dumpBGO);

        m_data->bgo.push_back(dumpBGO);
        newData.bgo.push_back(dumpBGO);
    }

    for(LevelNPC &npc : BufferIn.npc)
    {
        //Gen Copy of NPC
        LevelNPC dumpNPC = npc;
        dumpNPC.x = (long)pos.x() + npc.x - baseX;
        dumpNPC.y = (long)pos.y() + npc.y - baseY;
        m_data->npc_array_id++;
        dumpNPC.meta.array_id = m_data->npc_array_id;

        placeNPC(dumpNPC);

        m_data->npc.push_back(dumpNPC);
        newData.npc.push_back(dumpNPC);
    }

    for(LevelPhysEnv &water : BufferIn.physez)
    {
        //Gen Copy of Water
        LevelPhysEnv dumpWater = water;
        dumpWater.x = (long)pos.x() + water.x - baseX;
        dumpWater.y = (long)pos.y() + water.y - baseY;
        m_data->physenv_array_id++;
        dumpWater.meta.array_id = m_data->physenv_array_id;

        placeEnvironmentZone(dumpWater);

        m_data->physez.push_back(dumpWater);
        newData.physez.push_back(dumpWater);
    }

    auto s = selectedItems();
    applyGroupGrid(s, true);

    QString objType;
    newData.blocks.clear();
    newData.bgo.clear();
    newData.npc.clear();
    newData.physez.clear();

    // Get an actual set of items after grid aligning
    for(QGraphicsItem *it : s)
    {
        if(!it) continue;
        objType = it->data(ITEM_TYPE).toString();
        if(objType == "NPC")
        {
            ItemNPC *item = dynamic_cast<ItemNPC *>(it);
            newData.npc.push_back(item->m_data);
        }
        else if(objType == "Block")
        {
            ItemBlock *item = dynamic_cast<ItemBlock *>(it);
            newData.blocks.push_back(item->m_data);
        }
        else if(objType == "BGO")
        {
            ItemBGO *item = dynamic_cast<ItemBGO *>(it);
            newData.bgo.push_back(item->m_data);
        }
        else if(objType == "Water")
        {
            ItemPhysEnv *item = dynamic_cast<ItemPhysEnv *>(it);
            newData.physez.push_back(item->m_data);
        }
    }

    m_data->meta.modified = true;
    m_history->addPlace(newData);
}
