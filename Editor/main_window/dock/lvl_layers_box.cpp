/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2024 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QComboBox>

#include <editing/_scenes/level/lvl_history_manager.h>
#include <editing/_scenes/level/lvl_item_placing.h>
#include <editing/_scenes/level/lvl_scene.h>
#include <PGE_File_Formats/file_formats.h>
#include <common_features/util.h>
#include <main_window/dock/lvl_item_properties.h>
#include <main_window/dock/lvl_warp_props.h>
#include <main_window/dock/lvl_search_box.h>
#include <main_window/dock/lvl_events_box.h>
#include <networking/engine_intproc.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>

#include "lvl_layers_box.h"
#include "ui_lvl_layers_box.h"

LvlLayersBox::LvlLayersBox(QWidget *parent) :
    QDockWidget(parent),
    MWDock_Base(parent),
    ui(new Ui::LvlLayersBox)
{
    setVisible(false);
    setAttribute(Qt::WA_ShowWithoutActivating);
    ui->setupUi(this);

    QRect mwg = mw()->geometry();
    int GOffset = 10;
    mw()->addDockWidget(Qt::RightDockWidgetArea, this);
    QObject::connect(mw(), &MainWindow::languageSwitched, this, &LvlLayersBox::re_translate);
    QObject::connect(this, &LvlLayersBox::visibilityChanged,
                     mw()->ui->actionLayersBox, &QAction::setChecked);
    setFloating(true);
    setGeometry(
        mwg.right() - width() - GOffset,
        mwg.y() + 120,
        width(),
        height()
    );

    QObject::connect(ui->LvlLayerList->model(), &QAbstractItemModel::rowsMoved, this, &LvlLayersBox::dragAndDroppedLayer);

    m_lastVisibilityState = isVisible();
    mw()->docks_level.
    addState(this, &m_lastVisibilityState);

    m_lockSettings = false;
}

LvlLayersBox::~LvlLayersBox()
{
    delete ui;
}

void LvlLayersBox::re_translate()
{
    ui->retranslateUi(this);
}

void MainWindow::on_actionLayersBox_triggered(bool checked)
{
    dock_LvlLayers->setVisible(checked);
    if(checked) dock_LvlLayers->raise();
}

bool LvlLayersBox::layerIsExist(QString lr, int *index)
{
    LevelEdit *edit = mw()->activeLvlEditWin();
    if(!edit) return false;

    for(int i = 0; i < edit->LvlData.layers.size(); i++)
    {
        if(edit->LvlData.layers[i].name == lr)
        {
            if(index)
                *index = i;
            return true;
            break;
        }
    }
    return false;
}

void LvlLayersBox::setLayersBox()
{
    int WinType = mw()->activeChildWindow();
    QListWidgetItem *item;
    LevelEdit *edit = mw()->activeLvlEditWin();
    if(!edit)
        return;

    LvlPlacingItems::layer.clear();

    util::memclear(ui->LvlLayerList);

    if(WinType == MainWindow::WND_Level)
    {
        foreach(LevelLayer layer, edit->LvlData.layers)
        for(LevelLayer &layer : edit->LvlData.layers)
        {
            item = new QListWidgetItem();
            item->setText(layer.name);
            item->setFlags(Qt::ItemIsUserCheckable);

            if((layer.name != "Destroyed Blocks") && (layer.name != "Spawned NPCs"))
                item->setFlags(item->flags() | Qt::ItemIsEnabled | Qt::ItemIsSelectable);

            if(layer.name != "Default")
                item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsDragEnabled);

            item->setCheckState((layer.hidden) ? Qt::Unchecked : Qt::Checked);
            item->setData(Qt::UserRole, QString::number(layer.meta.array_id));
            ui->LvlLayerList->addItem(item);
        }
    }
}

