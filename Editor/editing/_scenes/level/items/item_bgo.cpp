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

#include <QInputDialog>

#include <common_features/mainwinconnect.h>
#include <common_features/logger.h>

#include "item_block.h"
#include "item_bgo.h"
#include "item_npc.h"
#include "item_water.h"
#include "item_door.h"
#include "../newlayerbox.h"

ItemBGO::ItemBGO(QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    gridSize=32;
    gridOffsetX=0;
    gridOffsetY=0;
    isLocked=false;

    animatorID=-1;
    imageSize = QRectF(0,0,10,10);

    zMode = LevelBGO::ZDefault;

    mouseLeft=false;
    mouseMid=false;
    mouseRight=false;
}


ItemBGO::~ItemBGO()
{
    //WriteToLog(QtDebugMsg, "!<-BGO destroyed->!");
    //if(timer) delete timer;
}

void ItemBGO::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
    if((this->flags()&QGraphicsItem::ItemIsSelectable)==0)
    {
        QGraphicsItem::mousePressEvent(mouseEvent); return;
    }

    if(scene->DrawMode)
    {
        unsetCursor();
        ungrabMouse();
        this->setSelected(false);
        return;
    }
    //Discard multi-mouse keys
    if((mouseLeft)||(mouseMid)||(mouseRight))
    {
        mouseEvent->accept();
        return;
    }

    if( mouseEvent->buttons() & Qt::LeftButton )
        mouseLeft=true;
    if( mouseEvent->buttons() & Qt::MiddleButton )
        mouseMid=true;
    if( mouseEvent->buttons() & Qt::RightButton )
        mouseRight=true;

    QGraphicsItem::mousePressEvent(mouseEvent);
}

