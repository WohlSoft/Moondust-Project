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

#include <common_features/mainwinconnect.h>
#include <common_features/themes.h>
#include <main_window/global_settings.h>
#include <editing/edit_level/level_edit.h>

#include "lvl_scene.h"
#include "items/item_block.h"
#include "items/item_bgo.h"
#include "items/item_npc.h"
#include "items/item_water.h"
#include "items/item_door.h"
#include "newlayerbox.h"

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
        switchMode("Placing");
        break;

    case MODE_DrawSquare:
        switchMode("Square");
        break;

    case MODE_Line:
        switchMode("Line");
        break;

    case MODE_Resizing:
        switchMode("Resize");
        break;

    case MODE_PasteFromClip:
        switchMode("Select");
        clearSelection();
        disableMoveItems=true;
        _viewPort->setInteractive(true);
        _viewPort->setCursor(Themes::Cursor(Themes::cursor_pasting));
        _viewPort->setDragMode(QGraphicsView::NoDrag);
        break;

    case MODE_Erasing:
        switchMode("Erase");
        break;

    case MODE_SelectingOnly:
        switchMode("Select");
        disableMoveItems=true;
        break;

    case MODE_HandScroll:
        switchMode("HandScroll");
        break;


    case MODE_Fill:
        switchMode("Fill");
        break;

    case MODE_Selecting:
    default:
        switchMode("Select");
        break;
    }
    EditingMode = EdtMode;

}

void LvlScene::switchMode(QString title)
{
    qDebug() << "Switching mode " << title;
    for(int i=0; i<EditModes.size(); i++)
    {
        if(EditModes[i]->name()==title)
        {
            CurrentMode = EditModes[i];
            CurrentMode->set();
            qDebug() << "mode " << title << "switched!";
            break;
        }
    }
}


void LvlScene::hideWarpsAndDoors(bool visible)
{
    QMap<QString, LevelLayers> localLayers;
    for(int i = 0; i < LvlData->layers.size(); ++i){
        localLayers[LvlData->layers[i].name] = LvlData->layers[i];
    }

    foreach (QGraphicsItem* i, items()) {
        if(i->data(ITEM_TYPE).toString()=="Water"){
            i->setVisible(!localLayers[((ItemWater*)i)->waterData.layer].hidden && visible);
        }else if(i->data(ITEM_TYPE).toString()=="Door_exit" || i->data(ITEM_TYPE).toString()=="Door_enter"){
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
        if((*it)->data(ITEM_TYPE)=="Block")
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
        if((*it)->data(ITEM_TYPE)=="BGO")
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
        if((*it)->data(ITEM_TYPE)=="NPC")
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
        if((*it)->data(ITEM_TYPE)=="Water")
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
        if(((*it)->data(ITEM_TYPE)=="Door_enter")||((*it)->data(ITEM_TYPE)=="Door_exit"))
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
            if((*it)->data(ITEM_TYPE).toString()=="Block")
            {
                (*it)->setFlag(QGraphicsItem::ItemIsSelectable, (!( (lock) || dynamic_cast<ItemBlock *>(*it)->isLocked ) ) );
                (*it)->setFlag(QGraphicsItem::ItemIsMovable, (!( (lock) || dynamic_cast<ItemBlock *>(*it)->isLocked ) ) );
            }
            break;
        case 2://BGO
            if((*it)->data(ITEM_TYPE).toString()=="BGO")
            {
                (*it)->setFlag(QGraphicsItem::ItemIsSelectable, (!( (lock) || dynamic_cast<ItemBGO *>(*it)->isLocked ) ));
                (*it)->setFlag(QGraphicsItem::ItemIsMovable, (!( (lock) || dynamic_cast<ItemBGO *>(*it)->isLocked ) ));
            }
            break;
        case 3://NPC
            if((*it)->data(ITEM_TYPE).toString()=="NPC")
            {
                (*it)->setFlag(QGraphicsItem::ItemIsSelectable, (!( (lock) || dynamic_cast<ItemNPC *>(*it)->isLocked ) ) );
                (*it)->setFlag(QGraphicsItem::ItemIsMovable, (!( (lock) || dynamic_cast<ItemNPC *>(*it)->isLocked ) ) );
            }
            break;
        case 4://Water
            if((*it)->data(ITEM_TYPE).toString()=="Water")
            {
                (*it)->setFlag(QGraphicsItem::ItemIsSelectable, (!lock));
                (*it)->setFlag(QGraphicsItem::ItemIsMovable, (!lock));
            }
            break;
        case 5://Doors
            if(((*it)->data(ITEM_TYPE).toString()=="Door_enter")||((*it)->data(ITEM_TYPE).toString()=="Door_exit"))
            {
                (*it)->setFlag(QGraphicsItem::ItemIsSelectable, (!lock));
                (*it)->setFlag(QGraphicsItem::ItemIsMovable, (!lock));
            }
            break;
        default: break;
        }
    }

}


