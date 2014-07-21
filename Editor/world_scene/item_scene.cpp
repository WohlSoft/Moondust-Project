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

#include "item_scene.h"
#include "../common_features/logger.h"

#include "../common_features/mainwinconnect.h"


ItemScene::ItemScene(QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    gridSize=32;
    gridOffsetX=0;
    gridOffsetY=0;
    isLocked=false;

    animatorID=-1;
    imageSize = QRectF(0,0,10,10);
}


ItemScene::~ItemScene()
{
    //WriteToLog(QtDebugMsg, "!<-BGO destroyed->!");
    //if(timer) delete timer;
}

void ItemScene::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
    if(scene->DrawMode)
    {
        unsetCursor();
        ungrabMouse();
        this->setSelected(false);
        return;
    }
    QGraphicsItem::mousePressEvent(mouseEvent);
}

void ItemScene::contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
{
    if((!scene->lock_scene)&&(!scene->DrawMode)&&(!isLocked))
    {
        //Remove selection from non-bgo items
        if(this->isSelected())
        {
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(0).toString()!="SCENERY") SelItem->setSelected(false);
            }
        }
        else
        {
            scene->clearSelection();
            this->setSelected(true);
        }

        this->setSelected(1);
        ItemMenu->clear();

        QAction *copyTile = ItemMenu->addAction(tr("Copy"));
        copyTile->deleteLater();
        QAction *cutTile = ItemMenu->addAction(tr("Cut"));
        cutTile->deleteLater();
        ItemMenu->addSeparator()->deleteLater();
        QAction *remove = ItemMenu->addAction(tr("Remove"));
        remove->deleteLater();
        //ItemMenu->addSeparator()->deleteLater();;
        //QAction *props = ItemMenu->addAction(tr("Properties..."));
        //props->deleteLater();

        scene->contextMenuOpened = true; //bug protector
QAction *selected = ItemMenu->exec(event->screenPos());

        if(!selected)
        {
            #ifdef _DEBUG_
            WriteToLog(QtDebugMsg, "Context Menu <- NULL");
            #endif
            scene->contextMenuOpened = true;
            return;
        }
        event->accept();

        if(selected==cutTile)
        {
            //scene->doCut = true ;
            MainWinConnect::pMainWin->on_actionCut_triggered();
            scene->contextMenuOpened = false;
        }
        else
        if(selected==copyTile)
        {
            //scene->doCopy = true ;
            MainWinConnect::pMainWin->on_actionCopy_triggered();
            scene->contextMenuOpened = false;
        }
        else
        if(selected==remove)
        {
            WorldData removedItems;
            //bool deleted=false;
            scene->contextMenuOpened = false;
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(0).toString()=="SCENERY")
                {
                    removedItems.scenery.push_back(((ItemScene *)SelItem)->sceneData);
                    ((ItemScene *)SelItem)->removeFromArray();
                    scene->removeItem(SelItem);
                    delete SelItem;
                    //deleted=true;
                }
            }
            //if(deleted) MainWinConnect::pMainWin->activeLvlEditWin()->scene->addRemoveHistory( removedItems );
        }
//        else
//        if(selected==props)
//        {
//            scene->openProps();
//        }
//        else
//        {
//            bool itemIsFound=false;
//            QString lName;
//            if(selected==newLayer)
//            {
//                scene->contextMenuOpened = false;
//                ToNewLayerBox * layerBox = new ToNewLayerBox(scene->WldData);
//                layerBox->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
//                layerBox->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, layerBox->size(), qApp->desktop()->availableGeometry()));
//                if(layerBox->exec()==QDialog::Accepted)
//                {
//                    itemIsFound=true;
//                    lName = layerBox->lName;

//                    //Store new layer into array
//                    LevelLayers nLayer;
//                    nLayer.name = lName;
//                    nLayer.hidden = layerBox->lHidden;
//                    scene->WldData->layers_array_id++;
//                    nLayer.array_id = scene->WldData->layers_array_id;
//                    scene->WldData->layers.push_back(nLayer);

//                    //scene->SyncLayerList=true; //Refresh layer list
//                    MainWinConnect::pMainWin->setLayerToolsLocked(true);
//                    MainWinConnect::pMainWin->setLayersBox();
//                    MainWinConnect::pMainWin->setLayerToolsLocked(false);
//                }
//                delete layerBox;
//            }
//            else
//            foreach(QAction * lItem, layerItems)
//            {
//                if(selected==lItem)
//                {
//                    itemIsFound=true;
//                    lName = lItem->data().toString();
//                    //FOUND!!!
//                 break;
//                }//Find selected layer's item
//            }