void MainWindow::LayerListsSync()
{
    dock_LvlItemProps->hide();
    dock_LvlItemProps->setSettingsLock(true);
    dock_LvlWarpProps->setSettingsLock(true);

    //save old entry from search!
    QString curSearchLayerBlock = dock_LvlSearchBox->cbox_layer_block()->currentText();
    QString curSearchLayerBGO = dock_LvlSearchBox->cbox_layer_bgo()->currentText();
    QString curSearchLayerNPC = dock_LvlSearchBox->cbox_layer_npc()->currentText();
    QString curSearchLayerAttachedNPC = dock_LvlSearchBox->cbox_layer_attached_npc()->currentText();

    QString curWarpLayer = dock_LvlWarpProps->cbox_layer()->currentText();

    int WinType = activeChildWindow();

    dock_LvlItemProps->setSettingsLock(true);
    dock_LvlItemProps->cbox_layer_bgo()->clear();
    dock_LvlItemProps->cbox_layer_npc()->clear();
    dock_LvlItemProps->cbox_layer_block()->clear();
    dock_LvlItemProps->cbox_layer_npc_att()->clear();
    dock_LvlItemProps->cbox_layer_npc_att()->addItem(tr("[None]"));

    dock_LvlWarpProps->cbox_layer()->clear();

    dock_LvlEvents->m_externalLock = true;
    dock_LvlEvents->cbox_layer_to_move()->clear();
    dock_LvlEvents->cbox_layer_to_move()->addItem(tr("[None]"));

    dock_LvlSearchBox->cbox_layer_block()->clear();
    dock_LvlSearchBox->cbox_layer_bgo()->clear();
    dock_LvlSearchBox->cbox_layer_npc()->clear();
    dock_LvlSearchBox->cbox_layer_attached_npc()->clear();
    dock_LvlSearchBox->cbox_layer_attached_npc()->addItem(tr("[None]"));

    if(WinType == MainWindow::WND_Level)
    {
        for(LevelLayer &layer : activeLvlEditWin()->LvlData.layers)
        {
            if((layer.name == "Destroyed Blocks") || (layer.name == "Spawned NPCs"))
                continue;

            dock_LvlItemProps->cbox_layer_bgo()->addItem(layer.name);
            dock_LvlItemProps->cbox_layer_npc()->addItem(layer.name);
            dock_LvlItemProps->cbox_layer_block()->addItem(layer.name);
            dock_LvlItemProps->cbox_layer_npc_att()->addItem(layer.name);

            dock_LvlSearchBox->cbox_layer_block()->addItem(layer.name);
            dock_LvlSearchBox->cbox_layer_bgo()->addItem(layer.name);
            dock_LvlSearchBox->cbox_layer_npc()->addItem(layer.name);
            dock_LvlSearchBox->cbox_layer_attached_npc()->addItem(layer.name);
            dock_LvlEvents->cbox_layer_to_move()->addItem(layer.name);
            dock_LvlWarpProps->cbox_layer()->addItem(layer.name, QVariant(layer.name));
        }
    }

    if(!curWarpLayer.isEmpty())
        dock_LvlWarpProps->cbox_layer()->setCurrentText(curWarpLayer);

    dock_LvlSearchBox->cbox_layer_block()->setCurrentText(curSearchLayerBlock);
    dock_LvlSearchBox->cbox_layer_bgo()->setCurrentText(curSearchLayerBGO);
    dock_LvlSearchBox->cbox_layer_npc()->setCurrentText(curSearchLayerNPC);
    dock_LvlSearchBox->cbox_layer_attached_npc()->setCurrentText(curSearchLayerAttachedNPC);

    dock_LvlWarpProps->setSettingsLock(false);
    dock_LvlItemProps->setSettingsLock(false);
    dock_LvlEvents->m_externalLock = false;
}

void LvlLayersBox::setLayerToolsLocked(bool locked)
{
    m_lockSettings = locked;
}



void LvlLayersBox::removeCurrentLayer(bool moveToDefault)
{
    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    LevelEdit *edit = mw()->activeLvlEditWin();

    if(!edit)
        return;

    bool layerVisible = true;
    //Remove from List
    QList<QListWidgetItem * > selected = ui->LvlLayerList->selectedItems();

    if(selected.isEmpty())
        return; // Bad selection

    QListWidgetItem *layer = selected[0];
    Q_ASSERT(layer);

    //Sync layer name with events
    for(int j = 0; j < edit->LvlData.events.size(); j++)
    {
        auto &e = edit->LvlData.events[j];

        for(int i = 0; i < e.layers_hide.size(); i++)
        {
            if(e.layers_hide[i] == layer->text())
                e.layers_hide.removeAt(i--);
        }

        for(int i = 0; i < e.layers_show.size(); i++)
        {
            if(e.layers_show[i] == layer->text())
                e.layers_show.removeAt(i--);
        }

        for(int i = 0; i < e.layers_toggle.size(); i++)
        {
            if(e.layers_toggle[i] == layer->text())
                e.layers_toggle.removeAt(i--);
        }

        if(e.movelayer == layer->text())
            e.movelayer.clear();
    }

    mw()->dock_LvlEvents->setEventsBox(); //Refresh events


    if(moveToDefault)
    {
        //Find default layer visibly
        foreach(LevelLayer layer, edit->LvlData.layers)
        {
            if(layer.name == "Default")
            {
                layerVisible = !layer.hidden;
                break;
            }
        }
    }

    if(moveToDefault)
        modifyLayer(layer->text(), "Default", layerVisible, 0);
    else
        removeLayerItems(layer->text());

    if(!layer)
        return;

    removeLayerFromListAndData(layer);
}


