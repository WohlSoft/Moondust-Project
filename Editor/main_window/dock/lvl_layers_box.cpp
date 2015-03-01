/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <editing/_scenes/level/items/item_bgo.h>
#include <editing/_scenes/level/items/item_block.h>
#include <editing/_scenes/level/items/item_npc.h>
#include <editing/_scenes/level/items/item_water.h>
#include <editing/_scenes/level/items/item_door.h>
#include <editing/_scenes/level/lvl_item_placing.h>
#include <file_formats/file_formats.h>
#include <common_features/util.h>
#include <main_window/dock/lvl_item_properties.h>
#include <main_window/dock/lvl_warp_props.h>
#include <main_window/dock/lvl_search_box.h>
#include <ui_lvl_item_properties.h>
#include <ui_lvl_warp_props.h>
#include <ui_lvl_search_box.h>

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
    ui->setupUi(this);

    QRect mwg = mw()->geometry();
    int GOffset=240;
    mw()->addDockWidget(Qt::RightDockWidgetArea, this);
    connect(mw(), SIGNAL(languageSwitched()), this, SLOT(re_translate()));
    setFloating(true);
    setGeometry(
                mwg.x()+mwg.width()-width()-GOffset,
                mwg.y()+120,
                width(),
                height()
                );

    connect(ui->LvlLayerList->model(), SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)),
            this, SLOT(DragAndDroppedLayer(QModelIndex,int,int,QModelIndex,int)));

    mw()->docks_level.
          addState(this, &GlobalSettings::LevelLayersBoxVis,
            &setVisible, &isVisible);

    lockLayerEdit=false;
}

LvlLayersBox::~LvlLayersBox()
{
    delete ui;
}

void LvlLayersBox::re_translate()
{
    ui->retranslateUi(this);
}

void LvlLayersBox::on_LvlLayersBox_visibilityChanged(bool visible)
{
    mw()->ui->actionLayersBox->setChecked(visible);
}

void MainWindow::on_actionLayersBox_triggered(bool checked)
{
    dock_LvlLayers->setVisible(checked);
    if(checked) dock_LvlLayers->raise();
}

void LvlLayersBox::setLayersBox()
{
    int WinType = mw()->activeChildWindow();
    QListWidgetItem * item;
    LevelEdit * edit = mw()->activeLvlEditWin();
    if(!edit) return;

    LvlPlacingItems::layer="";

    util::memclear(ui->LvlLayerList);

    if(WinType==1)
    {
        foreach(LevelLayers layer, edit->LvlData.layers)
        {
            item = new QListWidgetItem;
            item->setText(layer.name);
            item->setFlags(Qt::ItemIsUserCheckable);

            if((layer.name!="Destroyed Blocks")&&(layer.name!="Spawned NPCs"))
                item->setFlags(item->flags() | Qt::ItemIsEnabled);

            if(layer.name!="Default")
                item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsSelectable);

            item->setCheckState( (layer.hidden) ? Qt::Unchecked: Qt::Checked );
            item->setData(Qt::UserRole, QString::number( layer.array_id ) );
            ui->LvlLayerList->addItem( item );
        }
    }
}

