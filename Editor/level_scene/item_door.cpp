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

#include "item_door.h"
#include "../common_features/logger.h"

#include "newlayerbox.h"

#include "../common_features/mainwinconnect.h"


ItemDoor::ItemDoor(QGraphicsRectItem *parent)
    : QGraphicsRectItem(parent)
{

    isLocked=false;
    waterSize = QSize(32,32);
    //image = new QGraphicsRectItem;
}


ItemDoor::~ItemDoor()
{
 //   WriteToLog(QtDebugMsg, "!<-Water destroyed->!");
}

void ItemDoor::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
    if(scene->DrawMode)
    {
        unsetCursor();
        ungrabMouse();
        this->setSelected(false);
        return;
    }
    QGraphicsRectItem::mousePressEvent(mouseEvent);
}

void ItemDoor::contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
{
    if((!scene->lock_water)&&(!scene->DrawMode)&&(!isLocked))
    {
        //Remove selection from non-bgo items
        if(this->isSelected())
        {
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(0).toString()!="Water") SelItem->setSelected(false);
            }
        }
        else
        {
            scene->clearSelection();
            this->setSelected(true);
        }

        this->setSelected(1);
        ItemMenu->clear();

        QMenu * LayerName = ItemMenu->addMenu(tr("Layer: ")+QString("[%1]").arg(doorData.layer));

        QAction *setLayer;
        QList<QAction *> layerItems;

        QAction * newLayer = LayerName->addAction(tr("Add to new layer..."));
            LayerName->addSeparator();

        foreach(LevelLayers layer, scene->LvlData->layers)
        {
            //Skip system layers
            if((layer.name=="Destroyed Blocks")||(layer.name=="Spawned NPCs")) continue;

            setLayer = LayerName->addAction( layer.name+((layer.hidden)?" [hidden]":"") );
            setLayer->setData(layer.name);
            setLayer->setCheckable(true);
            setLayer->setEnabled(true);
            setLayer->setChecked( layer.name==doorData.layer );
            layerItems.push_back(setLayer);
        }

        ItemMenu->addSeparator();

        QAction *jumpTo=NULL;
        if(this->data(0).toString()=="Door_enter")
        {
            jumpTo = ItemMenu->addAction(tr("Jump to exit"));
            jumpTo->setVisible( (doorData.isSetIn)&&(doorData.isSetOut) );
        }
        else
        if(this->data(0).toString()=="Door_exit")
        {
            jumpTo = ItemMenu->addAction(tr("Jump to entrance"));
            jumpTo->setVisible( (doorData.isSetIn)&&(doorData.isSetOut) );
        }

        ItemMenu->addSeparator();
        QAction *copyDoor = ItemMenu->addAction(tr("Copy"));
        QAction *cutDoor = ItemMenu->addAction(tr("Cut"));

        ItemMenu->addSeparator();
        QAction *remove = ItemMenu->addAction(tr("Remove"));

        scene->contextMenuOpened = true; //bug protector