void LvlScene::setLayerToSelected()
{
    QString lName;
    ToNewLayerBox * layerBox = new ToNewLayerBox(LvlData);
    layerBox->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    layerBox->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, layerBox->size(), qApp->desktop()->availableGeometry()));
    if(layerBox->exec()==QDialog::Accepted)
    {
        lName = layerBox->lName;

        //Store new layer into array
        LevelLayers nLayer;
        nLayer.name = lName;
        nLayer.hidden = layerBox->lHidden;
        LvlData->layers_array_id++;
        nLayer.array_id = LvlData->layers_array_id;
        LvlData->layers.push_back(nLayer);
        //scene->SyncLayerList=true; //Refresh layer list
        MainWinConnect::pMainWin->setLayerToolsLocked(true);
        MainWinConnect::pMainWin->setLayersBox();
        MainWinConnect::pMainWin->setLayerToolsLocked(false);
        MainWinConnect::pMainWin->setLayerLists();
        MainWinConnect::pMainWin->setEventData();
        setLayerToSelected(lName, true);
    }
    delete layerBox;

}

void LvlScene::setLayerToSelected(QString lName, bool isNew)
{
    LevelData modData;
    foreach(LevelLayers lr, LvlData->layers)
    { //Find layer's settings
        if(lr.name==lName)
        {
            foreach(QGraphicsItem * SelItem, selectedItems() )
            {
                if(SelItem->data(ITEM_TYPE).toString()=="Block")
                {
                    modData.blocks.push_back(dynamic_cast<ItemBlock *>(SelItem)->blockData);
                    dynamic_cast<ItemBlock *>(SelItem)->blockData.layer = lr.name;
                    dynamic_cast<ItemBlock *>(SelItem)->setVisible(!lr.hidden);
                    dynamic_cast<ItemBlock *>(SelItem)->arrayApply();
                }
                else
                if(SelItem->data(ITEM_TYPE).toString()=="BGO")
                {
                    modData.bgo.push_back(dynamic_cast<ItemBGO *>(SelItem)->bgoData);
                    dynamic_cast<ItemBGO *>(SelItem)->bgoData.layer = lr.name;
                    dynamic_cast<ItemBGO *>(SelItem)->setVisible(!lr.hidden);
                    dynamic_cast<ItemBGO *>(SelItem)->arrayApply();
                }
                else
                if(SelItem->data(ITEM_TYPE).toString()=="NPC")
                {
                    modData.npc.push_back(dynamic_cast<ItemNPC *>(SelItem)->npcData);
                    dynamic_cast<ItemNPC *>(SelItem)->npcData.layer = lr.name;
                    dynamic_cast<ItemNPC *>(SelItem)->setVisible(!lr.hidden);
                    dynamic_cast<ItemNPC *>(SelItem)->arrayApply();
                }
                else
                if(SelItem->data(ITEM_TYPE).toString()=="Water")
                {
                    modData.physez.push_back(dynamic_cast<ItemWater *>(SelItem)->waterData);
                    dynamic_cast<ItemWater *>(SelItem)->waterData.layer = lr.name;
                    dynamic_cast<ItemWater *>(SelItem)->setVisible(!lr.hidden);
                    dynamic_cast<ItemWater *>(SelItem)->arrayApply();
                }
                else
                if((SelItem->data(ITEM_TYPE).toString()=="Door_exit")  ||
                        (SelItem->data(ITEM_TYPE).toString()=="Door_enter"))
                {
                    if(SelItem->data(ITEM_TYPE).toString()=="Door_exit"){
                        LevelDoors tDoor = dynamic_cast<ItemDoor *>(SelItem)->doorData;
                        tDoor.isSetOut = true;
                        tDoor.isSetIn = false;
                        modData.doors.push_back(tDoor);
                    }
                    else
                    if(SelItem->data(ITEM_TYPE).toString()=="Door_enter"){
                        LevelDoors tDoor = dynamic_cast<ItemDoor *>(SelItem)->doorData;
                        tDoor.isSetOut = false;
                        tDoor.isSetIn = true;
                        modData.doors.push_back(tDoor);
                    }
                    dynamic_cast<ItemDoor *>(SelItem)->doorData.layer = lr.name;
                    dynamic_cast<ItemDoor *>(SelItem)->setVisible(!lr.hidden);
                    dynamic_cast<ItemDoor *>(SelItem)->arrayApply();
                }
            }
            if(isNew)
            {
                addChangedNewLayerHistory(modData, lr);
            }
            break;
        }
    }//Find layer's settings

    if(!isNew)
    {
        addChangedLayerHistory(modData, lName);
    }
}