void MainWindow::setLayerLists()
{
    dock_LvlItemProps->hide();
    dock_LvlItemProps->LvlItemPropsLock=true;
    dock_LvlWarpProps->lockWarpSetSettings=true;

    //save old entry from search!
    QString curSearchLayerBlock = dock_LvlSearchBox->ui->Find_Combo_LayerBlock->currentText();
    QString curSearchLayerBGO = dock_LvlSearchBox->ui->Find_Combo_LayerBGO->currentText();
    QString curSearchLayerNPC = dock_LvlSearchBox->ui->Find_Combo_LayerNPC->currentText();

    QString curWarpLayer = dock_LvlWarpProps->ui->WarpLayer->currentText();

    int WinType = activeChildWindow();
    dock_LvlItemProps->LvlItemPropsLock = true;
    LvlEventBoxLock = true;
    dock_LvlItemProps->ui->PROPS_BGOLayer->clear();
    dock_LvlItemProps->ui->PROPS_NpcLayer->clear();
    dock_LvlItemProps->ui->PROPS_BlockLayer->clear();
    dock_LvlItemProps->ui->PROPS_NpcAttachLayer->clear();
    dock_LvlWarpProps->ui->WarpLayer->clear();
    dock_LvlItemProps->ui->PROPS_NpcAttachLayer->addItem(tr("[None]"));
    ui->LVLEvent_LayerMov_List->clear();
    ui->LVLEvent_LayerMov_List->addItem(tr("[None]"));
    dock_LvlSearchBox->ui->Find_Combo_LayerBlock->clear();
    dock_LvlSearchBox->ui->Find_Combo_LayerBGO->clear();
    dock_LvlSearchBox->ui->Find_Combo_LayerNPC->clear();

    if (WinType==1)
    {
        foreach(LevelLayers layer, activeLvlEditWin()->LvlData.layers)
        {
            if((layer.name=="Destroyed Blocks")||(layer.name=="Spawned NPCs"))
                continue;
            dock_LvlItemProps->ui->PROPS_BGOLayer->addItem(layer.name);
            dock_LvlItemProps->ui->PROPS_NpcLayer->addItem(layer.name);
            dock_LvlItemProps->ui->PROPS_BlockLayer->addItem(layer.name);
            dock_LvlItemProps->ui->PROPS_NpcAttachLayer->addItem(layer.name);
            ui->LVLEvent_LayerMov_List->addItem(layer.name);
            dock_LvlWarpProps->ui->WarpLayer->addItem(layer.name, QVariant(layer.name));
            dock_LvlSearchBox->ui->Find_Combo_LayerBlock->addItem(layer.name);
            dock_LvlSearchBox->ui->Find_Combo_LayerBGO->addItem(layer.name);
            dock_LvlSearchBox->ui->Find_Combo_LayerNPC->addItem(layer.name);
        }
    }

    if(!curWarpLayer.isEmpty())
        dock_LvlWarpProps->ui->WarpLayer->setCurrentText(curWarpLayer);

    dock_LvlSearchBox->ui->Find_Combo_LayerBlock->setCurrentText(curSearchLayerBlock);
    dock_LvlSearchBox->ui->Find_Combo_LayerBGO->setCurrentText(curSearchLayerBGO);
    dock_LvlSearchBox->ui->Find_Combo_LayerNPC->setCurrentText(curSearchLayerNPC);
    dock_LvlWarpProps->lockWarpSetSettings = false;
    dock_LvlItemProps->LvlItemPropsLock = false;
    LvlEventBoxLock = false;
}

void LvlLayersBox::setLayerToolsLocked(bool locked)
{
    lockLayerEdit = locked;
}



void LvlLayersBox::RemoveCurrentLayer(bool moveToDefault)
{
    if(mw()->activeChildWindow()!=1) return;
    LevelEdit * edit = mw()->activeLvlEditWin();
    if(!edit) return;

    bool layerVisible = true;
    //Remove from List
    QList<QListWidgetItem * > selected = ui->LvlLayerList->selectedItems();

    //Sync layer name with events
    for(int j=0; j<edit->LvlData.events.size(); j++)
    {
        for(int i=0; i<edit->LvlData.events[j].layers_hide.size(); i++)
        {
            if(edit->LvlData.events[j].layers_hide[i] == selected[0]->text()) edit->LvlData.events[j].layers_hide.removeAt(i);
        }
        for(int i=0; i<edit->LvlData.events[j].layers_show.size(); i++)
        {
            if(edit->LvlData.events[j].layers_show[i] == selected[0]->text()) edit->LvlData.events[j].layers_show.removeAt(i);
        }
        for(int i=0; i<edit->LvlData.events[j].layers_toggle.size(); i++)
        {
            if(edit->LvlData.events[j].layers_toggle[i] == selected[0]->text()) edit->LvlData.events[j].layers_toggle.removeAt(i);
        }
        if(edit->LvlData.events[j].movelayer == selected[0]->text() ) edit->LvlData.events[j].movelayer = "";
    }

    mw()->setEventsBox(); //Refresh events


    if(moveToDefault)
    { //Find default layer visibly
        foreach(LevelLayers layer, edit->LvlData.layers)
        {
            if( layer.name=="Default" )
            {
                layerVisible=!layer.hidden;
                break;
            }
        }
    }

    if(moveToDefault)
    {
        ModifyLayer(selected[0]->text(), "Default", layerVisible, 0);
    }
    else
    {
        RemoveLayerItems(selected[0]->text());
    }

    if(selected.isEmpty()) return;

    RemoveLayerFromListAndData(selected[0]);
}


