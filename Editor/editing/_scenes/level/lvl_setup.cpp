/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2026 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <mainwindow.h>
#include <common_features/themes.h>
#include <main_window/global_settings.h>
#include <editing/edit_level/level_edit.h>
#include <main_window/dock/lvl_layers_box.h>
#include <main_window/dock/lvl_events_box.h>

#include "lvl_history_manager.h"
#include "newlayerbox.h"


void LvlScene::hideWarpsAndDoors(bool visible)
{
    QMap<QString, LevelLayer> localLayers;

    for(auto &layer : m_data->layers)
        localLayers[layer.name] = layer;

    foreach(QGraphicsItem *i, m_itemsAll)
    {
        if(i->data(ITEM_IS_ITEM).isNull() || !i->data(ITEM_IS_ITEM).toBool())
            continue;

        int objType = i->data(ITEM_TYPE_INT).toInt();

        if(objType == ItemTypes::LVL_PhysEnv)
            i->setVisible(!localLayers[qgraphicsitem_cast<ItemPhysEnv *>(i)->m_data.layer].hidden && visible);
        else if(objType == ItemTypes::LVL_META_DoorEnter || objType == ItemTypes::LVL_META_DoorExit)
            i->setVisible(!localLayers[qgraphicsitem_cast<ItemDoor *>(i)->m_data.layer].hidden && visible);
    }
}


void LvlScene::setTiledBackground(bool forceTiled)
{
    ChangeSectionBG(m_data->sections[m_data->CurSection].background, -1, forceTiled);
}