//            if(itemIsFound)
//            {
//                LevelData modData;
//                foreach(LevelLayers lr, scene->WldData->layers)
//                { //Find layer's settings
//                    if(lr.name==lName)
//                    {
//                        foreach(QGraphicsItem * SelItem, scene->selectedItems() )
//                        {

//                            if(SelItem->data(0).toString()=="BGO")
//                            {
//                                modData.bgo.push_back(((ItemScene*) SelItem)->sceneData);
//                                ((ItemScene *) SelItem)->sceneData.layer = lr.name;
//                                ((ItemScene *) SelItem)->setVisible(!lr.hidden);
//                                ((ItemScene *) SelItem)->arrayApply();
//                            }
//                        }
//                        if(selected==newLayer){
//                            scene->addChangedNewLayerHistory(modData, lr);
//                        }
//                        break;
//                    }
//                }//Find layer's settings
//                if(selected!=newLayer){
//                    scene->addChangedLayerHistory(modData, lName);
//                }
//                scene->contextMenuOpened = false;
//            }
//        }
    }
    else
    {
        QGraphicsItem::contextMenuEvent(event);
    }
}


///////////////////MainArray functions/////////////////////////////
//void ItemScene::setLayer(QString layer)
//{
//    foreach(LevelLayers lr, scene->WldData->layers)
//    {
//        if(lr.name==layer)
//        {
//            sceneData.layer = layer;
//            this->setVisible(!lr.hidden);
//            arrayApply();
//        break;
//        }
//    }
//}

void ItemScene::arrayApply()
{
    bool found=false;
    if(sceneData.index < (unsigned int)scene->WldData->scenery.size())
    { //Check index
        if(sceneData.array_id == scene->WldData->scenery[sceneData.index].array_id)
        {
            found=true;
        }
    }

    //Apply current data in main array
    if(found)
    { //directlry
        scene->WldData->scenery[sceneData.index] = sceneData; //apply current sceneData
    }
    else
    for(int i=0; i<scene->WldData->scenery.size(); i++)
    { //after find it into array
        if(scene->WldData->scenery[i].array_id == sceneData.array_id)
        {
            sceneData.index = i;
            scene->WldData->scenery[i] = sceneData;
            break;
        }
    }
}

void ItemScene::removeFromArray()
{
    bool found=false;
    if(sceneData.index < (unsigned int)scene->WldData->scenery.size())
    { //Check index
        if(sceneData.array_id == scene->WldData->scenery[sceneData.index].array_id)
        {
            found=true;
        }
    }

    if(found)
    { //directlry
        scene->WldData->scenery.remove(sceneData.index);
    }
    else
    for(int i=0; i<scene->WldData->scenery.size(); i++)
    {
        if(scene->WldData->scenery[i].array_id == sceneData.array_id)
        {
            scene->WldData->scenery.remove(i); break;
        }
    }
}

void ItemScene::setSceneData(WorldScenery inD)
{
    sceneData = inD;
}


QRectF ItemScene::boundingRect() const
{
    return imageSize;
}

void ItemScene::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if(animatorID<0)
    {
        painter->drawRect(QRect(0,0,1,1));
        return;
    }
    if(scene->animates_Scenery.size()>animatorID)
        painter->drawPixmap(imageSize, scene->animates_Scenery[animatorID]->image(), imageSize);
    else
        painter->drawRect(QRect(0,0,32,32));

    if(this->isSelected())
    {
        painter->setPen(QPen(QBrush(Qt::black), 2, Qt::SolidLine));
        painter->drawRect(1,1,imageSize.width()-2,imageSize.height()-2);
        painter->setPen(QPen(QBrush(Qt::yellow), 2, Qt::DotLine));
        painter->drawRect(1,1,imageSize.width()-2,imageSize.height()-2);
    }
}

void ItemScene::setContextMenu(QMenu &menu)
{
    ItemMenu = &menu;
}

void ItemScene::setScenePoint(WldScene *theScene)
{
    scene = theScene;
}


////////////////Animation///////////////////

void ItemScene::setAnimator(long aniID)
{
    if(aniID<scene->animates_Scenery.size())
    imageSize = QRectF(0,0,
                scene->animates_Scenery[aniID]->image().width(),
                scene->animates_Scenery[aniID]->image().height()
                );

    this->setData(9, QString::number( gridSize ) ); //width
    this->setData(10, QString::number( gridSize ) ); //height

    //WriteToLog(QtDebugMsg, QString("Scenery Animator ID: %1").arg(aniID));

    animatorID = aniID;
}