void LvlLayersBox::RemoveLayerItems(QString layerName)
{
    LevelEdit * edit = mw()->activeLvlEditWin();
    if(!edit) return;

    QList<QGraphicsItem*> ItemList = edit->scene->items();
    LevelData delData;
    for (QList<QGraphicsItem*>::iterator it = ItemList.begin(); it != ItemList.end(); it++)
    {
        if((*it)->data(ITEM_IS_CURSOR).toString()=="CURSOR") continue; //skip cursor item

        if((*it)->data(ITEM_TYPE).toString()=="Block")
        {
            if(((ItemBlock *)(*it))->blockData.layer==layerName)
            {
                delData.blocks.push_back(((ItemBlock *)(*it))->blockData);
                ((ItemBlock *)(*it))->removeFromArray();
                delete (*it);
                //activeLvlEditWin()->scene->removeItem((*it));
            }

        }
        else
        if((*it)->data(ITEM_TYPE).toString()=="BGO")
        {
            if(((ItemBGO *)(*it))->bgoData.layer==layerName)
            {
                delData.bgo.push_back(((ItemBGO *)(*it))->bgoData);
                ((ItemBGO *)(*it))->removeFromArray();
                delete (*it);
                //activeLvlEditWin()->scene->removeItem((*it));
            }
        }
        else
        if((*it)->data(ITEM_TYPE).toString()=="NPC")
        {
            if(((ItemNPC *)(*it))->npcData.layer==layerName)
            {
                delData.npc.push_back(((ItemNPC *)(*it))->npcData);
                ((ItemNPC *)(*it))->removeFromArray();
                delete (*it);
                //activeLvlEditWin()->scene->removeItem((*it));
            }
        }
        else
        if((*it)->data(ITEM_TYPE).toString()=="Water")
        {
            if(((ItemWater *)(*it))->waterData.layer==layerName)
            {
                delData.physez.push_back(((ItemWater *)(*it))->waterData);
                ((ItemWater *)(*it))->removeFromArray();
                delete (*it);
                //activeLvlEditWin()->scene->removeItem((*it));
            }
        }
        else
        if(((*it)->data(ITEM_TYPE).toString()=="Door_enter")||((*it)->data(ITEM_TYPE).toString()=="Door_exit"))
        {
            if(((ItemDoor *)(*it))->doorData.layer==layerName)
            {
                if(((*it)->data(0).toString()=="Door_enter")){
                    LevelDoors tData = ((ItemDoor *)(*it))->doorData;
                    tData.isSetIn = true;
                    tData.isSetOut = false;
                    delData.doors.push_back(tData);
                }
                else
                if(((*it)->data(0).toString()=="Door_exit")){
                    LevelDoors tData = ((ItemDoor *)(*it))->doorData;
                    tData.isSetIn = false;
                    tData.isSetOut = true;
                    delData.doors.push_back(tData);
                }
                ((ItemDoor *)(*it))->removeFromArray();
                delete (*it);
                //activeLvlEditWin()->scene->removeItem((*it));
            }
        }
    }
    foreach (LevelLayers l, edit->LvlData.layers) {
        if(l.name == layerName){
            delData.layers.push_back(l);
            break;
        }
    }
    edit->scene->addRemoveLayerHistory(delData);

    mw()->setLayerLists();  //Sync comboboxes in properties
}

void LvlLayersBox::RemoveLayerFromListAndData(QListWidgetItem *layerItem)
{
    LevelEdit * edit = mw()->activeLvlEditWin();
    if(!edit) return;

    if(layerItem->text()=="Destroyed Blocks") return;
    if(layerItem->text()=="Spawned NPCs") return;
    if(layerItem->text()=="Default") return;

    int WinType = mw()->activeChildWindow();

    if (WinType==1)
    {
        for(int i=0;i< edit->LvlData.layers.size(); i++)
        {
            if( edit->LvlData.layers[i].array_id==(unsigned int)layerItem->data(Qt::UserRole).toInt() )
            {
                edit->LvlData.layers.remove(i);
                delete layerItem;
                break;
            }
        }
    }
    mw()->setLayerLists();  //Sync comboboxes in properties
}

