/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QDesktopWidget>

#include <mainwindow.h>
#include <common_features/themes.h>
#include <main_window/global_settings.h>
#include <editing/edit_level/level_edit.h>
#include <main_window/dock/lvl_layers_box.h>
#include <main_window/dock/lvl_events_box.h>

#include "lvl_history_manager.h"
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
    m_eraserIsEnabled=false;
    m_pastingMode=false;
    m_busyMode=false;
    m_disableMoveItems=false;

    switch(EdtMode)
    {
    case MODE_PlacingNew:
        switchMode("Placing");
        break;

    case MODE_DrawRect:
        switchMode("Square");
        break;

    case MODE_DrawCircle:
        switchMode("Circle");
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
        m_disableMoveItems=true;
        m_viewPort->setInteractive(true);
        m_viewPort->setCursor(Themes::Cursor(Themes::cursor_pasting));
        m_viewPort->setDragMode(QGraphicsView::NoDrag);
        break;

    case MODE_Erasing:
        switchMode("Erase");
        break;

    case MODE_SelectingOnly:
        switchMode("Select");
        m_disableMoveItems=true;
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
    m_editMode = EdtMode;

}

void LvlScene::switchMode(QString title)
{
    qDebug() << "Switching mode " << title;
    for(int i=0; i<m_editModes.size(); i++)
    {
        if(m_editModes[i]->name()==title)
        {
            m_editModeObj = m_editModes[i];
            m_editModeObj->set();
            qDebug() << "mode " << title << "switched!";
            break;
        }
    }
}


void LvlScene::hideWarpsAndDoors(bool visible)
{
    QMap<QString, LevelLayer> localLayers;
    for(int i = 0; i < m_data->layers.size(); ++i){
        localLayers[m_data->layers[i].name] = m_data->layers[i];
    }

    foreach (QGraphicsItem* i, items()) {
        if(i->data(ITEM_TYPE).toString()=="Water"){
            i->setVisible(!localLayers[((ItemPhysEnv*)i)->m_data.layer].hidden && visible);
        }else if(i->data(ITEM_TYPE).toString()=="Door_exit" || i->data(ITEM_TYPE).toString()=="Door_enter"){
            i->setVisible(!localLayers[((ItemDoor*)i)->m_data.layer].hidden && visible);
        }
    }
}


void LvlScene::setTiledBackground(bool forceTiled)
{
    ChangeSectionBG(m_data->sections[m_data->CurSection].background, -1, forceTiled);
}

void LvlScene::applyLayersVisible()
{
    QList<QGraphicsItem*> ItemList = items();
    for (QList<QGraphicsItem*>::iterator it = ItemList.begin(); it != ItemList.end(); it++)
    {
        QGraphicsItem* item = (*it);
        if(!item->data(ITEM_IS_CURSOR).isNull())
            continue;

        if((*it)->data(ITEM_TYPE)=="Block")
        {
            foreach(LevelLayer layer, m_data->layers)
            {
                if( dynamic_cast<ItemBlock *>(item)->m_data.layer == layer.name)
                {
                    dynamic_cast<ItemBlock *>(item)->setVisible( !layer.hidden ); break;
                }
            }
        }
        else
        if(item->data(ITEM_TYPE)=="BGO")
        {
            foreach(LevelLayer layer, m_data->layers)
            {
                if( dynamic_cast<ItemBGO *>(item)->m_data.layer == layer.name)
                {
                    dynamic_cast<ItemBGO *>(item)->setVisible( !layer.hidden ); break;
                }
            }
        }
        else
        if(item->data(ITEM_TYPE)=="NPC")
        {
            foreach(LevelLayer layer, m_data->layers)
            {
                if( dynamic_cast<ItemNPC *>(item)->m_data.layer == layer.name)
                {
                    dynamic_cast<ItemNPC *>(item)->setVisible( !layer.hidden ); break;
                }
            }
        }
        else
        if(item->data(ITEM_TYPE)=="Water")
        {
            foreach(LevelLayer layer, m_data->layers)
            {
                if( dynamic_cast<ItemPhysEnv *>(item)->m_data.layer == layer.name)
                {
                    dynamic_cast<ItemPhysEnv *>(item)->setVisible( !layer.hidden ); break;
                }
            }
        }
        else
        if( (item->data(ITEM_TYPE)=="Door_enter") || (item->data(ITEM_TYPE)=="Door_exit") )
        {
            foreach(LevelLayer layer, m_data->layers)
            {
                if( dynamic_cast<ItemDoor *>(item)->m_data.layer == layer.name)
                {
                    dynamic_cast<ItemDoor *>(item)->setVisible( !layer.hidden ); break;
                }
            }
        }
    }
}


