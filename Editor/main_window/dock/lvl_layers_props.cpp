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

#include "../../ui_mainwindow.h"
#include "../../mainwindow.h"

#include "../../file_formats/file_formats.h"

#include "../../level_scene/item_bgo.h"
#include "../../level_scene/item_block.h"
#include "../../level_scene/item_npc.h"
#include "../../level_scene/item_water.h"
#include "../../level_scene/item_door.h"

#include "../../common_features/util.h"


static bool lockLayerEdit=false;

void MainWindow::setLayersBox()
{
    int WinType = activeChildWindow();
    QListWidgetItem * item;


    util::memclear(ui->LvlLayerList);

    if (WinType==1)
    {
        foreach(LevelLayers layer, activeLvlEditWin()->LvlData.layers)
        {
            item = new QListWidgetItem;
            item->setText(layer.name);
            item->setFlags(Qt::ItemIsUserCheckable);

            if((layer.name!="Destroyed Blocks")&&(layer.name!="Spawned NPCs"))
                item->setFlags(item->flags() | Qt::ItemIsEnabled);

            if(layer.name!="Default")
                item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsSelectable);

            item->setCheckState( (layer.hidden) ? Qt::Unchecked: Qt::Checked );
            item->setData(3, QString::number( layer.array_id ) );
            ui->LvlLayerList->addItem( item );
        }

    }
}

void MainWindow::setLayerLists()
{
    ui->ItemProperties->hide();
    LvlItemPropsLock=true;

    //save old entry from search!
    QString curSearchLayerBlock = ui->Find_Combo_LayerBlock->currentText();
    QString curSearchLayerBGO = ui->Find_Combo_LayerBGO->currentText();
    QString curSearchLayerNPC = ui->Find_Combo_LayerNPC->currentText();

    int WinType = activeChildWindow();
    LvlItemPropsLock = true;
    ui->PROPS_BGOLayer->clear();
    ui->PROPS_NpcLayer->clear();
    ui->PROPS_BlockLayer->clear();
    ui->PROPS_NpcAttachLayer->clear();
    ui->PROPS_NpcAttachLayer->addItem(tr("[None]"));
    ui->LVLEvent_LayerMov_List->clear();
    ui->LVLEvent_LayerMov_List->addItem(tr("[None]"));
    ui->Find_Combo_LayerBlock->clear();
    ui->Find_Combo_LayerBGO->clear();
    ui->Find_Combo_LayerNPC->clear();

    if (WinType==1)
    {
        foreach(LevelLayers layer, activeLvlEditWin()->LvlData.layers)
        {
            if((layer.name=="Destroyed Blocks")||(layer.name=="Spawned NPCs"))
                continue;
            ui->PROPS_BGOLayer->addItem(layer.name);
            ui->PROPS_NpcLayer->addItem(layer.name);
            ui->PROPS_BlockLayer->addItem(layer.name);
            ui->PROPS_NpcAttachLayer->addItem(layer.name);
            ui->LVLEvent_LayerMov_List->addItem(layer.name);
            ui->Find_Combo_LayerBlock->addItem(layer.name);
            ui->Find_Combo_LayerBGO->addItem(layer.name);
            ui->Find_Combo_LayerNPC->addItem(layer.name);
        }
    }

    ui->Find_Combo_LayerBlock->setCurrentText(curSearchLayerBlock);
    ui->Find_Combo_LayerBGO->setCurrentText(curSearchLayerBGO);
    ui->Find_Combo_LayerNPC->setCurrentText(curSearchLayerNPC);
    LvlItemPropsLock = false;

}

void MainWindow::setLayerToolsLocked(bool locked)
{
    lockLayerEdit = locked;
}