void LvlLayersBox::ModifyLayer(QString layerName, bool visible)
{
    //Apply layer's visibly to all items
    LevelEdit * edit = mw()->activeLvlEditWin();
    if(!edit) return;

    QList<QGraphicsItem*> ItemList = edit->scene->items();

    for (QList<QGraphicsItem*>::iterator it = ItemList.begin(); it != ItemList.end(); it++)
    {
        if((*it)->data(ITEM_IS_CURSOR).toString()=="CURSOR") continue; //skip cursor item

        if((*it)->data(ITEM_TYPE).toString()=="Block")
        {
            if(((ItemBlock *)(*it))->blockData.layer==layerName)
            {
                (*it)->setVisible(visible);
            }

        }
        else
        if((*it)->data(ITEM_TYPE).toString()=="BGO")
        {
            if(((ItemBGO *)(*it))->bgoData.layer==layerName)
            {
                (*it)->setVisible(visible);
            }
        }
        else
        if((*it)->data(ITEM_TYPE).toString()=="NPC")
        {
            if(((ItemNPC *)(*it))->npcData.layer==layerName)
            {
                (*it)->setVisible(visible);
            }
        }
        else
        if((*it)->data(ITEM_TYPE).toString()=="Water")
        {
            if(((ItemWater *)(*it))->waterData.layer==layerName)
            {
                (*it)->setVisible(visible);
            }
        }
        else
        if(((*it)->data(ITEM_TYPE).toString()=="Door_enter")||((*it)->data(ITEM_TYPE).toString()=="Door_exit"))
        {
            if(((ItemDoor *)(*it))->doorData.layer==layerName)
            {
                (*it)->setVisible(visible);
            }
        }
    }
    mw()->setLayerLists();  //Sync comboboxes in properties
}

void LvlLayersBox::ModifyLayer(QString layerName, QString newLayerName)
{
    //Apply layer's name to all items
    LevelEdit * edit = mw()->activeLvlEditWin();
    if(!edit) return;

    QList<QGraphicsItem*> ItemList = edit->scene->items();

    for (QList<QGraphicsItem*>::iterator it = ItemList.begin(); it != ItemList.end(); it++)
    {
        if((*it)->data(ITEM_IS_CURSOR).toString()=="CURSOR") continue; //skip cursor item

        if((*it)->data(ITEM_TYPE).toString()=="Block")
        {
            if(((ItemBlock *)(*it))->blockData.layer==layerName)
            {
                ((ItemBlock *)(*it))->blockData.layer = newLayerName;
                ((ItemBlock *)(*it))->arrayApply();
            }
        }
        else
        if((*it)->data(ITEM_TYPE).toString()=="BGO")
        {
            if(((ItemBGO *)(*it))->bgoData.layer==layerName)
            {
                ((ItemBGO *)(*it))->bgoData.layer = newLayerName;
                ((ItemBGO *)(*it))->arrayApply();
            }
        }
        else
        if((*it)->data(ITEM_TYPE).toString()=="NPC")
        {
            if(((ItemNPC *)(*it))->npcData.layer==layerName)
            {
                ((ItemNPC *)(*it))->npcData.layer = newLayerName;
                ((ItemNPC *)(*it))->arrayApply();
            }
        }
        else
        if((*it)->data(ITEM_TYPE).toString()=="Water")
        {
            if(((ItemWater *)(*it))->waterData.layer==layerName)
            {
                ((ItemWater *)(*it))->waterData.layer = newLayerName;
                ((ItemWater *)(*it))->arrayApply();
            }
        }
        else
        if(((*it)->data(ITEM_TYPE).toString()=="Door_enter")||((*it)->data(ITEM_TYPE).toString()=="Door_exit"))
        {
            if(((ItemDoor *)(*it))->doorData.layer==layerName)
            {
                ((ItemDoor *)(*it))->doorData.layer = newLayerName;
                ((ItemDoor *)(*it))->arrayApply();
            }
        }
    }

    //Sync layer name with events
    for(int j=0; j<edit->LvlData.events.size(); j++)
    {
        for(int i=0; i<edit->LvlData.events[j].layers_hide.size(); i++)
        {
            if(edit->LvlData.events[j].layers_hide[i] == layerName) edit->LvlData.events[j].layers_hide[i] = newLayerName;
        }
        for(int i=0; i<edit->LvlData.events[j].layers_show.size(); i++)
        {
            if(edit->LvlData.events[j].layers_show[i] == layerName) edit->LvlData.events[j].layers_show[i] = newLayerName;
        }
        for(int i=0; i<edit->LvlData.events[j].layers_toggle.size(); i++)
        {
            if(edit->LvlData.events[j].layers_toggle[i] == layerName) edit->LvlData.events[j].layers_toggle[i] = newLayerName;
        }
        if(edit->LvlData.events[j].movelayer == layerName ) edit->LvlData.events[j].movelayer = newLayerName;
    }
    mw()->setEventsBox(); //Refresh events

    mw()->setLayerLists();  //Sync comboboxes in properties
}