void ItemBGO::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    int multimouse=0;
    bool callContext=false;
    if(((mouseMid)||(mouseRight))&&( mouseLeft^(mouseEvent->buttons() & Qt::LeftButton) ))
        multimouse++;
    if( (((mouseLeft)||(mouseRight)))&&( mouseMid^(mouseEvent->buttons() & Qt::MiddleButton) ))
        multimouse++;
    if((((mouseLeft)||(mouseMid)))&&( mouseRight^(mouseEvent->buttons() & Qt::RightButton) ))
        multimouse++;
    if(multimouse>0)
    {
        mouseEvent->accept(); return;
    }

    if( mouseLeft^(mouseEvent->buttons() & Qt::LeftButton) )
        mouseLeft=false;

    if( mouseMid^(mouseEvent->buttons() & Qt::MiddleButton) )
        mouseMid=false;

    if( mouseRight^(mouseEvent->buttons() & Qt::RightButton) )
    {
        if(!scene->IsMoved) callContext=true;
        mouseRight=false;
    }

    QGraphicsItem::mouseReleaseEvent(mouseEvent);

    /////////////////////////CONTEXT MENU:///////////////////////////////
    if((callContext)&&(!scene->contextMenuOpened))
    {
        if((!scene->lock_bgo)&&(!scene->DrawMode)&&(!isLocked))
        {
            scene->contextMenuOpened=true;
            //Remove selection from non-bgo items
            if(!this->isSelected())
            {
                scene->clearSelection();
                this->setSelected(true);
            }

            this->setSelected(1);
            ItemMenu->clear();

            QMenu * LayerName = ItemMenu->addMenu(tr("Layer: ")+QString("[%1]").arg(bgoData.layer).replace("&", "&&&"));
            LayerName->deleteLater();

            QAction *setLayer;
            QList<QAction *> layerItems;

            QAction * newLayer = LayerName->addAction(tr("Add to new layer..."));
            LayerName->addSeparator()->deleteLater();

            foreach(LevelLayers layer, scene->LvlData->layers)
            {
                //Skip system layers
                if((layer.name=="Destroyed Blocks")||(layer.name=="Spawned NPCs")) continue;

                setLayer = LayerName->addAction( layer.name.replace("&", "&&&")+((layer.hidden)?" [hidden]":"") );
                setLayer->setData(layer.name);
                setLayer->setCheckable(true);
                setLayer->setEnabled(true);
                setLayer->setChecked( layer.name==bgoData.layer );
                setLayer->deleteLater();
                layerItems.push_back(setLayer);
            }
            ItemMenu->addSeparator()->deleteLater();

            bool isLvlx = !scene->LvlData->smbx64strict;

            QAction *ZOffset = ItemMenu->addAction(tr("Change Z-Offset..."));
            ZOffset->setEnabled(isLvlx);
            QMenu *ZMode = ItemMenu->addMenu(tr("Z-Layer"));
            ZMode->setEnabled(isLvlx);

            QAction *ZMode_bg2 = ZMode->addAction(tr("Background-2"));
            ZMode_bg2->setCheckable(true);
            ZMode_bg2->setChecked(bgoData.z_mode==LevelBGO::Background2);
            QAction *ZMode_bg1 = ZMode->addAction(tr("Background"));
            ZMode_bg1->setCheckable(true);
            ZMode_bg1->setChecked(bgoData.z_mode==LevelBGO::Background1);
            QAction *ZMode_def = ZMode->addAction(tr("Default"));
            ZMode_def->setCheckable(true);
            ZMode_def->setChecked(bgoData.z_mode==LevelBGO::ZDefault);
            QAction *ZMode_fg1 = ZMode->addAction(tr("Foreground"));
            ZMode_fg1->setCheckable(true);
            ZMode_fg1->setChecked(bgoData.z_mode==LevelBGO::Foreground1);
            QAction *ZMode_fg2 = ZMode->addAction(tr("Foreground-2"));
            ZMode_fg2->setCheckable(true);
            ZMode_fg2->setChecked(bgoData.z_mode==LevelBGO::Foreground2);

            ItemMenu->addSeparator()->deleteLater();
            QAction *copyBGO = ItemMenu->addAction(tr("Copy"));
            copyBGO->deleteLater();
            QAction *cutBGO = ItemMenu->addAction(tr("Cut"));
            cutBGO->deleteLater();
            ItemMenu->addSeparator()->deleteLater();
            QAction *remove = ItemMenu->addAction(tr("Remove"));
            remove->deleteLater();
            ItemMenu->addSeparator()->deleteLater();
            QAction *props = ItemMenu->addAction(tr("Properties..."));
            props->deleteLater();

    QAction *selected = ItemMenu->exec(mouseEvent->screenPos());

            if(!selected)
            {
                #ifdef _DEBUG_
                WriteToLog(QtDebugMsg, "Context Menu <- NULL");
                #endif
                return;
            }

            if(selected==cutBGO)
            {
                //scene->doCut = true ;
                MainWinConnect::pMainWin->on_actionCut_triggered();
            }
            else
            if(selected==copyBGO)
            {
                //scene->doCopy = true ;
                MainWinConnect::pMainWin->on_actionCopy_triggered();
            }
            else
            if(selected==remove)
            {
                scene->removeSelectedLvlItems();
            }
            else
                if(selected==ZMode_bg2)
                {
                    foreach(QGraphicsItem * SelItem, scene->selectedItems() )
                    {
                        if(SelItem->data(ITEM_TYPE).toString()=="BGO")
                        {
                            ((ItemBGO *) SelItem)->setZMode(LevelBGO::Background2, ((ItemBGO *) SelItem)->bgoData.z_offset);
                        }
                    }
                }
                else
                if(selected==ZMode_bg1)
                {
                    foreach(QGraphicsItem * SelItem, scene->selectedItems() )
                    {
                        if(SelItem->data(ITEM_TYPE).toString()=="BGO")
                        {
                            ((ItemBGO *) SelItem)->setZMode(LevelBGO::Background1, ((ItemBGO *) SelItem)->bgoData.z_offset);
                        }
                    }
                }
                else
                if(selected==ZMode_def)
                {
                    foreach(QGraphicsItem * SelItem, scene->selectedItems() )
                    {
                        if(SelItem->data(ITEM_TYPE).toString()=="BGO")
                        {
                            ((ItemBGO *) SelItem)->setZMode(LevelBGO::ZDefault, ((ItemBGO *) SelItem)->bgoData.z_offset);
                        }
                    }
                }
                else
                if(selected==ZMode_fg1)
                {
                    foreach(QGraphicsItem * SelItem, scene->selectedItems() )
                    {
                        if(SelItem->data(ITEM_TYPE).toString()=="BGO")
                        {
                            ((ItemBGO *) SelItem)->setZMode(LevelBGO::Foreground1, ((ItemBGO *) SelItem)->bgoData.z_offset);
                        }
                    }
                }
                else
                if(selected==ZMode_fg2)
                {
                    foreach(QGraphicsItem * SelItem, scene->selectedItems() )
                    {
                        if(SelItem->data(ITEM_TYPE).toString()=="BGO")
                        {
                            ((ItemBGO *) SelItem)->setZMode(LevelBGO::Foreground2, ((ItemBGO *) SelItem)->bgoData.z_offset);
                        }
                    }
                }
            else
            if(selected==ZOffset)
            {
                bool ok;
                qreal newzOffset = QInputDialog::getDouble(nullptr, tr("Z-Offset"),
                                                           tr("Please enter the Z-value offset:"),
                                                           bgoData.z_offset,
                                                           -500, 500,7, &ok);
                if(ok)
                foreach(QGraphicsItem * SelItem, scene->selectedItems() )
                {
                    if(SelItem->data(ITEM_TYPE).toString()=="BGO")
                    {
                        ((ItemBGO *) SelItem)->setZMode(((ItemBGO *) SelItem)->bgoData.z_mode, newzOffset);
                    }
                }
            }
            else
            if(selected==props)
            {
                scene->openProps();
            }
            else
            if(selected==newLayer)
            {
                scene->setLayerToSelected();
            }
            else
            {
                //Fetch layers menu
                foreach(QAction * lItem, layerItems)
                {
                    if(selected==lItem)
                    {
                        //FOUND!!!
                        scene->setLayerToSelected(lItem->data().toString());
                        break;
                    }//Find selected layer's item
                }
            }
        }

    }

}

