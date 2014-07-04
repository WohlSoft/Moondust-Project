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

#include "item_block.h"
#include "item_bgo.h"
#include "item_npc.h"
#include "item_water.h"
#include "item_door.h"
#include "../main_window/global_settings.h"

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
//            ((ItemBlock *)tmp)->AnimationStart();
//        }
//        else
//        if(((*it)->data(0)=="BGO")&&((*it)->data(4)=="animated"))
//        {
//            tmp = (*it);
//            ((ItemBGO *)tmp)->AnimationStart();
//        }
//        else
        if(((*it)->data(0)=="NPC")&&((*it)->data(4)=="animated"))
        {
            tmp = (*it);
            ((ItemNPC *)tmp)->AnimationStart();
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
//            ((ItemBlock *)tmp)->AnimationStop();
//        }
//        else
//        if(((*it)->data(0)=="BGO")&&((*it)->data(4)=="animated"))
//        {
//            tmp = (*it);
//            ((ItemBGO *)tmp)->AnimationStop();
//        }
//        else
        if(((*it)->data(0)=="NPC")&&((*it)->data(4)=="animated"))
        {
            tmp = (*it);
            ((ItemNPC *)tmp)->AnimationStop();
        }
    }

    update();
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
                if( ((ItemBlock *)tmp)->blockData.layer == layer.name)
                {
                    ((ItemBlock *)tmp)->setVisible( !layer.hidden ); break;
                }
            }
        }
        else
        if((*it)->data(0)=="BGO")
        {
            tmp = (*it);
            foreach(LevelLayers layer, LvlData->layers)
            {
                if( ((ItemBGO *)tmp)->bgoData.layer == layer.name)
                {
                    ((ItemBGO *)tmp)->setVisible( !layer.hidden ); break;
                }
            }
        }
        else
        if((*it)->data(0)=="NPC")
        {
            tmp = (*it);
            foreach(LevelLayers layer, LvlData->layers)
            {
                if( ((ItemNPC *)tmp)->npcData.layer == layer.name)
                {
                    ((ItemNPC *)tmp)->setVisible( !layer.hidden ); break;
                }
            }
        }
        else
        if((*it)->data(0)=="Water")
        {
            tmp = (*it);
            foreach(LevelLayers layer, LvlData->layers)
            {
                if( ((ItemWater *)tmp)->waterData.layer == layer.name)
                {
                    ((ItemWater *)tmp)->setVisible( !layer.hidden ); break;
                }
            }
        }
        else
        if(((*it)->data(0)=="Door_enter")||((*it)->data(0)=="Door_exit"))
        {
            tmp = (*it);
            foreach(LevelLayers layer, LvlData->layers)
            {
                if( ((ItemDoor *)tmp)->doorData.layer == layer.name)
                {
                    ((ItemDoor *)tmp)->setVisible( !layer.hidden ); break;
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
                (*it)->setFlag(QGraphicsItem::ItemIsSelectable, (!( (lock) || ((ItemBlock *)(*it))->isLocked ) ) );
                (*it)->setFlag(QGraphicsItem::ItemIsMovable, (!( (lock) || ((ItemBlock *)(*it))->isLocked ) ) );
            }
            break;
        case 2://BGO
            if((*it)->data(0).toString()=="BGO")
            {
                (*it)->setFlag(QGraphicsItem::ItemIsSelectable, (!( (lock) || ((ItemBGO *)(*it))->isLocked ) ));
                (*it)->setFlag(QGraphicsItem::ItemIsMovable, (!( (lock) || ((ItemBGO *)(*it))->isLocked ) ));
            }
            break;
        case 3://NPC
            if((*it)->data(0).toString()=="NPC")
            {
                (*it)->setFlag(QGraphicsItem::ItemIsSelectable, (!( (lock) || ((ItemNPC *)(*it))->isLocked ) ) );
                (*it)->setFlag(QGraphicsItem::ItemIsMovable, (!( (lock) || ((ItemNPC *)(*it))->isLocked ) ) );
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