void LvlLayersBox::ModifyLayer(QString layerName, QString newLayerName, bool visible, int historyRecord)
{
    //Apply layer's name/visibly to all items
    LevelEdit * edit = mw()->activeLvlEditWin();
    if(!edit) return;

    QList<QGraphicsItem*> ItemList = edit->scene->items();


    LevelData modData;
    for (QList<QGraphicsItem*>::iterator it = ItemList.begin(); it != ItemList.end(); it++)
    {
        if((*it)->data(ITEM_IS_CURSOR).toString()=="CURSOR") continue; //skip cursor item

        if((*it)->data(ITEM_TYPE).toString()=="Block")
        {
            if(((ItemBlock *)(*it))->blockData.layer==layerName)
            {
                modData.blocks.push_back(((ItemBlock *)(*it))->blockData);
                ((ItemBlock *)(*it))->blockData.layer = newLayerName;
                (*it)->setVisible(visible);
                ((ItemBlock *)(*it))->arrayApply();
            }

        }
        else
        if((*it)->data(ITEM_TYPE).toString()=="BGO")
        {
            if(((ItemBGO *)(*it))->bgoData.layer==layerName)
            {
                modData.bgo.push_back(((ItemBGO *)(*it))->bgoData);
                ((ItemBGO *)(*it))->bgoData.layer = newLayerName;
                (*it)->setVisible(visible);
                ((ItemBGO *)(*it))->arrayApply();

            }
        }
        else
        if((*it)->data(ITEM_TYPE).toString()=="NPC")
        {
            if(((ItemNPC *)(*it))->npcData.layer==layerName)
            {
                modData.npc.push_back(((ItemNPC *)(*it))->npcData);
                ((ItemNPC *)(*it))->npcData.layer = newLayerName;
                (*it)->setVisible(visible);
                ((ItemNPC *)(*it))->arrayApply();
            }
        }
        else
        if((*it)->data(ITEM_TYPE).toString()=="Water")
        {
            if(((ItemWater *)(*it))->waterData.layer==layerName)
            {
                modData.physez.push_back(((ItemWater *)(*it))->waterData);
                ((ItemWater *)(*it))->waterData.layer = newLayerName;
                (*it)->setVisible(visible);
                ((ItemWater *)(*it))->arrayApply();
            }
        }
        else
        if(((*it)->data(ITEM_TYPE).toString()=="Door_enter")||((*it)->data(ITEM_TYPE).toString()=="Door_exit"))
        {
            if(((ItemDoor *)(*it))->doorData.layer==layerName)
            {
                if((*it)->data(ITEM_TYPE).toString()=="Door_enter"){
                    LevelDoors tData = ((ItemDoor *)(*it))->doorData;
                    tData.isSetIn = true;
                    tData.isSetOut = false;
                    modData.doors.push_back(tData);
                }else if((*it)->data(ITEM_TYPE).toString()=="Door_exit"){
                    LevelDoors tData = ((ItemDoor *)(*it))->doorData;
                    tData.isSetIn = false;
                    tData.isSetOut = true;
                    modData.doors.push_back(tData);
                }
                ((ItemDoor *)(*it))->doorData.layer = newLayerName;
                (*it)->setVisible(visible);
                ((ItemDoor *)(*it))->arrayApply();
            }
        }
    }

   //Apply new layer name for non-placed warps
   for(int i=0; i<edit->LvlData.doors.size();i++)
   {
       if(edit->LvlData.doors[i].layer==layerName)
            edit->LvlData.doors[i].layer = newLayerName;
   }

    if(historyRecord == 0){
        if(newLayerName == "Default"){
            for(int i = 0; i < edit->LvlData.layers.size(); i++){
                if(edit->LvlData.layers[i].name == layerName){
                    modData.layers.push_back(edit->LvlData.layers[i]);
                    edit->scene->addRemoveLayerAndSaveItemsHistory(modData);
                    break;
                }
            }
        }
    }
    else
    if(historyRecord == 1){
        for(int i = 0; i < edit->LvlData.layers.size(); i++){
            if(edit->LvlData.layers[i].name == layerName){
                modData.layers.push_back(edit->LvlData.layers[i]);
                edit->scene->addMergeLayer(modData, newLayerName);
                break;
            }
        }
    }


    //Sync layer name with events
    for(int j=0; j<edit->LvlData.events.size(); j++)
    {
        for(int i=0; i<edit->LvlData.events[j].layers_hide.size(); i++)
        {
            if(edit->LvlData.events[j].layers_hide[i] == layerName) edit->LvlData.events[j].layers_hide[i] = newLayerName;
        }
        for(int i=0; i<edit->LvlData.events[j].layers_show.size(); i++)
        {
            if(edit->LvlData.events[j].layers_show[i] == layerName) edit->LvlData.events[j].layers_show[i] = newLayerName;
        }
        for(int i=0; i<edit->LvlData.events[j].layers_toggle.size(); i++)
        {
            if(edit->LvlData.events[j].layers_toggle[i] == layerName) edit->LvlData.events[j].layers_toggle[i] = newLayerName;
        }
        if(edit->LvlData.events[j].movelayer == layerName ) edit->LvlData.events[j].movelayer = newLayerName;
    }
    mw()->setEventsBox(); //Refresh events
    mw()->setLayerLists();  //Sync comboboxes in properties
}



