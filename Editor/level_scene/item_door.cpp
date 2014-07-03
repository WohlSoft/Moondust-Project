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

#include "item_door.h"
#include "../common_features/logger.h"

#include "newlayerbox.h"

#include "../common_features/mainwinconnect.h"


ItemDoor::ItemDoor(QGraphicsRectItem *parent)
    : QGraphicsRectItem(parent)
{
    isLocked=false;
    itemSize = QSize(32,32);
    doorLabel=NULL;
    doorLabel_shadow=NULL;
    //image = new QGraphicsRectItem;
}


ItemDoor::~ItemDoor()
{
    //WriteToLog(QtDebugMsg, "!<-Door destroy->!");
    if(doorLabel!=NULL) delete doorLabel;
    if(doorLabel_shadow!=NULL) delete doorLabel_shadow;
    if(grp!=NULL) delete grp;

    //WriteToLog(QtDebugMsg, "!<-Door destroyed->!");
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
    if((!scene->lock_door)&&(!scene->DrawMode)&&(!isLocked))
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
            LayerName->deleteLater();

        QAction *setLayer;
        QList<QAction *> layerItems;

        QAction * newLayer = LayerName->addAction(tr("Add to new layer..."));
            LayerName->addSeparator()->deleteLater();;
            newLayer->deleteLater();

        foreach(LevelLayers layer, scene->LvlData->layers)
        {
            //Skip system layers
            if((layer.name=="Destroyed Blocks")||(layer.name=="Spawned NPCs")) continue;

            setLayer = LayerName->addAction( layer.name+((layer.hidden)?" [hidden]":"") );
            setLayer->setData(layer.name);
            setLayer->setCheckable(true);
            setLayer->setEnabled(true);
            setLayer->setChecked( layer.name==doorData.layer );
            setLayer->deleteLater();
            layerItems.push_back(setLayer);
        }

        ItemMenu->addSeparator()->deleteLater();;

        QAction *jumpTo=NULL;
        if(this->data(0).toString()=="Door_enter")
        {
            jumpTo = ItemMenu->addAction(tr("Jump to exit"));
            jumpTo->setVisible( (doorData.isSetIn)&&(doorData.isSetOut) );
            jumpTo->deleteLater();
        }
        else
        if(this->data(0).toString()=="Door_exit")
        {
            jumpTo = ItemMenu->addAction(tr("Jump to entrance"));
            jumpTo->setVisible( (doorData.isSetIn)&&(doorData.isSetOut) );
            jumpTo->deleteLater();
        }

        ItemMenu->addSeparator()->deleteLater();


        QAction * NoTransport = ItemMenu->addAction(tr("No Yoshi"));
        NoTransport->setCheckable(true);
        NoTransport->setChecked( doorData.noyoshi );
        NoTransport->deleteLater();

        QAction * AllowNPC = ItemMenu->addAction(tr("Allow NPC"));
        AllowNPC->setCheckable(true);
        AllowNPC->setChecked( doorData.allownpc );
        AllowNPC->deleteLater();

        QAction * Locked = ItemMenu->addAction(tr("Locked"));
        Locked->setCheckable(true);
        Locked->setChecked( doorData.locked );
        Locked->deleteLater();

        /*
        ItemMenu->addSeparator();
        QAction *copyDoor = ItemMenu->addAction(tr("Copy"));
            copyDoor->setDisabled(true);
        QAction *cutDoor = ItemMenu->addAction(tr("Cut"));
            cutDoor->setDisabled(true);
        */

        ItemMenu->addSeparator()->deleteLater();;
            QAction *remove = ItemMenu->addAction(tr("Remove"));
            remove->deleteLater();

        ItemMenu->addSeparator()->deleteLater();;
            QAction *props = ItemMenu->addAction(tr("Properties..."));
            props->deleteLater();

        scene->contextMenuOpened = true; //bug protector
QAction *selected = ItemMenu->exec(event->screenPos());

        if(!selected)
        {
            WriteToLog(QtDebugMsg, "Context Menu <- NULL");
            scene->contextMenuOpened = true;
            return;
        }
        event->accept();

        if(selected==jumpTo)
        {
            //scene->doCopy = true ;
            if(this->data(0).toString()=="Door_enter")
            {
                if(doorData.isSetOut)
                MainWinConnect::pMainWin->activeLvlEditWin()->goTo(doorData.ox, doorData.oy, true, QPoint(-300, -300));
            }
            else
            if(this->data(0).toString()=="Door_exit")
            {
                if(doorData.isSetIn)
                MainWinConnect::pMainWin->activeLvlEditWin()->goTo(doorData.ix, doorData.iy, true, QPoint(-300, -300));
            }
            scene->contextMenuOpened = false;
        }
        else
        if(selected==NoTransport)
        {
            LevelData modDoors;
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if((SelItem->data(0).toString()=="Door_exit")||(SelItem->data(0).toString()=="Door_enter"))
                {
                    if(SelItem->data(0).toString()=="Door_exit"){
                        LevelDoors door = ((ItemDoor *) SelItem)->doorData;
                        door.isSetOut = true;
                        door.isSetIn = false;
                        modDoors.doors.push_back(door);
                    }else if(SelItem->data(0).toString()=="Door_enter"){
                        LevelDoors door = ((ItemDoor *) SelItem)->doorData;
                        door.isSetOut = false;
                        door.isSetIn = true;
                        modDoors.doors.push_back(door);
                    }
                    ((ItemDoor *) SelItem)->doorData.noyoshi=NoTransport->isChecked();
                    ((ItemDoor *) SelItem)->arrayApply();
                }
            }
            scene->addChangeSettingsHistory(modDoors, LvlScene::SETTING_NOYOSHI, QVariant(NoTransport->isChecked()));
            MainWinConnect::pMainWin->setDoorData(-2);
            scene->contextMenuOpened = false;
        }
        else
        if(selected==AllowNPC)
        {
            LevelData modDoors;
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if((SelItem->data(0).toString()=="Door_exit")||(SelItem->data(0).toString()=="Door_enter"))
                {
                    if(SelItem->data(0).toString()=="Door_exit"){
                        LevelDoors door = ((ItemDoor *) SelItem)->doorData;
                        door.isSetOut = true;
                        door.isSetIn = false;
                        modDoors.doors.push_back(door);
                    }else if(SelItem->data(0).toString()=="Door_enter"){
                        LevelDoors door = ((ItemDoor *) SelItem)->doorData;
                        door.isSetOut = false;
                        door.isSetIn = true;
                        modDoors.doors.push_back(door);
                    }
                    ((ItemDoor *) SelItem)->doorData.allownpc=AllowNPC->isChecked();
                    ((ItemDoor *) SelItem)->arrayApply();
                }
            }
            scene->addChangeSettingsHistory(modDoors, LvlScene::SETTING_ALLOWNPC, QVariant(AllowNPC->isChecked()));
            MainWinConnect::pMainWin->setDoorData(-2);
            scene->contextMenuOpened = false;
        }
        else
        if(selected==Locked)
        {
            LevelData modDoors;
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if((SelItem->data(0).toString()=="Door_exit")||(SelItem->data(0).toString()=="Door_enter"))
                {
                    if(SelItem->data(0).toString()=="Door_exit"){
                        LevelDoors door = ((ItemDoor *) SelItem)->doorData;
                        door.isSetOut = true;
                        door.isSetIn = false;
                        modDoors.doors.push_back(door);
                    }else if(SelItem->data(0).toString()=="Door_enter"){
                        LevelDoors door = ((ItemDoor *) SelItem)->doorData;
                        door.isSetOut = false;
                        door.isSetIn = true;
                        modDoors.doors.push_back(door);
                    }
                    ((ItemDoor *) SelItem)->doorData.locked=Locked->isChecked();
                    ((ItemDoor *) SelItem)->arrayApply();
                }
            }
            scene->addChangeSettingsHistory(modDoors, LvlScene::SETTING_LOCKED, QVariant(Locked->isChecked()));
            MainWinConnect::pMainWin->setDoorData(-2);
            scene->contextMenuOpened = false;
        }
        else
        if(selected==remove)
        {
            //LevelData removedItems;
            //bool deleted=false;

            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if((SelItem->data(0).toString()=="Door_exit")||(SelItem->data(0).toString()=="Door_enter"))
                {
                   // removedItems.doors.push_back(((ItemDoor *)SelItem)->doorData);
                    ((ItemDoor *)SelItem)->removeFromArray();
                    scene->removeItem(SelItem);
                    delete SelItem;
                  //  deleted=true;
                }
            }
            MainWinConnect::pMainWin->setDoorData(-2);
            /* if(deleted) scene->addRemoveHistory( removedItems );*/
            scene->contextMenuOpened = false;
        }
        else
        if(selected==props)
        {
            MainWinConnect::pMainWin->SwitchToDoor(doorData.array_id);
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
                delete layerBox;
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
                LevelData modData;
                foreach(LevelLayers lr, scene->LvlData->layers)
                { //Find layer's settings
                    if(lr.name==lName)
                    {
                        foreach(QGraphicsItem * SelItem, scene->selectedItems() )
                        {

                            if((SelItem->data(0).toString()=="Door_exit")  ||
                                    (SelItem->data(0).toString()=="Door_enter"))
                            {
                                if(SelItem->data(0).toString()=="Door_exit"){
                                    LevelDoors tDoor = ((ItemDoor *) SelItem)->doorData;
                                    tDoor.isSetOut = true;
                                    tDoor.isSetIn = false;
                                    modData.doors.push_back(tDoor);
                                }
                                else
                                if(SelItem->data(0).toString()=="Door_enter"){
                                    LevelDoors tDoor = ((ItemDoor *) SelItem)->doorData;
                                    tDoor.isSetOut = false;
                                    tDoor.isSetIn = true;
                                    modData.doors.push_back(tDoor);
                                }
                                ((ItemDoor *) SelItem)->doorData.layer = lr.name;
                                ((ItemDoor *) SelItem)->setVisible(!lr.hidden);
                                ((ItemDoor *) SelItem)->arrayApply();
                            }
                        }
                        if(selected==newLayer){
                            scene->addChangedNewLayerHistory(modData, lr);
                        }
                        break;
                    }
                }//Find layer's settings
                if(selected!=newLayer){
                    scene->addChangedLayerHistory(modData, lName);
                }
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

    //Sync data to his pair door item
    if(direction==D_Entrance)
    {
        if(doorData.isSetOut)
        {
            foreach(QGraphicsItem * door, scene->items())
            {
                if((door->data(0).toString()=="Door_exit")&&((unsigned int)door->data(2).toInt()==doorData.array_id))
                {
                    ((ItemDoor *)door)->doorData = doorData;
                    break;
                }
            }
        }
    }
    else
    {
        if(doorData.isSetIn)
        {
            foreach(QGraphicsItem * door, scene->items())
            {
                if((door->data(0).toString()=="Door_enter")&&((unsigned int)door->data(2).toInt()==doorData.array_id))
                {
                    ((ItemDoor *)door)->doorData = doorData;
                    break;
                }
            }
        }

    }

}