void LvlLayersBox::removeLayerItems(QString layerName)
{
    LevelEdit *edit = mw()->activeLvlEditWin();
    if(!edit) return;

    QList<QGraphicsItem *> ItemList = edit->scene->items();
    LevelData delData;

    for(QList<QGraphicsItem *>::iterator it = ItemList.begin(); it != ItemList.end(); it++)
    {
        if((*it)->data(ITEM_IS_CURSOR).toString() == "CURSOR") continue; //skip cursor item

        if((*it)->data(ITEM_TYPE).toString() == "Block")
        {
            auto *b = qgraphicsitem_cast<ItemBlock *>(*it);
            Q_ASSERT(b);

            if(b->m_data.layer == layerName)
            {
                delData.blocks.push_back(b->m_data);
                b->removeFromArray();
                delete(b);
                //activeLvlEditWin()->scene->removeItem((*it));
            }

        }
        else if((*it)->data(ITEM_TYPE).toString() == "BGO")
        {
            auto *b = qgraphicsitem_cast<ItemBGO *>(*it);
            Q_ASSERT(b);

            if(b->m_data.layer == layerName)
            {
                delData.bgo.push_back(b->m_data);
                b->removeFromArray();
                delete(b);
                //activeLvlEditWin()->scene->removeItem((*it));
            }
        }
        else if((*it)->data(ITEM_TYPE).toString() == "NPC")
        {
            auto *b = qgraphicsitem_cast<ItemNPC *>(*it);
            Q_ASSERT(b);

            if(b->m_data.layer == layerName)
            {
                delData.npc.push_back(b->m_data);
                b->removeFromArray();
                delete(b);
                //activeLvlEditWin()->scene->removeItem((*it));
            }
        }
        else if((*it)->data(ITEM_TYPE).toString() == "Water")
        {
            auto *b = qgraphicsitem_cast<ItemPhysEnv *>(*it);
            Q_ASSERT(b);

            if(b->m_data.layer == layerName)
            {
                delData.physez.push_back(b->m_data);
                b->removeFromArray();
                delete(b);
                //activeLvlEditWin()->scene->removeItem((*it));
            }
        }
        else if(((*it)->data(ITEM_TYPE).toString() == "Door_enter") || ((*it)->data(ITEM_TYPE).toString() == "Door_exit"))
        {
            auto *b = qgraphicsitem_cast<ItemDoor *>(*it);
            Q_ASSERT(b);

            if(b->m_data.layer == layerName)
            {
                if(b->data(ITEM_TYPE).toString() == "Door_enter")
                {
                    LevelDoor tData = ((ItemDoor *)(*it))->m_data;
                    tData.isSetIn = true;
                    tData.isSetOut = false;
                    delData.doors.push_back(tData);
                }
                else if(b->data(ITEM_TYPE).toString() == "Door_exit")
                {
                    LevelDoor tData = ((ItemDoor *)(*it))->m_data;
                    tData.isSetIn = false;
                    tData.isSetOut = true;
                    delData.doors.push_back(tData);
                }

                b->removeFromArray();
                delete(b);
                //activeLvlEditWin()->scene->removeItem((*it));
            }
        }
    }

    for(LevelLayer &l : edit->LvlData.layers)
    {
        if(l.name == layerName)
        {
            delData.layers.push_back(l);
            break;
        }
    }

    edit->scene->m_history->addRemoveLayer(delData);

    mw()->LayerListsSync();  //Sync comboboxes in properties
}

void LvlLayersBox::removeLayerFromListAndData(QListWidgetItem *layerItem)
{
    LevelEdit *edit = mw()->activeLvlEditWin();
    if(!edit)
        return;

    if(layerItem->text().compare("Destroyed Blocks", Qt::CaseInsensitive) == 0)
        return;

    if(layerItem->text().compare("Spawned NPCs", Qt::CaseInsensitive) == 0)
        return;

    if(layerItem->text().compare("Default", Qt::CaseInsensitive) == 0)
        return;

    int WinType = mw()->activeChildWindow();

    if(WinType == MainWindow::WND_Level)
    {
        for(int i = 0; i < edit->LvlData.layers.size(); i++)
        {
            if(edit->LvlData.layers[i].meta.array_id == (unsigned int)layerItem->data(Qt::UserRole).toInt())
            {
                edit->LvlData.layers.removeAt(i);
                delete layerItem;
                break;
            }
        }
    }

    mw()->LayerListsSync();  //Sync comboboxes in properties
}

void LvlLayersBox::modifyLayer(QString layerName, bool visible)
{
    //Apply layer's visibly to all items
    LevelEdit *edit = mw()->activeLvlEditWin();
    if(!edit) return;

    QList<QGraphicsItem *> ItemList = edit->scene->items();

    for(QList<QGraphicsItem *>::iterator it = ItemList.begin(); it != ItemList.end(); it++)
    {
        if((*it)->data(ITEM_IS_CURSOR).toString() == "CURSOR") continue; //skip cursor item

        if((*it)->data(ITEM_TYPE).toString() == "Block")
        {
            if(((ItemBlock *)(*it))->m_data.layer == layerName)
                (*it)->setVisible(visible);

        }
        else if((*it)->data(ITEM_TYPE).toString() == "BGO")
        {
            if(((ItemBGO *)(*it))->m_data.layer == layerName)
                (*it)->setVisible(visible);
        }
        else if((*it)->data(ITEM_TYPE).toString() == "NPC")
        {
            if(((ItemNPC *)(*it))->m_data.layer == layerName)
                (*it)->setVisible(visible);
        }
        else if((*it)->data(ITEM_TYPE).toString() == "Water")
        {
            if(((ItemPhysEnv *)(*it))->m_data.layer == layerName)
                (*it)->setVisible(visible);
        }
        else if(((*it)->data(ITEM_TYPE).toString() == "Door_enter") || ((*it)->data(ITEM_TYPE).toString() == "Door_exit"))
        {
            if(((ItemDoor *)(*it))->m_data.layer == layerName)
                (*it)->setVisible(visible);
        }
    }
    mw()->LayerListsSync();  //Sync comboboxes in properties
}