void LvlLayersBox::AddNewLayer(QString layerName, bool setEdited)
{
    LevelEdit * edit = mw()->activeLvlEditWin();
    if(!edit) return;

    QListWidgetItem * item;
    item = new QListWidgetItem;
    item->setText(layerName);
    item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEditable);
    item->setFlags(item->flags() | Qt::ItemIsEnabled);
    item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsSelectable);
    item->setCheckState( Qt::Checked );
    item->setData(Qt::UserRole, QString("NewLayer") );
    ui->LvlLayerList->addItem( item );

    if(setEdited)
    {
        on_LvlLayerList_itemChanged(item);
        if(item)
        {
            ui->LvlLayerList->setFocus();
            ui->LvlLayerList->scrollToItem( item );
            ui->LvlLayerList->editItem( item );
        }
    }
    else
    {
        bool AlreadyExist=false;
        foreach(LevelLayers layer, edit->LvlData.layers)
        {
            if( layer.name==item->text() )
            {
                AlreadyExist=true;
                break;
            }
        }

        if(AlreadyExist)
        {
            delete item;
            return;
        }
        else
        {
            LevelLayers NewLayer;
            NewLayer.name = item->text();
            NewLayer.hidden = (item->checkState() == Qt::Unchecked );
            edit->LvlData.layers_array_id++;
            NewLayer.array_id = edit->LvlData.layers_array_id;
            edit->scene->addAddLayerHistory(NewLayer.array_id, NewLayer.name);

            item->setData(Qt::UserRole, QString::number(NewLayer.array_id));

            edit->LvlData.layers.push_back(NewLayer);
            edit->LvlData.modified=true;
        }
    }
    mw()->setLayerLists();  //Sync comboboxes in properties
}