void ItemDoor::removeFromArray()
{

    if(direction==D_Entrance)
    {
        doorData.isSetIn=false;
        doorData.ix = 0;
        doorData.iy = 0;
    }
    else
    {
        doorData.isSetOut=false;
        doorData.ox = 0;
        doorData.oy = 0;
    }
    arrayApply();
}

void ItemDoor::setDoorData(LevelDoors inD, int doorDir, bool init)
{
    doorData = inD;
    direction = doorDir;

    long ix, iy, ox, oy;
    QColor cEnter(Qt::magenta);
    QColor cExit(Qt::darkMagenta);
    cEnter.setAlpha(50);
    cExit.setAlpha(50);

    ix = doorData.ix;
    iy = doorData.iy;
    ox = doorData.ox;
    oy = doorData.oy;

    QFont font1, font2;
    font1.setWeight(50);
    font1.setBold(1);
    font1.setPointSize(14);

    font2.setWeight(14);
    font2.setBold(0);
    font2.setPointSize(12);

    setRect(0, 0, itemSize.width(), itemSize.height());

    doorLabel_shadow = new QGraphicsTextItem(QString::number(doorData.array_id));
    doorLabel = new QGraphicsTextItem(QString::number(doorData.array_id));
    if(direction==D_Entrance)
    {
        doorData.isSetIn=true;
        setBrush(QBrush(cEnter));
        setPen(QPen(Qt::magenta, 2,Qt::SolidLine));

        doorLabel_shadow->setDefaultTextColor(Qt::black);
        doorLabel_shadow->setFont(font1);
        doorLabel_shadow->setPos(ix-5, iy-2);
        doorLabel->setDefaultTextColor(Qt::white);
        doorLabel->setFont(font2);
        doorLabel->setPos(ix-3, iy);

        this->setPos(ix, iy);

        this->setData(0, "Door_enter"); // ObjType
    }
    else
    {
        doorData.isSetOut=true;
        setBrush(QBrush(cExit));
        setPen( QPen(Qt::darkMagenta, 2,Qt::SolidLine) );

        doorLabel_shadow->setDefaultTextColor(Qt::black);
        doorLabel_shadow->setFont(font1);
        doorLabel_shadow->setPos(ox+10, oy+8);
        doorLabel->setDefaultTextColor(Qt::white);
        doorLabel->setFont(font2);
        doorLabel->setPos(ox+12, oy+10);

        this->setPos(ox, oy);

        this->setData(0, "Door_exit"); // ObjType
    }
    grp->addToGroup(doorLabel);
    grp->addToGroup(doorLabel_shadow);

    this->setFlag(QGraphicsItem::ItemIsSelectable, (!scene->lock_door));
    this->setFlag(QGraphicsItem::ItemIsMovable, (!scene->lock_door));

    doorLabel_shadow->setZValue(scene->doorZ+0.0000001);
    doorLabel->setZValue(scene->doorZ+0.0000002);

    this->setData(1, QString::number(0) );
    this->setData(2, QString::number(doorData.array_id) );

    this->setZValue(scene->doorZ);

    if(!init)
    {
        arrayApply();
    }
}

void ItemDoor::setLocked(bool lock)
{
    this->setFlag(QGraphicsItem::ItemIsSelectable, !lock);
    this->setFlag(QGraphicsItem::ItemIsMovable, !lock);
    isLocked = lock;
}


QRectF ItemDoor::boundingRect() const
{
    return QRectF(0,0,itemSize.width(),itemSize.height());
}

void ItemDoor::setContextMenu(QMenu &menu)
{
    ItemMenu = &menu;
}

void ItemDoor::setScenePoint(LvlScene *theScene)
{
    scene = theScene;
    grp = new QGraphicsItemGroup(this);
    doorLabel = NULL;
    doorLabel_shadow = NULL;
}