void LvlLayersBox::modifyLayer(QString layerName, QString newLayerName)
{
    //Apply layer's name to all items
    LevelEdit *edit = mw()->activeLvlEditWin();
    if(!edit) return;

    QList<QGraphicsItem *> ItemList = edit->scene->items();

    for(QList<QGraphicsItem *>::iterator it = ItemList.begin(); it != ItemList.end(); it++)
    {
        if((*it)->data(ITEM_IS_CURSOR).toString() == "CURSOR") continue; //skip cursor item

        if((*it)->data(ITEM_TYPE).toString() == "Block")
        {
            if(((ItemBlock *)(*it))->m_data.layer == layerName)
            {
                ((ItemBlock *)(*it))->m_data.layer = newLayerName;
                ((ItemBlock *)(*it))->arrayApply();
            }
        }
        else if((*it)->data(ITEM_TYPE).toString() == "BGO")
        {
            if(((ItemBGO *)(*it))->m_data.layer == layerName)
            {
                ((ItemBGO *)(*it))->m_data.layer = newLayerName;
                ((ItemBGO *)(*it))->arrayApply();
            }
        }
        else if((*it)->data(ITEM_TYPE).toString() == "NPC")
        {
            if(((ItemNPC *)(*it))->m_data.layer == layerName)
            {
                ((ItemNPC *)(*it))->m_data.layer = newLayerName;
                ((ItemNPC *)(*it))->arrayApply();
            }
        }
        else if((*it)->data(ITEM_TYPE).toString() == "Water")
        {
            if(((ItemPhysEnv *)(*it))->m_data.layer == layerName)
            {
                ((ItemPhysEnv *)(*it))->m_data.layer = newLayerName;
                ((ItemPhysEnv *)(*it))->arrayApply();
            }
        }
        else if(((*it)->data(ITEM_TYPE).toString() == "Door_enter") || ((*it)->data(ITEM_TYPE).toString() == "Door_exit"))
        {
            if(((ItemDoor *)(*it))->m_data.layer == layerName)
            {
                ((ItemDoor *)(*it))->m_data.layer = newLayerName;
                ((ItemDoor *)(*it))->arrayApply();
            }
        }
    }

    //Sync layer name with events
    for(int j = 0; j < edit->LvlData.events.size(); j++)
    {
        for(int i = 0; i < edit->LvlData.events[j].layers_hide.size(); i++)
        {
            if(edit->LvlData.events[j].layers_hide[i] == layerName)
                edit->LvlData.events[j].layers_hide[i] = newLayerName;
        }

        for(int i = 0; i < edit->LvlData.events[j].layers_show.size(); i++)
        {
            if(edit->LvlData.events[j].layers_show[i] == layerName)
                edit->LvlData.events[j].layers_show[i] = newLayerName;
        }

        for(int i = 0; i < edit->LvlData.events[j].layers_toggle.size(); i++)
        {
            if(edit->LvlData.events[j].layers_toggle[i] == layerName)
                edit->LvlData.events[j].layers_toggle[i] = newLayerName;
        }

        if(edit->LvlData.events[j].movelayer == layerName)
            edit->LvlData.events[j].movelayer = newLayerName;
    }
    mw()->dock_LvlEvents->setEventsBox();//Refresh events

    mw()->LayerListsSync();  //Sync comboboxes in properties
}