void MainWindow::RemoveCurrentLayer(bool moveToDefault)
{
    if(activeChildWindow()!=1) return;

    bool layerVisible = true;
    //Remove from List
    QList<QListWidgetItem * > selected = ui->LvlLayerList->selectedItems();

    leveledit * edit = activeLvlEditWin();

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
    setEventsBox(); //Refresh events


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

void MainWindow::RemoveLayerItems(QString layerName)
{
    QList<QGraphicsItem*> ItemList = activeLvlEditWin()->scene->items();
    LevelData delData;
    for (QList<QGraphicsItem*>::iterator it = ItemList.begin(); it != ItemList.end(); it++)
    {
        if((*it)->data(25).toString()=="CURSOR") continue; //skip cursor item

        if((*it)->data(0).toString()=="Block")
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
        if((*it)->data(0).toString()=="BGO")
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
        if((*it)->data(0).toString()=="NPC")
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
        if((*it)->data(0).toString()=="Water")
        {
            if(((ItemWater *)(*it))->waterData.layer==layerName)
            {
                delData.water.push_back(((ItemWater *)(*it))->waterData);
                ((ItemWater *)(*it))->removeFromArray();
                delete (*it);
                //activeLvlEditWin()->scene->removeItem((*it));
            }
        }
        else
        if(((*it)->data(0).toString()=="Door_enter")||((*it)->data(0).toString()=="Door_exit"))
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
    foreach (LevelLayers l, activeLvlEditWin()->LvlData.layers) {
        if(l.name == layerName){
            delData.layers.push_back(l);
            break;
        }
    }
    activeLvlEditWin()->scene->addRemoveLayerHistory(delData);

    setLayerLists();  //Sync comboboxes in properties
}

void MainWindow::RemoveLayerFromListAndData(QListWidgetItem *layerItem)
{

    if(layerItem->text()=="Destroyed Blocks") return;
    if(layerItem->text()=="Spawned NPCs") return;
    if(layerItem->text()=="Default") return;

    int WinType = activeChildWindow();

    if (WinType==1)
    {
        for(int i=0;i< activeLvlEditWin()->LvlData.layers.size(); i++)
        {
            if( activeLvlEditWin()->LvlData.layers[i].array_id==(unsigned int)layerItem->data(3).toInt() )
            {
                activeLvlEditWin()->LvlData.layers.remove(i);
                delete layerItem;
                break;
            }
        }
    }
    setLayerLists();  //Sync comboboxes in properties
}

void MainWindow::ModifyLayer(QString layerName, bool visible)
{
    //Apply layer's visibly to all items
    leveledit * edit = activeLvlEditWin();
    QList<QGraphicsItem*> ItemList = edit->scene->items();

    for (QList<QGraphicsItem*>::iterator it = ItemList.begin(); it != ItemList.end(); it++)
    {
        if((*it)->data(25).toString()=="CURSOR") continue; //skip cursor item

        if((*it)->data(0).toString()=="Block")
        {
            if(((ItemBlock *)(*it))->blockData.layer==layerName)
            {
                (*it)->setVisible(visible);
            }

        }
        else
        if((*it)->data(0).toString()=="BGO")
        {
            if(((ItemBGO *)(*it))->bgoData.layer==layerName)
            {
                (*it)->setVisible(visible);
            }
        }
        else
        if((*it)->data(0).toString()=="NPC")
        {
            if(((ItemNPC *)(*it))->npcData.layer==layerName)
            {
                (*it)->setVisible(visible);
            }
        }
        else
        if((*it)->data(0).toString()=="Water")
        {
            if(((ItemWater *)(*it))->waterData.layer==layerName)
            {
                (*it)->setVisible(visible);
            }
        }
        else
        if(((*it)->data(0).toString()=="Door_enter")||((*it)->data(0).toString()=="Door_exit"))
        {
            if(((ItemDoor *)(*it))->doorData.layer==layerName)
            {
                (*it)->setVisible(visible);
            }
        }
    }
    setLayerLists();  //Sync comboboxes in properties
}

void MainWindow::ModifyLayer(QString layerName, QString newLayerName)
{
    //Apply layer's name to all items
    leveledit * edit = activeLvlEditWin();
    QList<QGraphicsItem*> ItemList = edit->scene->items();

    for (QList<QGraphicsItem*>::iterator it = ItemList.begin(); it != ItemList.end(); it++)
    {
        if((*it)->data(25).toString()=="CURSOR") continue; //skip cursor item

        if((*it)->data(0).toString()=="Block")
        {
            if(((ItemBlock *)(*it))->blockData.layer==layerName)
            {
                ((ItemBlock *)(*it))->blockData.layer = newLayerName;
                ((ItemBlock *)(*it))->arrayApply();
            }
        }
        else
        if((*it)->data(0).toString()=="BGO")
        {
            if(((ItemBGO *)(*it))->bgoData.layer==layerName)
            {
                ((ItemBGO *)(*it))->bgoData.layer = newLayerName;
                ((ItemBGO *)(*it))->arrayApply();
            }
        }
        else
        if((*it)->data(0).toString()=="NPC")
        {
            if(((ItemNPC *)(*it))->npcData.layer==layerName)
            {
                ((ItemNPC *)(*it))->npcData.layer = newLayerName;
                ((ItemNPC *)(*it))->arrayApply();
            }
        }
        else
        if((*it)->data(0).toString()=="Water")
        {
            if(((ItemWater *)(*it))->waterData.layer==layerName)
            {
                ((ItemWater *)(*it))->waterData.layer = newLayerName;
                ((ItemWater *)(*it))->arrayApply();
            }
        }
        else
        if(((*it)->data(0).toString()=="Door_enter")||((*it)->data(0).toString()=="Door_exit"))
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
    setEventsBox(); //Refresh events

    setLayerLists();  //Sync comboboxes in properties
}

void MainWindow::ModifyLayer(QString layerName, QString newLayerName, bool visible, int historyRecord)
{
    //Apply layer's name/visibly to all items
    leveledit * edit = activeLvlEditWin();
    QList<QGraphicsItem*> ItemList = edit->scene->items();


    LevelData modData;
    for (QList<QGraphicsItem*>::iterator it = ItemList.begin(); it != ItemList.end(); it++)
    {
        if((*it)->data(25).toString()=="CURSOR") continue; //skip cursor item

        if((*it)->data(0).toString()=="Block")
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
        if((*it)->data(0).toString()=="BGO")
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
        if((*it)->data(0).toString()=="NPC")
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
        if((*it)->data(0).toString()=="Water")
        {
            if(((ItemWater *)(*it))->waterData.layer==layerName)
            {
                modData.water.push_back(((ItemWater *)(*it))->waterData);
                ((ItemWater *)(*it))->waterData.layer = newLayerName;
                (*it)->setVisible(visible);
                ((ItemWater *)(*it))->arrayApply();
            }
        }
        else
        if(((*it)->data(0).toString()=="Door_enter")||((*it)->data(0).toString()=="Door_exit"))
        {
            if(((ItemDoor *)(*it))->doorData.layer==layerName)
            {
                if((*it)->data(0).toString()=="Door_enter"){
                    LevelDoors tData = ((ItemDoor *)(*it))->doorData;
                    tData.isSetIn = true;
                    tData.isSetOut = false;
                    modData.doors.push_back(tData);
                }else if((*it)->data(0).toString()=="Door_exit"){
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
    setEventsBox(); //Refresh events

    setLayerLists();  //Sync comboboxes in properties
}

void MainWindow::AddNewLayer(QString layerName, bool setEdited)
{
    QListWidgetItem * item;
    item = new QListWidgetItem;
    item->setText(layerName);
    item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEditable);
    item->setFlags(item->flags() | Qt::ItemIsEnabled);
    item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsSelectable);
    item->setCheckState( Qt::Checked );
    item->setData(3, QString("NewLayer") );
    ui->LvlLayerList->addItem( item );

    if(setEdited)
    {
        ui->LvlLayerList->setFocus();
        ui->LvlLayerList->scrollToItem( item );
        ui->LvlLayerList->editItem( item );
    }
    else
    {
        bool AlreadyExist=false;
        foreach(LevelLayers layer, activeLvlEditWin()->LvlData.layers)
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
            activeLvlEditWin()->LvlData.layers_array_id++;
            NewLayer.array_id = activeLvlEditWin()->LvlData.layers_array_id;
            activeLvlEditWin()->scene->addAddLayerHistory(NewLayer.array_id, NewLayer.name);

            item->setData(3, QString::number(NewLayer.array_id));

            activeLvlEditWin()->LvlData.layers.push_back(NewLayer);
            activeLvlEditWin()->LvlData.modified=true;
        }
    }

    setLayerLists();  //Sync comboboxes in properties

}

void MainWindow::ModifyLayerItem(QListWidgetItem *item, QString oldLayerName, QString newLayerName, bool visible)
{
    //Find layer enrty in array and apply settings
    leveledit * edit = activeLvlEditWin();
    for(int i=0; i < edit->LvlData.layers.size(); i++)
    {
        if( edit->LvlData.layers[i].array_id==(unsigned int)item->data(3).toInt() )
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
                        setLayerLists();  //Sync comboboxes in properties
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

    setLayerLists();  //Sync comboboxes in properties
}

void MainWindow::DragAndDroppedLayer(QModelIndex /*sourceParent*/,int sourceStart,int sourceEnd,QModelIndex /*destinationParent*/,int destinationRow)
{
    WriteToLog(QtDebugMsg, "Row Change at " + QString::number(sourceStart) +
               " " + QString::number(sourceEnd) +
               " to " + QString::number(destinationRow));

    int WinType = activeChildWindow();
    if (WinType==1)
    {
        LevelLayers buffer;
        if(sourceStart<activeLvlEditWin()->LvlData.layers.size())
        {
            buffer = activeLvlEditWin()->LvlData.layers[sourceStart];
            activeLvlEditWin()->LvlData.layers.remove(sourceStart);
            activeLvlEditWin()->LvlData.layers.insert(((destinationRow>sourceStart)?destinationRow-1:destinationRow), buffer);
        }
    }

    setLayerLists();  //Sync comboboxes in properties
}


void MainWindow::on_LevelLayers_visibilityChanged(bool visible)
{
    ui->actionLayersBox->setChecked(visible);
}

void MainWindow::on_actionLayersBox_triggered(bool checked)
{
    ui->LevelLayers->setVisible(checked);
    if(checked) ui->LevelLayers->raise();
}

void MainWindow::on_AddLayer_clicked()
{
    AddNewLayer(tr("New Layer %1").arg( ui->LvlLayerList->count()+1 ), true);
    /*
    QListWidgetItem * item;
    item = new QListWidgetItem;
    item->setText(tr("New Layer %1").arg( ui->LvlLayerList->count()+1 ) );
    item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEditable);
    item->setFlags(item->flags() | Qt::ItemIsEnabled);
    item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsSelectable);
    item->setCheckState( Qt::Checked );
    item->setData(3, QString("NewLayer") );
    ui->LvlLayerList->addItem( item );

    ui->LvlLayerList->setFocus();
    ui->LvlLayerList->editItem( item );
    */
}


void MainWindow::on_LvlLayerList_itemChanged(QListWidgetItem *item)
{
    if(lockLayerEdit) return;
    int WinType = activeChildWindow();

    if (WinType==1)
    {
        if(item->data(3).toString()=="NewLayer")
        {
            bool AlreadyExist=false;
            foreach(LevelLayers layer, activeLvlEditWin()->LvlData.layers)
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
                activeLvlEditWin()->LvlData.layers_array_id++;
                NewLayer.array_id = activeLvlEditWin()->LvlData.layers_array_id;
                activeLvlEditWin()->scene->addAddLayerHistory(NewLayer.array_id, NewLayer.name);

                item->setData(3, QString::number(NewLayer.array_id));

                activeLvlEditWin()->LvlData.layers.push_back(NewLayer);
                activeLvlEditWin()->LvlData.modified=true;
            }

        }//if(item->data(3).toString()=="NewLayer")
        else
        {
            QString layerName = item->text();
            QString oldLayerName = item->text();
            //unsigned int layerArId = (unsigned int)item->data(3).toInt();
            bool layerVisible = (item->checkState()==Qt::Checked);

            ModifyLayerItem(item, oldLayerName, layerName, layerVisible);

            //Find layer enrty in array and apply settings
            /*for(int i=0; i < activeLvlEditWin()->LvlData.layers.size(); i++)
            {
                if( activeLvlEditWin()->LvlData.layers[i].array_id==layerArId )
                {
                    oldLayerName = activeLvlEditWin()->LvlData.layers[i].name;
                    activeLvlEditWin()->LvlData.layers[i].name = layerName;
                    activeLvlEditWin()->LvlData.layers[i].hidden = !layerVisible;
                    break;
                }
            }*/
            //Apply layer's name/visibly to all items
            //ModifyLayer(oldLayerName, layerName, layerVisible);

            /*
            QList<QGraphicsItem*> ItemList = activeLvlEditWin()->scene->items();

            for (QList<QGraphicsItem*>::iterator it = ItemList.begin(); it != ItemList.end(); it++)
            {
                if((*it)->data(0).toString()=="Block")
                {
                    if(((ItemBlock *)(*it))->blockData.layer!=oldLayerName)
                        continue;
                    ((ItemBlock *)(*it))->blockData.layer = layerName;
                    (*it)->setVisible(layerVisible);
                }
                else
                if((*it)->data(0).toString()=="BGO")
                {
                    if(((ItemBGO *)(*it))->bgoData.layer!=oldLayerName)
                        continue;
                    ((ItemBGO *)(*it))->bgoData.layer = layerName;
                    (*it)->setVisible(layerVisible);
                }
                else
                if((*it)->data(0).toString()=="NPC")
                {
                    if(((ItemNPC *)(*it))->npcData.layer!=oldLayerName)
                        continue;
                    ((ItemNPC *)(*it))->npcData.layer = layerName;
                    (*it)->setVisible(layerVisible);
                }
                else
                if((*it)->data(0).toString()=="Water")
                {
                    //if(((ItemWater *)(*it))->waterData.layer!=oldLayerName)
                    //    continue;
                    //((ItemWater *)(*it))->waterData.layer = layerName;
                    //(*it)->setVisible(layerVisible);
                }
                else
                if(((*it)->data(0).toString()=="Door_enter")||((*it)->data(0).toString()=="Door_exit"))
                {
                    //if(((ItemDoor *)(*it))->doorData.layer!=oldLayerName)
                    //    continue;
                    //((ItemDoor *)(*it))->doorData.layer = layerName;
                    //(*it)->setVisible(layerVisible);
                }
            }
            */
            activeLvlEditWin()->LvlData.modified=true;
        }

    }//if WinType==1
}



void MainWindow::on_RemoveLayer_clicked()
{
    if(ui->LvlLayerList->selectedItems().isEmpty()) return;

    RemoveCurrentLayer(true);
   /*
   QList<QListWidgetItem * > selected = ui->LvlLayerList->selectedItems();

   if(selected.isEmpty()) return;

   if(selected[0]->text()=="Destroyed Blocks") return;
   if(selected[0]->text()=="Spawned NPCs") return;
   if(selected[0]->text()=="Default") return;

   int WinType = activeChildWindow();

   if (WinType==1)
   {
       for(int i=0;i< activeLvlEditWin()->LvlData.layers.size(); i++)
       {
           if( activeLvlEditWin()->LvlData.layers[i].array_id==(unsigned int)selected[0]->data(3).toInt() )
           {
               activeLvlEditWin()->LvlData.layers.remove(i);
               delete selected[0];
               break;
           }
       }
   }
    */
}



void MainWindow::on_LvlLayerList_customContextMenuRequested(const QPoint &pos)
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
        /*
        QString layerName = ui->LvlLayerList->selectedItems()[0]->text();
        bool layerVisible = true;
        bool saveItems = (selected==removeLayerOnly);

        if(saveItems)
        { //Find default layer visibly
            foreach(LevelLayers layer, activeLvlEditWin()->LvlData.layers)
            {
                if( layer.name=="Default" )
                {
                    layerVisible=!layer.hidden;
                    break;
                }
            }
        }


        //Apply layer's name/visibly to all items
        QList<QGraphicsItem*> ItemList = activeLvlEditWin()->scene->items();

        for (QList<QGraphicsItem*>::iterator it = ItemList.begin(); it != ItemList.end(); it++)
        {
            if((*it)->data(0).toString()=="Block")
            {
                if(((ItemBlock *)(*it))->blockData.layer!=layerName)
                    continue;
                if(saveItems)
                {
                    ((ItemBlock *)(*it))->blockData.layer = "Default";
                    (*it)->setVisible(layerVisible);
                }
                else
                {
                    ((ItemBlock *)(*it))->removeFromArray();
                    activeLvlEditWin()->scene->removeItem((*it));
                }
            }
            else
            if((*it)->data(0).toString()=="BGO")
            {
                if(((ItemBGO *)(*it))->bgoData.layer!=layerName)
                    continue;
                if(saveItems)
                {
                    ((ItemBGO *)(*it))->bgoData.layer = "Default";
                    (*it)->setVisible(layerVisible);
                }
                else
                {
                    ((ItemBGO *)(*it))->removeFromArray();
                    activeLvlEditWin()->scene->removeItem((*it));
                }
            }
            else
            if((*it)->data(0).toString()=="NPC")
            {
                if(((ItemNPC *)(*it))->npcData.layer!=layerName)
                    continue;
                if(saveItems)
                {
                    ((ItemNPC *)(*it))->npcData.layer = "Default";
                    (*it)->setVisible(layerVisible);
                }
                else
                {
                    ((ItemNPC *)(*it))->removeFromArray();
                    activeLvlEditWin()->scene->removeItem((*it));
                }
            }
            else
            if((*it)->data(0).toString()=="Water")
            {
                //if(((ItemWater *)(*it))->waterData.layer!=layerName)
                //    continue;
                //((ItemWater *)(*it))->waterData.layer = layerName;
                //(*it)->setVisible(layerVisible);
            }
            else
            if(((*it)->data(0).toString()=="Door_enter")||((*it)->data(0).toString()=="Door_exit"))
            {
                //if(((ItemDoor *)(*it))->doorData.layer!=layerName)
                //    continue;
                //((ItemDoor *)(*it))->doorData.layer = layerName;
                //(*it)->setVisible(layerVisible);
            }
        }
        activeLvlEditWin()->LvlData.modified=true;


        on_RemoveLayer_clicked();
        */
    }


}