QAction *selected = ItemMenu->exec(event->screenPos());

        if(!selected)
        {
            WriteToLog(QtDebugMsg, "Context Menu <- NULL");
            scene->contextMenuOpened = true;
            return;
        }
        event->accept();

        if(selected==cutDoor)
        {
            //scene->doCut = true ;
            MainWinConnect::pMainWin->on_actionCut_triggered();
            scene->contextMenuOpened = false;
        }
        else
        if(selected==copyDoor)
        {
            //scene->doCopy = true ;
            MainWinConnect::pMainWin->on_actionCopy_triggered();
            scene->contextMenuOpened = false;
        }
        else
        if(selected==remove)
        {
            LevelData removedItems;
            bool deleted=false;

            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if((SelItem->data(0).toString()=="Door_exit")||(SelItem->data(0).toString()=="Door_enter"))
                {
                    removedItems.doors.push_back(((ItemDoor *)SelItem)->doorData);
                    ((ItemDoor *)SelItem)->removeFromArray();
                    scene->removeItem(SelItem);
                    delete SelItem;
                    deleted=true;
                }
            }
            if(deleted) scene->addRemoveHistory( removedItems );
            scene->contextMenuOpened = false;
        }
        else
        {
            bool itemIsFound=false;
            QString lName;
            if(selected==newLayer)
            {
                scene->contextMenuOpened = false;
                ToNewLayerBox * layerBox = new ToNewLayerBox(scene->LvlData);
                layerBox->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
                layerBox->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, layerBox->size(), qApp->desktop()->availableGeometry()));
                if(layerBox->exec()==QDialog::Accepted)
                {
                    itemIsFound=true;
                    lName = layerBox->lName;

                    //Store new layer into array
                    LevelLayers nLayer;
                    nLayer.name = lName;
                    nLayer.hidden = layerBox->lHidden;
                    scene->LvlData->layers_array_id++;
                    nLayer.array_id = scene->LvlData->layers_array_id;
                    scene->LvlData->layers.push_back(nLayer);

                    //scene->SyncLayerList=true; //Refresh layer list
                    MainWinConnect::pMainWin->setLayersBox();
                }
            }
            else
            foreach(QAction * lItem, layerItems)
            {
                if(selected==lItem)
                {
                    itemIsFound=true;
                    lName = lItem->data().toString();
                    //FOUND!!!
                 break;
                }//Find selected layer's item
            }

            if(itemIsFound)
            {
                foreach(LevelLayers lr, scene->LvlData->layers)
                { //Find layer's settings
                    if(lr.name==lName)
                    {
                        foreach(QGraphicsItem * SelItem, scene->selectedItems() )
                        {

                            if(SelItem->data(0).toString()=="Water")
                            {
                            ((ItemDoor *) SelItem)->doorData.layer = lr.name;
                            ((ItemDoor *) SelItem)->setVisible(!lr.hidden);
                            ((ItemDoor *) SelItem)->arrayApply();
                            }
                        }
                    break;
                    }
                }//Find layer's settings
             scene->contextMenuOpened = false;
            }
        }
    }
    else
    {
        QGraphicsRectItem::contextMenuEvent(event);
    }
}


///////////////////MainArray functions/////////////////////////////
void ItemDoor::setLayer(QString layer)
{
    foreach(LevelLayers lr, scene->LvlData->layers)
    {
        if(lr.name==layer)
        {
            doorData.layer = layer;
            this->setVisible(!lr.hidden);
            arrayApply();
        break;
        }
    }
}

void ItemDoor::arrayApply()
{
    bool found=false;
    if(doorData.index < (unsigned int)scene->LvlData->doors.size())
    { //Check index
        if(doorData.array_id == scene->LvlData->doors[doorData.index].array_id)
        {
            found=true;
        }
    }

    //Apply current data in main array
    if(found)
    { //directlry
        scene->LvlData->doors[doorData.index] = doorData; //apply current bgoData
    }
    else
    for(int i=0; i<scene->LvlData->doors.size(); i++)
    { //after find it into array
        if(scene->LvlData->doors[i].array_id == doorData.array_id)
        {
            doorData.index = i;
            scene->LvlData->doors[i] = doorData;
            break;
        }
    }
}

void ItemDoor::removeFromArray()
{
    bool found=false;
    if(doorData.index < (unsigned int)scene->LvlData->bgo.size())
    { //Check index
        if(doorData.array_id == scene->LvlData->doors[doorData.index].array_id)
        {
            found=true;
        }
    }

    if(found)
    { //directlry
        scene->LvlData->doors.remove(doorData.index);
    }
    else
    for(int i=0; i<scene->LvlData->doors.size(); i++)
    {
        if(scene->LvlData->doors[i].array_id == doorData.array_id)
        {
            scene->LvlData->doors.remove(i); break;
        }
    }
}

void ItemDoor::setDoorData(LevelDoors inD, int doorDir, bool init)
{
    doorData = inD;
    direction = doorDir;
    if(!init)
    {

    }
}


QRectF ItemDoor::boundingRect() const
{
    return QRectF(0,0,waterSize.width(),waterSize.height());
}

void ItemDoor::setContextMenu(QMenu &menu)
{
    ItemMenu = &menu;
}

void ItemDoor::setScenePoint(LvlScene *theScene)
{
    scene = theScene;
}