void LvlLayersBox::modifyLayer(QString layerName, QString newLayerName, bool visible, int historyRecord)
{
    //Apply layer's name/visibly to all items
    LevelEdit *edit = mw()->activeLvlEditWin();
    if(!edit)
        return;

    QList<QGraphicsItem *> ItemList = edit->scene->items();


    LevelData modData;
    for(QList<QGraphicsItem *>::iterator it = ItemList.begin(); it != ItemList.end(); it++)
    {
        if((*it)->data(ITEM_IS_CURSOR).toString() == "CURSOR")
            continue; //skip cursor item

        if((*it)->data(ITEM_TYPE).toString() == "Block")
        {
            if(((ItemBlock *)(*it))->m_data.layer == layerName)
            {
                modData.blocks.push_back(((ItemBlock *)(*it))->m_data);
                ((ItemBlock *)(*it))->m_data.layer = newLayerName;
                (*it)->setVisible(visible);
                ((ItemBlock *)(*it))->arrayApply();
            }

        }
        else if((*it)->data(ITEM_TYPE).toString() == "BGO")
        {
            if(((ItemBGO *)(*it))->m_data.layer == layerName)
            {
                modData.bgo.push_back(((ItemBGO *)(*it))->m_data);
                ((ItemBGO *)(*it))->m_data.layer = newLayerName;
                (*it)->setVisible(visible);
                ((ItemBGO *)(*it))->arrayApply();

            }
        }
        else if((*it)->data(ITEM_TYPE).toString() == "NPC")
        {
            if(((ItemNPC *)(*it))->m_data.layer == layerName)
            {
                modData.npc.push_back(((ItemNPC *)(*it))->m_data);
                ((ItemNPC *)(*it))->m_data.layer = newLayerName;
                (*it)->setVisible(visible);
                ((ItemNPC *)(*it))->arrayApply();
            }
        }
        else if((*it)->data(ITEM_TYPE).toString() == "Water")
        {
            if(((ItemPhysEnv *)(*it))->m_data.layer == layerName)
            {
                modData.physez.push_back(((ItemPhysEnv *)(*it))->m_data);
                ((ItemPhysEnv *)(*it))->m_data.layer = newLayerName;
                (*it)->setVisible(visible);
                ((ItemPhysEnv *)(*it))->arrayApply();
            }
        }
        else if(((*it)->data(ITEM_TYPE).toString() == "Door_enter") || ((*it)->data(ITEM_TYPE).toString() == "Door_exit"))
        {
            if(((ItemDoor *)(*it))->m_data.layer == layerName)
            {
                if((*it)->data(ITEM_TYPE).toString() == "Door_enter")
                {
                    LevelDoor tData = ((ItemDoor *)(*it))->m_data;
                    tData.isSetIn = true;
                    tData.isSetOut = false;
                    modData.doors.push_back(tData);
                }
                else if((*it)->data(ITEM_TYPE).toString() == "Door_exit")
                {
                    LevelDoor tData = ((ItemDoor *)(*it))->m_data;
                    tData.isSetIn = false;
                    tData.isSetOut = true;
                    modData.doors.push_back(tData);
                }

                ((ItemDoor *)(*it))->m_data.layer = newLayerName;
                (*it)->setVisible(visible);
                ((ItemDoor *)(*it))->arrayApply();
            }
        }
    }

    //Apply new layer name for non-placed warps
    for(int i = 0; i < edit->LvlData.doors.size(); i++)
    {
        if(edit->LvlData.doors[i].layer == layerName)
            edit->LvlData.doors[i].layer = newLayerName;
    }

    if(historyRecord == 0)
    {
        if(newLayerName == "Default")
        {
            for(int i = 0; i < edit->LvlData.layers.size(); i++)
            {
                if(edit->LvlData.layers[i].name == layerName)
                {
                    modData.layers.push_back(edit->LvlData.layers[i]);
                    edit->scene->m_history->addRemoveLayerAndSaveItems(modData);
                    break;
                }
            }
        }
    }
    else if(historyRecord == 1)
    {
        for(int i = 0; i < edit->LvlData.layers.size(); i++)
        {
            if(edit->LvlData.layers[i].name == layerName)
            {
                modData.layers.push_back(edit->LvlData.layers[i]);
                edit->scene->m_history->addMergeLayer(modData, newLayerName);
                break;
            }
        }
    }


    //Sync layer name with events
    for(int j = 0; j < edit->LvlData.events.size(); j++)
    {
        for(int i = 0; i < edit->LvlData.events[j].layers_hide.size(); i++)
        {
            if(edit->LvlData.events[j].layers_hide[i] == layerName)
                edit->LvlData.events[j].layers_hide[i] = newLayerName;
        }

        for(int i = 0; i < edit->LvlData.events[j].layers_show.size(); i++)
        {
            if(edit->LvlData.events[j].layers_show[i] == layerName)
                edit->LvlData.events[j].layers_show[i] = newLayerName;
        }

        for(int i = 0; i < edit->LvlData.events[j].layers_toggle.size(); i++)
        {
            if(edit->LvlData.events[j].layers_toggle[i] == layerName)
                edit->LvlData.events[j].layers_toggle[i] = newLayerName;
        }

        if(edit->LvlData.events[j].movelayer == layerName)
            edit->LvlData.events[j].movelayer = newLayerName;
    }

    mw()->setEventsBox(); //Refresh events
    mw()->LayerListsSync();  //Sync comboboxes in properties
}

void LvlLayersBox::addNewLayer(QString layerName, bool setEdited)
{
    LevelEdit *edit = mw()->activeLvlEditWin();
    if(!edit)
        return;

    if(layerIsExist(layerName))
        return;

    QListWidgetItem *item;
    item = new QListWidgetItem;
    item->setText(layerName);
    item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEditable);
    item->setFlags(item->flags() | Qt::ItemIsEnabled);
    item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsSelectable);
    item->setCheckState(Qt::Checked);

    //Register layer in the leveldata structure
    LevelLayer NewLayer;
    NewLayer.name = item->text();
    NewLayer.hidden = (item->checkState() == Qt::Unchecked);
    edit->LvlData.layers_array_id++;
    NewLayer.meta.array_id = edit->LvlData.layers_array_id;
    edit->scene->m_history->addAddLayer(NewLayer.meta.array_id, NewLayer.name, !NewLayer.hidden);

    item->setData(Qt::UserRole, QString::number(NewLayer.meta.array_id));
    ui->LvlLayerList->addItem(item);

    edit->LvlData.layers.push_back(NewLayer);
    edit->LvlData.meta.modified = true;

    if(setEdited)
    {
        if(item)
        {
            ui->LvlLayerList->setFocus();
            ui->LvlLayerList->scrollToItem(item);
            ui->LvlLayerList->editItem(item);
        }
    }

    mw()->LayerListsSync();  //Sync comboboxes in properties
}