/////////////////////////////////////////////Locks////////////////////////////////
void LvlScene::setLocked(int type, bool lock)
{
    // setLock
    switch(type)
    {
    case 1://Block
        m_lockBlock = lock;
        break;
    case 2://BGO
        m_lockBgo = lock;
        break;
    case 3://NPC
        m_lockNpc = lock;
        break;
    case 4://Water
        m_lockPhysenv = lock;
        break;
    case 5://Doors
        m_lockDoor = lock;
        break;
    default: break;
    }

    QList<QGraphicsItem*> ItemList = items();
    for (QList<QGraphicsItem*>::iterator it = ItemList.begin(); it != ItemList.end(); it++)
    {
        if((*it)==NULL) continue;
        if((*it)->data(ITEM_IS_ITEM).isNull()) continue;

        switch(type)
        {
        case 1://Block
            if((*it)->data(ITEM_TYPE).toString()=="Block")
            {
                ItemBlock *item=qgraphicsitem_cast<ItemBlock *>(*it);
                if(item) item->setLocked(lock);
            }
            break;
        case 2://BGO
            if((*it)->data(ITEM_TYPE).toString()=="BGO")
            {
                ItemBGO *i=qgraphicsitem_cast<ItemBGO *>(*it);
                if(i) i->setLocked(lock);
            }
            break;
        case 3://NPC
            if((*it)->data(ITEM_TYPE).toString()=="NPC")
            {
                ItemNPC *i=qgraphicsitem_cast<ItemNPC *>(*it);
                if(i) i->setLocked(lock);
            }
            break;
        case 4://Water
            if((*it)->data(ITEM_TYPE).toString()=="Water")
            {
                ItemPhysEnv *i=qgraphicsitem_cast<ItemPhysEnv *>(*it);
                if(i) i->setLocked(lock);
            }
            break;
        case 5://Doors
            if(((*it)->data(ITEM_TYPE).toString()=="Door_enter")||((*it)->data(ITEM_TYPE).toString()=="Door_exit"))
            {
                ItemDoor *i=qgraphicsitem_cast<ItemDoor *>(*it);
                if(i) i->setLocked(lock);
            }
            break;
        default: break;
        }
    }

}


void LvlScene::setLayerToSelected()
{
    QString lName;
    ToNewLayerBox * layerBox = new ToNewLayerBox(m_data, m_viewPort);
    layerBox->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    layerBox->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, layerBox->size(), qApp->desktop()->availableGeometry()));
    if(layerBox->exec()==QDialog::Accepted)
    {
        lName = layerBox->lName;

        //Store new layer into array
        LevelLayer nLayer;
        nLayer.name = lName;
        nLayer.hidden = layerBox->lHidden;
        m_data->layers_array_id++;
        nLayer.meta.array_id = m_data->layers_array_id;
        m_data->layers.push_back(nLayer);
        //scene->SyncLayerList=true; //Refresh layer list
        m_mw->dock_LvlLayers->setLayerToolsLocked(true);
        m_mw->dock_LvlLayers->setLayersBox();
        m_mw->dock_LvlLayers->setLayerToolsLocked(false);
        m_mw->LayerListsSync();
        m_mw->dock_LvlEvents->setEventData();
        setLayerToSelected(lName, true);
    }
    delete layerBox;

}

void LvlScene::setLayerToSelected(QString lName, bool isNew)
{
    LevelData modData;
    foreach(LevelLayer lr, m_data->layers)
    { //Find layer's settings
        if(lr.name==lName)
        {
            foreach(QGraphicsItem * SelItem, selectedItems() )
            {
                if(SelItem->data(ITEM_TYPE).toString()=="Block")
                {
                    modData.blocks.push_back(dynamic_cast<ItemBlock *>(SelItem)->m_data);
                    dynamic_cast<ItemBlock *>(SelItem)->m_data.layer = lr.name;
                    dynamic_cast<ItemBlock *>(SelItem)->setVisible(!lr.hidden);
                    dynamic_cast<ItemBlock *>(SelItem)->arrayApply();
                }
                else
                if(SelItem->data(ITEM_TYPE).toString()=="BGO")
                {
                    modData.bgo.push_back(dynamic_cast<ItemBGO *>(SelItem)->m_data);
                    dynamic_cast<ItemBGO *>(SelItem)->m_data.layer = lr.name;
                    dynamic_cast<ItemBGO *>(SelItem)->setVisible(!lr.hidden);
                    dynamic_cast<ItemBGO *>(SelItem)->arrayApply();
                }
                else
                if(SelItem->data(ITEM_TYPE).toString()=="NPC")
                {
                    modData.npc.push_back(dynamic_cast<ItemNPC *>(SelItem)->m_data);
                    dynamic_cast<ItemNPC *>(SelItem)->m_data.layer = lr.name;
                    dynamic_cast<ItemNPC *>(SelItem)->setVisible(!lr.hidden);
                    dynamic_cast<ItemNPC *>(SelItem)->arrayApply();
                }
                else
                if(SelItem->data(ITEM_TYPE).toString()=="Water")
                {
                    modData.physez.push_back(dynamic_cast<ItemPhysEnv *>(SelItem)->m_data);
                    dynamic_cast<ItemPhysEnv *>(SelItem)->m_data.layer = lr.name;
                    dynamic_cast<ItemPhysEnv *>(SelItem)->setVisible(!lr.hidden);
                    dynamic_cast<ItemPhysEnv *>(SelItem)->arrayApply();
                }
                else
                if((SelItem->data(ITEM_TYPE).toString()=="Door_exit")  ||
                        (SelItem->data(ITEM_TYPE).toString()=="Door_enter"))
                {
                    if(SelItem->data(ITEM_TYPE).toString()=="Door_exit"){
                        LevelDoor tDoor = dynamic_cast<ItemDoor *>(SelItem)->m_data;
                        tDoor.isSetOut = true;
                        tDoor.isSetIn = false;
                        modData.doors.push_back(tDoor);
                    }
                    else
                    if(SelItem->data(ITEM_TYPE).toString()=="Door_enter"){
                        LevelDoor tDoor = dynamic_cast<ItemDoor *>(SelItem)->m_data;
                        tDoor.isSetOut = false;
                        tDoor.isSetIn = true;
                        modData.doors.push_back(tDoor);
                    }
                    dynamic_cast<ItemDoor *>(SelItem)->m_data.layer = lr.name;
                    dynamic_cast<ItemDoor *>(SelItem)->setVisible(!lr.hidden);
                    dynamic_cast<ItemDoor *>(SelItem)->arrayApply();
                }
            }
            if(isNew)
            {
                m_history->addChangedNewLayer(modData, lr);
            }
            break;
        }
    }//Find layer's settings

    if(!isNew)
    {
        m_history->addChangedLayer(modData, lName);
    }
}