void ItemBGO::contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
{
//    else
//    {
        QGraphicsItem::contextMenuEvent(event);
//    }
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
    bgoData.x = qRound(this->scenePos().x());
    bgoData.y = qRound(this->scenePos().y());
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

void ItemBGO::setZMode(int mode, qreal offset, bool init)
{
    bgoData.z_mode = mode;
    bgoData.z_offset = offset;

    qreal targetZ=0;
    switch(zMode)
    {
        case -1:
            targetZ = scene->Z_BGOBack2 + zOffset + bgoData.z_offset; break;
        case 1:
            targetZ = scene->Z_BGOFore1 + zOffset + bgoData.z_offset; break;
        case 2:
            targetZ = scene->Z_BGOFore2 + zOffset + bgoData.z_offset; break;
        case 0:
        default:
            targetZ = scene->Z_BGOBack1 + zOffset + bgoData.z_offset;
    }


    switch(bgoData.z_mode)
    {
        case LevelBGO::Background2:
            targetZ = scene->Z_BGOBack2 + zOffset + bgoData.z_offset; break;
        case LevelBGO::Background1:
            targetZ = scene->Z_BGOBack1 + zOffset + bgoData.z_offset; break;
        case LevelBGO::Foreground1:
            targetZ = scene->Z_BGOFore1 + zOffset + bgoData.z_offset; break;
        case LevelBGO::Foreground2:
            targetZ = scene->Z_BGOFore2 + zOffset + bgoData.z_offset; break;
        default:
        break;
    }
    setZValue(targetZ);

    if(!init) arrayApply();
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

    this->setData(ITEM_WIDTH,  QString::number(qRound(imageSize.width())) ); //width
    this->setData(ITEM_HEIGHT, QString::number(qRound(imageSize.height())) ); //height

    //WriteToLog(QtDebugMsg, QString("BGO Animator ID: %1").arg(aniID));

    animatorID = aniID;
}