void LvlLayersBox::modifyLayerItem(QListWidgetItem *item, QString oldLayerName, QString newLayerName, bool visible)
{
    //Find layer enrty in array and apply settings
    LevelEdit *edit = mw()->activeLvlEditWin();
    if(!edit) return;

    for(int i = 0; i < edit->LvlData.layers.size(); i++)
    {
        if(edit->LvlData.layers[i].meta.array_id == (unsigned int)item->data(Qt::UserRole).toInt())
        {
            int l = 0;
            bool merge = false;
            bool exist = false;
            oldLayerName = edit->LvlData.layers[i].name;

            if(newLayerName.isEmpty())
            {
                //Discard change to empty
                m_lockSettings = true;
                item->setText(oldLayerName);
                m_lockSettings = false;
                return;
            }

            if(oldLayerName != newLayerName)
            {
                //Check for exists item equal to new item
                if(layerIsExist(newLayerName, &l))
                {
                    exist = true;
                    QMessageBox::StandardButton reply;
                    reply = QMessageBox::question(this, tr("Layers merge"),
                                                  tr("Layer with name '%1' already exist, do you want to merge layers?").arg(newLayerName),
                                                  QMessageBox::Yes | QMessageBox::No);
                    if(reply == QMessageBox::Yes)
                        merge = true;
                }
            }

            if(exist)
            {
                if(merge)
                {
                    //Merge layers
                    m_lockSettings = true;
                    edit->LvlData.layers[l].name = newLayerName;
                    edit->LvlData.layers[l].hidden = !visible;
                    modifyLayer(newLayerName, visible);
                    modifyLayer(oldLayerName, newLayerName, visible, 1);
                    delete item;
                    edit->LvlData.layers.removeAt(i);
                    mw()->LayerListsSync();  //Sync comboboxes in properties
                    setLayersBox();
                    m_lockSettings = false;
                    return;
                }
                else
                {
                    //cancel renaming
                    m_lockSettings = true;
                    item->setText(oldLayerName);
                    m_lockSettings = false;
                    return;
                }
            }
            else
            {
                if(oldLayerName != newLayerName)
                    edit->scene->m_history->addRenameLayer(edit->LvlData.layers[i].meta.array_id,
                                                           oldLayerName,
                                                           newLayerName);
                if(edit->LvlData.layers[i].hidden != !visible)
                    edit->scene->m_history->addChangedLayerVisibility(edit->LvlData.layers[i].meta.array_id,
                                                                      oldLayerName,
                                                                      !edit->LvlData.layers[i].hidden);
                edit->LvlData.layers[i].name = newLayerName;
                edit->LvlData.layers[i].hidden = !visible;
                //Apply layer's name/visibly to all items
                modifyLayer(oldLayerName, newLayerName, visible);
            }
            break;
        }
    }

    mw()->LayerListsSync();  //Sync comboboxes in properties
    mw()->dock_LvlWarpProps->setDoorData(-2);
}

void LvlLayersBox::dragAndDroppedLayer(QModelIndex /*sourceParent*/, int sourceStart, int sourceEnd, QModelIndex /*destinationParent*/, int destinationRow)
{
    LevelEdit *edit = mw()->activeLvlEditWin();
    if(!edit) return;

    LogDebug("Row Change at " + QString::number(sourceStart) +
             " " + QString::number(sourceEnd) +
             " to " + QString::number(destinationRow));

    int WinType = mw()->activeChildWindow();
    if(WinType == MainWindow::WND_Level)
    {
        LevelLayer buffer;
        if(sourceStart < edit->LvlData.layers.size())
        {
            buffer = edit->LvlData.layers[sourceStart];
            edit->LvlData.layers.removeAt(sourceStart);
            edit->LvlData.layers.insert(((destinationRow > sourceStart) ? destinationRow - 1 : destinationRow), buffer);
        }
    }

    mw()->LayerListsSync(); //Sync comboboxes in properties
}


void LvlLayersBox::on_AddLayer_clicked()
{
    int NewCounter = 1;
    QString newName = tr("New Layer %1");
    while(layerIsExist(newName.arg(NewCounter)))
        NewCounter++;
    addNewLayer(newName.arg(NewCounter), true);
}


void LvlLayersBox::on_LvlLayerList_itemClicked(QListWidgetItem *item)
{
    int WinType = mw()->activeChildWindow();
    int itemType = 0;
    LevelEdit *edit = nullptr;
    if(WinType == MainWindow::WND_Level)
        edit = mw()->activeLvlEditWin();
    bool allow = (edit && edit->sceneCreated);

    if(allow)
        itemType = edit->scene->m_placingItemType;

    if((allow) && (itemType == LvlScene::PLC_Block ||
                   itemType == LvlScene::PLC_BGO ||
                   itemType == LvlScene::PLC_NPC ||
                   itemType == LvlScene::PLC_Water))
        LvlPlacingItems::layer = item->text();
    else
        LvlPlacingItems::layer.clear();

    LvlPlacingItems::blockSet.layer = LvlPlacingItems::layer;
    LvlPlacingItems::bgoSet.layer = LvlPlacingItems::layer;
    LvlPlacingItems::npcSet.layer = LvlPlacingItems::layer;
    LvlPlacingItems::waterSet.layer = LvlPlacingItems::layer;
    mw()->dock_LvlItemProps->syncLayerFields(LvlPlacingItems::layer);
    g_intEngine.sendCurrentLayer(LvlPlacingItems::layer);
}