void LvlLayersBox::ModifyLayerItem(QListWidgetItem *item, QString oldLayerName, QString newLayerName, bool visible)
{
    //Find layer enrty in array and apply settings
    LevelEdit * edit = mw()->activeLvlEditWin();
    if(!edit) return;
    for(int i=0; i < edit->LvlData.layers.size(); i++)
    {
        if( edit->LvlData.layers[i].array_id==(unsigned int)item->data(Qt::UserRole).toInt() )
        {
            int l=0;
            bool merge=false;
            bool exist=false;
            oldLayerName = edit->LvlData.layers[i].name;

            if(newLayerName.isEmpty())
            {   //Discard change to empty
                lockLayerEdit=true;
                   item->setText(oldLayerName);
                lockLayerEdit=false;
                return;
            }

            if(oldLayerName!=newLayerName)
            {
                //Check for exists item equal to new item
                for(l=0;l<edit->LvlData.layers.size();l++)
                {
                    if(edit->LvlData.layers[l].name==newLayerName)
                    {
                        exist=true;
                        QMessageBox::StandardButton reply;
                        reply = QMessageBox::question(this, tr("Layers merge"),
                        tr("Layer with name '%1' already exist, do you want to merge layers?").arg(newLayerName),
                        QMessageBox::Yes|QMessageBox::No);
                        if (reply == QMessageBox::Yes) { merge=true;}
                        break;
                    }
                }
            }

            if(exist)
            {
                if(merge)
                {  //Merge layers
                    lockLayerEdit=true;
                        edit->LvlData.layers[l].name = newLayerName;
                        edit->LvlData.layers[l].hidden = !visible;
                        ModifyLayer(newLayerName, visible);
                        ModifyLayer(oldLayerName, newLayerName, visible, 1);
                        delete item;
                        edit->LvlData.layers.remove(i);
                        mw()->setLayerLists();  //Sync comboboxes in properties
                        setLayersBox();
                    lockLayerEdit=false;
                    return;
                }
                else
                { //cancel renaming
                 lockLayerEdit=true;
                    item->setText(oldLayerName);
                 lockLayerEdit=false;
                 return;
                }
            }
            else
            {
                edit->scene->addRenameLayerHistory(edit->LvlData.layers[i].array_id, oldLayerName, newLayerName);
                edit->LvlData.layers[i].name = newLayerName;
                edit->LvlData.layers[i].hidden = !visible;
            }

            break;
        }
    }
    //Apply layer's name/visibly to all items
    ModifyLayer(oldLayerName, newLayerName, visible);

    mw()->setLayerLists();  //Sync comboboxes in properties
    mw()->dock_LvlWarpProps->setDoorData(-2);
}

void LvlLayersBox::DragAndDroppedLayer(QModelIndex /*sourceParent*/,int sourceStart,int sourceEnd,QModelIndex /*destinationParent*/,int destinationRow)
{
    LevelEdit * edit = mw()->activeLvlEditWin();
    if(!edit) return;

    WriteToLog(QtDebugMsg, "Row Change at " + QString::number(sourceStart) +
               " " + QString::number(sourceEnd) +
               " to " + QString::number(destinationRow));

    int WinType = mw()->activeChildWindow();
    if (WinType==1)
    {
        LevelLayers buffer;
        if(sourceStart<edit->LvlData.layers.size())
        {
            buffer = edit->LvlData.layers[sourceStart];
            edit->LvlData.layers.remove(sourceStart);
            edit->LvlData.layers.insert(((destinationRow>sourceStart)?destinationRow-1:destinationRow), buffer);
        }
    }

    mw()->setLayerLists(); //Sync comboboxes in properties
}


void LvlLayersBox::on_AddLayer_clicked()
{
    AddNewLayer(tr("New Layer %1").arg( ui->LvlLayerList->count()+1 ), true);
}


void LvlLayersBox::on_LvlLayerList_itemClicked(QListWidgetItem *item)
{
    int WinType = mw()->activeChildWindow();
    int itemType=0;
    bool allow = ( (WinType==1) && mw()->activeLvlEditWin()->sceneCreated );

    if(allow) itemType = mw()->activeLvlEditWin()->scene->placingItem;

    if( (allow) && (itemType==LvlScene::PLC_Block ||
                    itemType==LvlScene::PLC_BGO ||
                    itemType==LvlScene::PLC_NPC ||
                    itemType==LvlScene::PLC_Water) )
        LvlPlacingItems::layer=item->text();
    else
        LvlPlacingItems::layer="";
    LvlPlacingItems::blockSet.layer = LvlPlacingItems::layer;
    LvlPlacingItems::bgoSet.layer = LvlPlacingItems::layer;
    LvlPlacingItems::npcSet.layer = LvlPlacingItems::layer;
    LvlPlacingItems::waterSet.layer = LvlPlacingItems::layer;
    mw()->dock_LvlItemProps->LvlItemProps_updateLayer(LvlPlacingItems::layer);
}

