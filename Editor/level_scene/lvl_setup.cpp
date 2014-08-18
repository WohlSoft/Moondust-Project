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
#include "../edit_level/level_edit.h"

#include "item_block.h"
#include "item_bgo.h"
#include "item_npc.h"
#include "item_water.h"
#include "item_door.h"
#include "../main_window/global_settings.h"

#include "../common_features/mainwinconnect.h"


void LvlScene::SwitchEditingMode(int EdtMode)
{
    //int EditingMode; // 0 - selecting,  1 - erasing, 2 - placeNewObject
                     // 3 - drawing water/sand zone, 4 - placing from Buffer
    //bool EraserEnabled;
    //bool PasteFromBuffer;

    //bool DrawMode; //Placing/drawing on map, disable selecting and dragging items

    //bool disableMoveItems;

    //bool contextMenuOpened;
    EraserEnabled=false;
    PasteFromBuffer=false;
    DrawMode=false;
    disableMoveItems=false;

    switch(EdtMode)
    {
    case MODE_PlacingNew:
        DrawMode=true;
        resetResizers();

        break;
    case MODE_DrawSquare:
        resetResizers();
        DrawMode=true;
        break;

    case MODE_Line:
        resetResizers();
        DrawMode=true;
        break;

    case MODE_Resizing:
        resetCursor();
        DrawMode=true;
        disableMoveItems=true;
        break;

    case MODE_PasteFromClip:
        resetCursor();
        resetResizers();
        disableMoveItems=true;
        break;

    case MODE_Erasing:
        resetCursor();
        resetResizers();
        break;

    case MODE_SelectingOnly:
        resetCursor();
        resetResizers();
        disableMoveItems=true;
        break;

    case MODE_Selecting:
    default:
        resetCursor();
        resetResizers();
        break;
    }
    EditingMode = EdtMode;

}



////////////////////////////////////Animator////////////////////////////////
void LvlScene::startBlockAnimation()
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

    QList<QGraphicsItem*> ItemList = items();
    QGraphicsItem *tmp;
    for (QList<QGraphicsItem*>::iterator it = ItemList.begin(); it != ItemList.end(); it++)
    {
//        if(((*it)->data(0)=="Block")&&((*it)->data(4)=="animated"))
//        {
//            tmp = (*it);
//            dynamic_cast<ItemBlock *>(tmp)->AnimationStart();
//        }
//        else
//        if(((*it)->data(0)=="BGO")&&((*it)->data(4)=="animated"))
//        {
//            tmp = (*it);
//            dynamic_cast<ItemBGO *>(tmp)->AnimationStart();
//        }
//        else
        if(((*it)->data(0)=="NPC")&&((*it)->data(4)=="animated"))
        {
            tmp = (*it);
            dynamic_cast<ItemNPC *>(tmp)->AnimationStart();
        }
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

    QList<QGraphicsItem*> ItemList = items();
    QGraphicsItem *tmp;
    for (QList<QGraphicsItem*>::iterator it = ItemList.begin(); it != ItemList.end(); it++)
    {
//        if(((*it)->data(0)=="Block")&&((*it)->data(4)=="animated"))
//        {
//            tmp = (*it);
//            dynamic_cast<ItemBlock *>(tmp)->AnimationStop();
//        }
//        else
//        if(((*it)->data(0)=="BGO")&&((*it)->data(4)=="animated"))
//        {
//            tmp = (*it);
//            dynamic_cast<ItemBGO *>(tmp)->AnimationStop();
//        }
//        else
        if(((*it)->data(0)=="NPC")&&((*it)->data(4)=="animated"))
        {
            tmp = (*it);
            dynamic_cast<ItemNPC *>(tmp)->AnimationStop();
        }
    }

    update();
}

void LvlScene::hideWarpsAndDoors(bool visible)
{
    QMap<QString, LevelLayers> localLayers;
    for(int i = 0; i < LvlData->layers.size(); ++i){
        localLayers[LvlData->layers[i].name] = LvlData->layers[i];
    }

    foreach (QGraphicsItem* i, items()) {
        if(i->data(0).toString()=="Water"){
            i->setVisible(!localLayers[((ItemWater*)i)->waterData.layer].hidden && visible);
        }else if(i->data(0).toString()=="Door_exit" || i->data(0).toString()=="Door_enter"){
            i->setVisible(!localLayers[((ItemDoor*)i)->doorData.layer].hidden && visible);
        }
    }
}


void LvlScene::setTiledBackground(bool forceTiled)
{
    ChangeSectionBG(LvlData->sections[LvlData->CurSection].background, -1, forceTiled);
}