void LvlLayersBox::on_LvlLayerList_itemSelectionChanged()
{
    if(ui->LvlLayerList->selectedItems().isEmpty())
        LvlPlacingItems::layer.clear();
    else
        LvlPlacingItems::layer = ui->LvlLayerList->selectedItems().first()->text();

    LvlPlacingItems::blockSet.layer = LvlPlacingItems::layer;
    LvlPlacingItems::bgoSet.layer = LvlPlacingItems::layer;
    LvlPlacingItems::npcSet.layer = LvlPlacingItems::layer;
}


void LvlLayersBox::on_LvlLayerList_itemChanged(QListWidgetItem *item)
{
    if(m_lockSettings) return;
    int WinType = mw()->activeChildWindow();

    LevelEdit *edit = mw()->activeLvlEditWin();
    if(!edit)
        return;

    if(WinType == MainWindow::WND_Level)
    {
        QString layerName = item->text();
        QString oldLayerName = item->text();

        bool layerVisible = (item->checkState() == Qt::Checked);
        modifyLayerItem(item, oldLayerName, layerName, layerVisible);

        edit->LvlData.meta.modified = true;
    }
}


void LvlLayersBox::on_RemoveLayer_clicked()
{
    if(ui->LvlLayerList->selectedItems().isEmpty()) return;
    QMessageBox::StandardButton answer = QMessageBox::question(mw(),
                                         tr("Remove layer"),
                                         tr("Are you sure you want to remove this layer?\nAll objects on this layer will be moved to the 'Default' layer."),
                                         QMessageBox::Yes | QMessageBox::No);
    if(answer == QMessageBox::Yes) removeCurrentLayer(true);
}

static QString boldIfNonZero(const QString &label, int number)
{
    if(number > 0)
        return QString("<li><b>%1</b>: %2</li>\n").arg(label).arg(number);
    else
        return QString("<li>%1: %2</li>\n").arg(label).arg(number);
}