void LvlScene::applyLayersVisible()
{
    foreach(QGraphicsItem *item, m_itemsAll)
    {
        if(item->data(ITEM_IS_ITEM).isNull() || !item->data(ITEM_IS_ITEM).toBool())
            continue;

        int objType = item->data(ITEM_TYPE_INT).toInt();

        if(objType == ItemTypes::LVL_Block)
        {
            foreach(const LevelLayer &layer, m_data->layers)
            {
                auto *itm = qgraphicsitem_cast<ItemBlock *>(item);
                if(itm->m_data.layer == layer.name)
                {
                    itm->setVisible(!layer.hidden);
                    break;
                }
            }
        }
        else if(objType == ItemTypes::LVL_BGO)
        {
            foreach(const LevelLayer &layer, m_data->layers)
            {
                auto *itm = qgraphicsitem_cast<ItemBGO *>(item);
                if(itm->m_data.layer == layer.name)
                {
                    itm->setVisible(!layer.hidden);
                    break;
                }
            }
        }
        else if(objType == ItemTypes::LVL_NPC)
        {
            foreach(const LevelLayer &layer, m_data->layers)
            {
                auto *itm = qgraphicsitem_cast<ItemNPC *>(item);
                if(itm->m_data.layer == layer.name)
                {
                    itm->setVisible(!layer.hidden);
                    break;
                }
            }
        }
        else if(objType == ItemTypes::LVL_PhysEnv)
        {
            foreach(const LevelLayer &layer, m_data->layers)
            {
                auto *itm = qgraphicsitem_cast<ItemPhysEnv *>(item);
                if(itm->m_data.layer == layer.name)
                {
                    itm->setVisible(!layer.hidden);
                    break;
                }
            }
        }
        else if(objType == ItemTypes::LVL_META_DoorEnter || objType == ItemTypes::LVL_META_DoorExit)
        {
            foreach(const LevelLayer &layer, m_data->layers)
            {
                auto *itm = qgraphicsitem_cast<ItemDoor *>(item);
                if(itm->m_data.layer == layer.name)
                {
                    itm->setVisible(!layer.hidden);
                    break;
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
    default:
        break;
    }

    // QList<QGraphicsItem *> ItemList = items();
    foreach(auto &it, m_itemsAll)
    {
        if(it == nullptr)
            continue;

        if(it->data(ITEM_IS_ITEM).isNull() || !it->data(ITEM_IS_ITEM).toBool())
            continue;

        int objType = it->data(ITEM_TYPE_INT).toInt();

        switch(type)
        {
        case 1://Block
            if(objType == ItemTypes::LVL_Block)
            {
                auto *item = qgraphicsitem_cast<ItemBlock *>(it);
                if(item) item->setLocked(lock);
            }
            break;
        case 2://BGO
            if(objType == ItemTypes::LVL_BGO)
            {
                auto *i = qgraphicsitem_cast<ItemBGO *>(it);
                if(i) i->setLocked(lock);
            }
            break;
        case 3://NPC
            if(objType == ItemTypes::LVL_NPC)
            {
                auto *i = qgraphicsitem_cast<ItemNPC *>(it);
                if(i) i->setLocked(lock);
            }
            break;
        case 4://Water
            if(objType == ItemTypes::LVL_PhysEnv)
            {
                auto *i = qgraphicsitem_cast<ItemPhysEnv *>(it);
                if(i) i->setLocked(lock);
            }
            break;
        case 5://Doors
            if(objType == ItemTypes::LVL_META_DoorEnter || objType == ItemTypes::LVL_META_DoorExit)
            {
                auto *i = qgraphicsitem_cast<ItemDoor *>(it);
                if(i) i->setLocked(lock);
            }
            break;
        default:
            break;
        }
    }

}


void LvlScene::setLayerToSelected()
{
    QString lName;
    auto *layerBox = new ToNewLayerBox(m_data, m_viewPort);
    layerBox->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    layerBox->setGeometry(util::alignToScreenCenter(layerBox->size()));

    if(layerBox->exec() == QDialog::Accepted)
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

void LvlScene::setLayerToSelected(const QString &lName, bool isNew)
{
    LevelData modData;
    foreach(const LevelLayer &lr, m_data->layers)
    {
        //Find layer's settings
        if(lr.name == lName)
        {
            for(QGraphicsItem *SelItem : selectedItems())
            {
                int objType = SelItem->data(ITEM_TYPE_INT).toInt();

                if(objType == ItemTypes::LVL_Block)
                {
                    auto *itm = qgraphicsitem_cast<ItemBlock *>(SelItem);
                    modData.blocks.push_back(itm->m_data);
                    itm->m_data.layer = lr.name;
                    itm->setVisible(!lr.hidden);
                    itm->arrayApply();
                }
                else if(objType == ItemTypes::LVL_BGO)
                {
                    auto *itm = qgraphicsitem_cast<ItemBGO *>(SelItem);
                    modData.bgo.push_back(itm->m_data);
                    itm->m_data.layer = lr.name;
                    itm->setVisible(!lr.hidden);
                    itm->arrayApply();
                }
                else if(objType == ItemTypes::LVL_NPC)
                {
                    auto *itm = qgraphicsitem_cast<ItemNPC *>(SelItem);
                    modData.npc.push_back(itm->m_data);
                    itm->m_data.layer = lr.name;
                    itm->setVisible(!lr.hidden);
                    itm->arrayApply();
                }
                else if(objType == ItemTypes::LVL_PhysEnv)
                {
                    auto *itm = qgraphicsitem_cast<ItemPhysEnv *>(SelItem);
                    modData.physez.push_back(itm->m_data);
                    itm->m_data.layer = lr.name;
                    itm->setVisible(!lr.hidden);
                    itm->arrayApply();
                }
                else if(objType == ItemTypes::LVL_META_DoorEnter || objType == ItemTypes::LVL_META_DoorExit)
                {
                    auto *itm = qgraphicsitem_cast<ItemDoor *>(SelItem);

                    if(objType == ItemTypes::LVL_META_DoorExit)
                    {
                        LevelDoor tDoor = itm->m_data;
                        tDoor.isSetOut = true;
                        tDoor.isSetIn = false;
                        modData.doors.push_back(tDoor);
                    }
                    else if(objType == ItemTypes::LVL_META_DoorEnter)
                    {
                        LevelDoor tDoor = itm->m_data;
                        tDoor.isSetOut = false;
                        tDoor.isSetIn = true;
                        modData.doors.push_back(tDoor);
                    }

                    itm->m_data.layer = lr.name;
                    itm->setVisible(!lr.hidden);
                    itm->arrayApply();
                }
            }

            if(isNew)
                m_history->addChangedNewLayer(modData, lr);

            break;
        }
    }//Find layer's settings

    if(!isNew)
        m_history->addChangedLayer(modData, lName);
}