void LvlScene::applyLayersVisible()
{
    QList<QGraphicsItem*> ItemList = items();
    QGraphicsItem *tmp;
    for (QList<QGraphicsItem*>::iterator it = ItemList.begin(); it != ItemList.end(); it++)
    {
        if((*it)->data(0)=="Block")
        {
            tmp = (*it);
            foreach(LevelLayers layer, LvlData->layers)
            {
                if( dynamic_cast<ItemBlock *>(tmp)->blockData.layer == layer.name)
                {
                    dynamic_cast<ItemBlock *>(tmp)->setVisible( !layer.hidden ); break;
                }
            }
        }
        else
        if((*it)->data(0)=="BGO")
        {
            tmp = (*it);
            foreach(LevelLayers layer, LvlData->layers)
            {
                if( dynamic_cast<ItemBGO *>(tmp)->bgoData.layer == layer.name)
                {
                    dynamic_cast<ItemBGO *>(tmp)->setVisible( !layer.hidden ); break;
                }
            }
        }
        else
        if((*it)->data(0)=="NPC")
        {
            tmp = (*it);
            foreach(LevelLayers layer, LvlData->layers)
            {
                if( dynamic_cast<ItemNPC *>(tmp)->npcData.layer == layer.name)
                {
                    dynamic_cast<ItemNPC *>(tmp)->setVisible( !layer.hidden ); break;
                }
            }
        }
        else
        if((*it)->data(0)=="Water")
        {
            tmp = (*it);
            foreach(LevelLayers layer, LvlData->layers)
            {
                if( dynamic_cast<ItemWater *>(tmp)->waterData.layer == layer.name)
                {
                    dynamic_cast<ItemWater *>(tmp)->setVisible( !layer.hidden ); break;
                }
            }
        }
        else
        if(((*it)->data(0)=="Door_enter")||((*it)->data(0)=="Door_exit"))
        {
            tmp = (*it);
            foreach(LevelLayers layer, LvlData->layers)
            {
                if( dynamic_cast<ItemDoor *>(tmp)->doorData.layer == layer.name)
                {
                    dynamic_cast<ItemDoor *>(tmp)->setVisible( !layer.hidden ); break;
                }
            }
        }
    }
}


/////////////////////////////////////////////Locks////////////////////////////////
void LvlScene::setLocked(int type, bool lock)
{
    QList<QGraphicsItem*> ItemList = items();
    // setLock
    switch(type)
    {
    case 1://Block
        lock_block = lock;
        break;
    case 2://BGO
        lock_bgo = lock;
        break;
    case 3://NPC
        lock_npc = lock;
        break;
    case 4://Water
        lock_water = lock;
        break;
    case 5://Doors
        lock_door = lock;
        break;
    default: break;
    }

    for (QList<QGraphicsItem*>::iterator it = ItemList.begin(); it != ItemList.end(); it++)
    {
        switch(type)
        {
        case 1://Block
            if((*it)->data(0).toString()=="Block")
            {
                (*it)->setFlag(QGraphicsItem::ItemIsSelectable, (!( (lock) || dynamic_cast<ItemBlock *>(*it)->isLocked ) ) );
                (*it)->setFlag(QGraphicsItem::ItemIsMovable, (!( (lock) || dynamic_cast<ItemBlock *>(*it)->isLocked ) ) );
            }
            break;
        case 2://BGO
            if((*it)->data(0).toString()=="BGO")
            {
                (*it)->setFlag(QGraphicsItem::ItemIsSelectable, (!( (lock) || dynamic_cast<ItemBGO *>(*it)->isLocked ) ));
                (*it)->setFlag(QGraphicsItem::ItemIsMovable, (!( (lock) || dynamic_cast<ItemBGO *>(*it)->isLocked ) ));
            }
            break;
        case 3://NPC
            if((*it)->data(0).toString()=="NPC")
            {
                (*it)->setFlag(QGraphicsItem::ItemIsSelectable, (!( (lock) || dynamic_cast<ItemNPC *>(*it)->isLocked ) ) );
                (*it)->setFlag(QGraphicsItem::ItemIsMovable, (!( (lock) || dynamic_cast<ItemNPC *>(*it)->isLocked ) ) );
            }
            break;
        case 4://Water
            if((*it)->data(0).toString()=="Water")
            {
                (*it)->setFlag(QGraphicsItem::ItemIsSelectable, (!lock));
                (*it)->setFlag(QGraphicsItem::ItemIsMovable, (!lock));
            }
            break;
        case 5://Doors
            if(((*it)->data(0).toString()=="Door_enter")||((*it)->data(0).toString()=="Door_exit"))
            {
                (*it)->setFlag(QGraphicsItem::ItemIsSelectable, (!lock));
                (*it)->setFlag(QGraphicsItem::ItemIsMovable, (!lock));
            }
            break;
        default: break;
        }
    }

}

