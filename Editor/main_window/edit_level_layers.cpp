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


#include "../ui_mainwindow.h"
#include "../mainwindow.h"

#include "../level_scene/item_bgo.h"
#include "../level_scene/item_block.h"
#include "../level_scene/item_npc.h"
#include "../level_scene/item_water.h"


void MainWindow::RemoveCurrentLayer(bool moveToDefault)
{
    if(activeChildWindow()!=1) return;

    bool layerVisible = true;
    //Remove from List
    QList<QListWidgetItem * > selected = ui->LvlLayerList->selectedItems();

    if(moveToDefault)
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

    if(moveToDefault)
    {
        ModifyLayer(selected[0]->text(), "Default", layerVisible);
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

    for (QList<QGraphicsItem*>::iterator it = ItemList.begin(); it != ItemList.end(); it++)
    {
        if((*it)->data(0).toString()=="Block")
        {
            if(((ItemBlock *)(*it))->blockData.layer==layerName)
            {
                ((ItemBlock *)(*it))->removeFromArray();
                activeLvlEditWin()->scene->removeItem((*it));
            }

        }
        else
        if((*it)->data(0).toString()=="BGO")
        {
            if(((ItemBGO *)(*it))->bgoData.layer==layerName)
            {
                ((ItemBGO *)(*it))->removeFromArray();
                activeLvlEditWin()->scene->removeItem((*it));
            }
        }
        else
        if((*it)->data(0).toString()=="NPC")
        {
            if(((ItemNPC *)(*it))->npcData.layer==layerName)
            {
                ((ItemNPC *)(*it))->removeFromArray();
                activeLvlEditWin()->scene->removeItem((*it));
            }
        }
        else
        if((*it)->data(0).toString()=="Water")
        {
            if(((ItemWater *)(*it))->waterData.layer==layerName)
            {
                ((ItemWater *)(*it))->removeFromArray();
                activeLvlEditWin()->scene->removeItem((*it));
            }
        }
        else
        if(((*it)->data(0).toString()=="Door_enter")||((*it)->data(0).toString()=="Door_exit"))
        {
            //TODO
        }
    }
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
}

void MainWindow::ModifyLayer(QString layerName, bool visible)
{
    //Apply layer's visibly to all items
    QList<QGraphicsItem*> ItemList = activeLvlEditWin()->scene->items();

    for (QList<QGraphicsItem*>::iterator it = ItemList.begin(); it != ItemList.end(); it++)
    {
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
            //TODO
        }
    }
}

void MainWindow::ModifyLayer(QString layerName, QString newLayerName)
{
    //Apply layer's name to all items
    QList<QGraphicsItem*> ItemList = activeLvlEditWin()->scene->items();

    for (QList<QGraphicsItem*>::iterator it = ItemList.begin(); it != ItemList.end(); it++)
    {
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
            //TODO
        }
    }
}

void MainWindow::ModifyLayer(QString layerName, QString newLayerName, bool visible)
{
    //Apply layer's name/visibly to all items
    QList<QGraphicsItem*> ItemList = activeLvlEditWin()->scene->items();

    for (QList<QGraphicsItem*>::iterator it = ItemList.begin(); it != ItemList.end(); it++)
    {
        if((*it)->data(0).toString()=="Block")
        {
            if(((ItemBlock *)(*it))->blockData.layer==layerName)
            {
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
                ((ItemWater *)(*it))->waterData.layer = newLayerName;
                (*it)->setVisible(visible);
                ((ItemWater *)(*it))->arrayApply();
            }
        }
        else
        if(((*it)->data(0).toString()=="Door_enter")||((*it)->data(0).toString()=="Door_exit"))
        {
            //TODO
        }
    }
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

            item->setData(3, QString::number(NewLayer.array_id));

            activeLvlEditWin()->LvlData.layers.push_back(NewLayer);
            activeLvlEditWin()->LvlData.modified=true;
        }
    }
}

void MainWindow::ModifyLayerItem(QListWidgetItem *item, QString oldLayerName, QString newLayerName, bool visible)
{
    //Find layer enrty in array and apply settings
    for(int i=0; i < activeLvlEditWin()->LvlData.layers.size(); i++)
    {
        if( activeLvlEditWin()->LvlData.layers[i].array_id==(unsigned int)item->data(3).toInt() )
        {
            oldLayerName = activeLvlEditWin()->LvlData.layers[i].name;
            activeLvlEditWin()->LvlData.layers[i].name = newLayerName;
            activeLvlEditWin()->LvlData.layers[i].hidden = !visible;
            break;
        }
    }
    //Apply layer's name/visibly to all items
    ModifyLayer(oldLayerName, newLayerName, visible);
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


void MainWindow::setLayersBox()
{
    int WinType = activeChildWindow();
    QListWidgetItem * item;

    ui->LvlLayerList->clear();

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