void LvlLayersBox::on_LvlLayerList_itemSelectionChanged()
{
    if(ui->LvlLayerList->selectedItems().isEmpty())
        LvlPlacingItems::layer="";
    else
        LvlPlacingItems::layer= ui->LvlLayerList->selectedItems().first()->text();

    LvlPlacingItems::blockSet.layer = LvlPlacingItems::layer;
    LvlPlacingItems::bgoSet.layer = LvlPlacingItems::layer;
    LvlPlacingItems::npcSet.layer = LvlPlacingItems::layer;
}


void LvlLayersBox::on_LvlLayerList_itemChanged(QListWidgetItem *item)
{
    if(lockLayerEdit) return;
    int WinType = mw()->activeChildWindow();

    LevelEdit * edit = mw()->activeLvlEditWin();
    if(!edit) return;

    if (WinType==1)
    {
        if(item->data(Qt::UserRole).toString()=="NewLayer")
        {
            bool AlreadyExist=false;
            foreach(LevelLayers layer, edit->LvlData.layers)
            {
                if( layer.name==item->text() )
                {
                    AlreadyExist=true;
                    break;
                }
            }

            if(AlreadyExist)
            {
                delete item;
                return;
            }
            else
            {
                LevelLayers NewLayer;
                NewLayer.name = item->text();
                NewLayer.hidden = (item->checkState() == Qt::Unchecked );
                edit->LvlData.layers_array_id++;
                NewLayer.array_id = edit->LvlData.layers_array_id;
                edit->scene->addAddLayerHistory(NewLayer.array_id, NewLayer.name);

                item->setData(Qt::UserRole, QString::number(NewLayer.array_id));

                edit->LvlData.layers.push_back(NewLayer);
                edit->LvlData.modified=true;
            }

        }//if(item->data(Qt::UserRole).toString()=="NewLayer")
        else
        {
            QString layerName = item->text();
            QString oldLayerName = item->text();
            //unsigned int layerArId = (unsigned int)item->data(Qt::UserRole).toInt();
            bool layerVisible = (item->checkState()==Qt::Checked);

            ModifyLayerItem(item, oldLayerName, layerName, layerVisible);

            edit->LvlData.modified=true;
        }

    }//if WinType==1
}


void LvlLayersBox::on_RemoveLayer_clicked()
{
    if(ui->LvlLayerList->selectedItems().isEmpty()) return;
    RemoveCurrentLayer(true);
}

void LvlLayersBox::on_LvlLayerList_customContextMenuRequested(const QPoint &pos)
{
    if(ui->LvlLayerList->selectedItems().isEmpty()) return;

    QPoint globPos = ui->LvlLayerList->mapToGlobal(pos);

    WriteToLog(QtDebugMsg, QString("Main Menu's context menu called! %1 %2 -> %3 %4")
               .arg(pos.x()).arg(pos.y())
               .arg(globPos.x()).arg(globPos.y()));

    QMenu *layer_menu = new QMenu(this);
    QAction * rename = layer_menu->addAction(tr("Rename layer"));

    layer_menu->addSeparator();

    QAction * removeLayer = layer_menu->addAction(tr("Remove layer with items"));
    QAction * removeLayerOnly = layer_menu->addAction(tr("Remove layer and save items"));

    QAction *selected = layer_menu->exec( globPos );
    if(selected==rename)
    {
        ui->LvlLayerList->editItem(ui->LvlLayerList->selectedItems()[0]);
    }
    else
    if((selected==removeLayer)||(selected==removeLayerOnly))
    {
        if(selected==removeLayerOnly){
            RemoveCurrentLayer(true);
        }else
        if(selected==removeLayer){
            RemoveCurrentLayer(false);
        }
    }
}
