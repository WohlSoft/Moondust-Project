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

#include "item_bgo.h"
#include "../common_features/logger.h"

#include "newlayerbox.h"

#include "../common_features/mainwinconnect.h"


ItemBGO::ItemBGO(QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    gridSize=32;
    gridOffsetX=0;
    gridOffsetY=0;
    isLocked=false;

    animatorID=-1;
    imageSize = QRectF(0,0,10,10);
}


ItemBGO::~ItemBGO()
{
    //WriteToLog(QtDebugMsg, "!<-BGO destroyed->!");
    //if(timer) delete timer;
}

void ItemBGO::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent )
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

void ItemBGO::contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
{
    if((!scene->lock_bgo)&&(!scene->DrawMode)&&(!isLocked))
    {
        //Remove selection from non-bgo items
        if(this->isSelected())
        {
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(0).toString()!="BGO") SelItem->setSelected(false);
            }
        }
        else
        {
            scene->clearSelection();
            this->setSelected(true);
        }

        this->setSelected(1);
        ItemMenu->clear();

        QMenu * LayerName = ItemMenu->addMenu(tr("Layer: ")+QString("[%1]").arg(bgoData.layer));
        LayerName->deleteLater();

        QAction *setLayer;
        QList<QAction *> layerItems;

        QAction * newLayer = LayerName->addAction(tr("Add to new layer..."));
        LayerName->addSeparator()->deleteLater();

        foreach(LevelLayers layer, scene->LvlData->layers)
        {
            //Skip system layers
            if((layer.name=="Destroyed Blocks")||(layer.name=="Spawned NPCs")) continue;

            setLayer = LayerName->addAction( layer.name+((layer.hidden)?" [hidden]":"") );
            setLayer->setData(layer.name);
            setLayer->setCheckable(true);
            setLayer->setEnabled(true);
            setLayer->setChecked( layer.name==bgoData.layer );
            setLayer->deleteLater();
            layerItems.push_back(setLayer);
        }

        ItemMenu->addSeparator();
        QAction *copyBGO = ItemMenu->addAction(tr("Copy"));
        copyBGO->deleteLater();
        QAction *cutBGO = ItemMenu->addAction(tr("Cut"));
        cutBGO->deleteLater();
        ItemMenu->addSeparator()->deleteLater();
        QAction *remove = ItemMenu->addAction(tr("Remove"));
        remove->deleteLater();
        ItemMenu->addSeparator()->deleteLater();;
        QAction *props = ItemMenu->addAction(tr("Properties..."));
        props->deleteLater();

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

        if(selected==cutBGO)
        {
            //scene->doCut = true ;
            MainWinConnect::pMainWin->on_actionCut_triggered();
            scene->contextMenuOpened = false;
        }
        else
        if(selected==copyBGO)
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
            scene->contextMenuOpened = false;
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(0).toString()=="BGO")
                {
                    removedItems.bgo.push_back(((ItemBGO *)SelItem)->bgoData);
                    ((ItemBGO *)SelItem)->removeFromArray();
                    scene->removeItem(SelItem);
                    delete SelItem;
                    deleted=true;
                }
            }
            if(deleted) MainWinConnect::pMainWin->activeLvlEditWin()->scene->addRemoveHistory( removedItems );
        }
        else
        if(selected==props)
        {
            scene->openProps();
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
                    MainWinConnect::pMainWin->setLayerToolsLocked(true);
                    MainWinConnect::pMainWin->setLayersBox();
                    MainWinConnect::pMainWin->setLayerToolsLocked(false);
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

                            if(SelItem->data(0).toString()=="BGO")
                            {
                                modData.bgo.push_back(((ItemBGO*) SelItem)->bgoData);
                                ((ItemBGO *) SelItem)->bgoData.layer = lr.name;
                                ((ItemBGO *) SelItem)->setVisible(!lr.hidden);
                                ((ItemBGO *) SelItem)->arrayApply();
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
        QGraphicsItem::contextMenuEvent(event);
    }
}


///////////////////MainArray functions/////////////////////////////
void ItemBGO::setLayer(QString layer)
{
    foreach(LevelLayers lr, scene->LvlData->layers)
    {
        if(lr.name==layer)
        {
            bgoData.layer = layer;
            this->setVisible(!lr.hidden);
            arrayApply();
        break;
        }
    }
}

void ItemBGO::arrayApply()
{
    bool found=false;
    if(bgoData.index < (unsigned int)scene->LvlData->bgo.size())
    { //Check index
        if(bgoData.array_id == scene->LvlData->bgo[bgoData.index].array_id)
        {
            found=true;
        }
    }

    //Apply current data in main array
    if(found)
    { //directlry
        scene->LvlData->bgo[bgoData.index] = bgoData; //apply current bgoData
    }
    else
    for(int i=0; i<scene->LvlData->bgo.size(); i++)
    { //after find it into array
        if(scene->LvlData->bgo[i].array_id == bgoData.array_id)
        {
            bgoData.index = i;
            scene->LvlData->bgo[i] = bgoData;
            break;
        }
    }
}

void ItemBGO::removeFromArray()
{
    bool found=false;
    if(bgoData.index < (unsigned int)scene->LvlData->bgo.size())
    { //Check index
        if(bgoData.array_id == scene->LvlData->bgo[bgoData.index].array_id)
        {
            found=true;
        }
    }

    if(found)
    { //directlry
        scene->LvlData->bgo.remove(bgoData.index);
    }
    else
    for(int i=0; i<scene->LvlData->bgo.size(); i++)
    {
        if(scene->LvlData->bgo[i].array_id == bgoData.array_id)
        {
            scene->LvlData->bgo.remove(i); break;
        }
    }
}

void ItemBGO::setBGOData(LevelBGO inD)
{
    bgoData = inD;
}


QRectF ItemBGO::boundingRect() const
{
    return imageSize;
}

void ItemBGO::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if(animatorID<0)
    {
        painter->drawRect(QRect(0,0,1,1));
        return;
    }
    if(scene->animates_BGO.size()>animatorID)
        painter->drawPixmap(imageSize, scene->animates_BGO[animatorID]->image(), imageSize);
    else
        painter->drawRect(QRect(0,0,32,32));

    if(this->isSelected())
    {
        painter->setPen(QPen(QBrush(Qt::black), 2, Qt::SolidLine));
        painter->drawRect(1,1,imageSize.width()-2,imageSize.height()-2);
        painter->setPen(QPen(QBrush(Qt::red), 2, Qt::DotLine));
        painter->drawRect(1,1,imageSize.width()-2,imageSize.height()-2);
    }
}

void ItemBGO::setContextMenu(QMenu &menu)
{
    ItemMenu = &menu;
}

void ItemBGO::setScenePoint(LvlScene *theScene)
{
    scene = theScene;
}


////////////////Animation///////////////////

void ItemBGO::setAnimator(long aniID)
{
    if(aniID<scene->animates_BGO.size())
    imageSize = QRectF(0,0,
                scene->animates_BGO[aniID]->image().width(),
                scene->animates_BGO[aniID]->image().height()
                );

    this->setData(9, QString::number(qRound(imageSize.width())) ); //width
    this->setData(10, QString::number(qRound(imageSize.height())) ); //height

    //WriteToLog(QtDebugMsg, QString("BGO Animator ID: %1").arg(aniID));

    animatorID = aniID;
}