void LvlLayersBox::on_LvlLayerList_customContextMenuRequested(const QPoint &pos)
{
    if(ui->LvlLayerList->selectedItems().isEmpty())
        return;

    QListWidgetItem *layerItem = ui->LvlLayerList->selectedItems()[0];
    QString layerName = layerItem->text();
    bool isDefaultLayer = layerName == "Default";

    QPoint globPos = ui->LvlLayerList->mapToGlobal(pos);

    LogDebug(QString("Layer's context menu called! %1 %2 -> %3 %4")
             .arg(pos.x()).arg(pos.y())
             .arg(globPos.x()).arg(globPos.y()));

    QMenu layer_menu(this);
    QAction *rename = nullptr;

    if(!isDefaultLayer)
        rename = layer_menu.addAction(tr("Rename layer"));

    QAction *selectAll = layer_menu.addAction(tr("Select all items"));
    if(layerItem->checkState() != Qt::Checked)
        selectAll->setEnabled(false);

    layer_menu.addSeparator();
    QAction *showStats = layer_menu.addAction(tr("Show layer statistics..."));

    QAction *removeLayer = nullptr;
    QAction *removeLayerOnly = nullptr;

    if(!isDefaultLayer)
    {
        layer_menu.addSeparator();
        removeLayer = layer_menu.addAction(tr("Remove layer with items"));
        removeLayerOnly = layer_menu.addAction(tr("Remove Layer and keep items"));
    }

    QAction *selected = layer_menu.exec(globPos);

    if(selected == rename)
        ui->LvlLayerList->editItem(layerItem);
    else if(selected == showStats)
    {
        LevelEdit *lvlEdit = mw()->activeLvlEditWin();
        Q_ASSERT(lvlEdit);
        Q_ASSERT(lvlEdit->scene);
        Q_ASSERT(lvlEdit->scene->m_data);
        const LevelData *d = lvlEdit->scene->m_data;

        int statsTotalMembers = 0;
        int statsBlocks = 0;
        int statsBGOs = 0;
        int statsNPCs = 0;
        int statsWarps = 0;
        int statsPEZs = 0;
        int statsAttached = 0;
        int statsEventsRefs = 0;

        QSet<QString> referredByEvents;

        for(auto &o : d->blocks)
        {
            if(layerName.compare(o.layer, Qt::CaseInsensitive) == 0)
            {
                statsTotalMembers++;
                statsBlocks++;
            }
        }

        for(auto &o : d->bgo)
        {
            if(layerName.compare(o.layer, Qt::CaseInsensitive) == 0)
            {
                statsTotalMembers++;
                statsBGOs++;
            }
        }

        for(auto &o : d->npc)
        {
            if(layerName.compare(o.layer, Qt::CaseInsensitive) == 0)
            {
                statsTotalMembers++;
                statsNPCs++;
            }

            if(layerName.compare(o.attach_layer, Qt::CaseInsensitive) == 0)
                statsAttached++;
        }

        for(auto &o : d->doors)
        {
            if(layerName.compare(o.layer, Qt::CaseInsensitive) == 0)
            {
                statsTotalMembers++;
                statsWarps++;
            }
        }

        for(auto &o : d->physez)
        {
            if(layerName.compare(o.layer, Qt::CaseInsensitive) == 0)
            {
                statsTotalMembers++;
                statsPEZs++;
            }
        }

        for(auto &e : d->events)
        {
            for(auto &o : e.layers_hide)
            {
                if(layerName.compare(o, Qt::CaseInsensitive) == 0)
                {
                    statsEventsRefs++;
                    referredByEvents.insert(e.name);
                }
            }

            for(auto &o : e.layers_show)
            {
                if(layerName.compare(o, Qt::CaseInsensitive) == 0)
                {
                    statsEventsRefs++;
                    referredByEvents.insert(e.name);
                }
            }

            for(auto &o : e.layers_toggle)
            {
                if(layerName.compare(o, Qt::CaseInsensitive) == 0)
                {
                    statsEventsRefs++;
                    referredByEvents.insert(e.name);
                }
            }
        }

        bool used = statsTotalMembers > 0 || statsAttached > 0 || statsEventsRefs > 0;
        QString stats;

        if(used)
        {
            stats = "<h2>" + tr("Layer \"%1\" statistics").arg(layerName) + ":</h2>\n";
            stats += "<ul>";
            stats += boldIfNonZero(tr("Total members", "Layer statistics field"), statsTotalMembers);
            stats += boldIfNonZero(tr("Blocks", "Layer statistics field"), statsTotalMembers);
            stats += boldIfNonZero(tr("Background objects", "Layer statistics field"), statsBGOs);
            stats += boldIfNonZero(tr("NPC", "Layer statistics field"), statsNPCs);
            stats += boldIfNonZero(tr("Warps", "Layer statistics field"), statsWarps);
            stats += boldIfNonZero(tr("Physical environments", "Layer statistics field"), statsPEZs);
            stats += "</ul>";

            stats += "<h2>" + tr("Usage") + ":</h2>\n";
            stats += "<ul>";
            stats += boldIfNonZero(tr("Attaches to NPCs", "Layer statistics field"), statsAttached);
            stats += boldIfNonZero(tr("Referred in events", "Layer statistics field"), statsEventsRefs);
            stats += "</ul>";
        }
        else
            stats = tr("<h2>Layer \"%1\" statistics:</h2>\n"
                       "<center>&lt;This is an unused layer with no members&gt;</center>\n").arg(layerName);

        if(!referredByEvents.isEmpty())
        {
            stats += "<h2>" + tr("Referred in events") + ":</h2>\n";
            stats += "<ul>\n";
            for(auto &e : referredByEvents)
                stats += QString("<li>%1</li>\n").arg(e);
            stats += "</ul>\n";
        }


        QMessageBox s(this);
        QPushButton removeButton;
        removeButton.setText(tr("Remove this layer"));
        s.setText(stats);
        s.setTextFormat(Qt::RichText);
        s.setWindowTitle(tr("Layer statistics"));
        s.addButton(QMessageBox::Close);
        if(!used)
        {
            s.addButton(&removeButton, QMessageBox::ActionRole);
            QObject::connect(&removeButton, static_cast<void (QPushButton::*)(bool)>(&QPushButton::clicked),
                             [this](bool)->void
                             {
                                 removeCurrentLayer(false);
                             });
        }
        s.exec();
    }
    else if(selected == selectAll)
    {
        LevelEdit *lvlEdit = mw()->activeLvlEditWin();
        LvlScene *scene = nullptr;

        if(!lvlEdit || !lvlEdit->sceneCreated || !lvlEdit->scene)
        {
            qWarning() << "Attempt to send an action from a non-initialized scene";
            return;
        }

        scene = lvlEdit->scene;
        scene->clearSelection();

        QList<QGraphicsItem*> items = lvlEdit->getGraphicsView()->items();

        for(auto *it : items)
        {
            if(it->data(ITEM_IS_ITEM).toInt() != 1)
                continue;

            LvlBaseItem* item = dynamic_cast<LvlBaseItem *>(it);
            Q_ASSERT(item);
            item->setSelected(item->getLayerName() == layerName);
        }
    }
    else if((selected == removeLayer) || (selected == removeLayerOnly))
    {
        if(selected == removeLayerOnly)
        {
            QMessageBox::StandardButton answer = QMessageBox::question(mw(),
                                                 tr("Remove layer"),
                                                 tr("Are you sure you want to remove this layer?\nAll elements of this layer will be moved to the 'Default' layer!"),
                                                 QMessageBox::Yes | QMessageBox::No);
            if(answer == QMessageBox::Yes) removeCurrentLayer(true);
        }
        else if(selected == removeLayer)
        {
            QMessageBox::StandardButton answer = QMessageBox::question(mw(),
                                                 tr("Remove layer"),
                                                 tr("Are you sure you want to remove this layer?\nAll elements of this layer will be removed too!"),
                                                 QMessageBox::Yes | QMessageBox::No);
            if(answer == QMessageBox::Yes) removeCurrentLayer(false);
        }
    }
}
