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

#include <editing/_dialogs/itemselectdialog.h>
#include <common_features/util.h>
#include <common_features/mainwinconnect.h>
#include <common_features/logger.h>

#include "item_tile.h"

ItemTile::ItemTile(QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    construct();

}

ItemTile::ItemTile(WldScene *parentScene, QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    construct();
    if(!parentScene) return;
    setScenePoint(parentScene);
    scene->addItem(this);
    setZValue(scene->tileZ);
}

void ItemTile::construct()
{
    gridSize=32;
    isLocked=false;

    animatorID=-1;
    imageSize = QRectF(0,0,10,10);

    mouseLeft=false;
    mouseMid=false;
    mouseRight=false;

    setData(ITEM_TYPE, "TILE");
    setData(ITEM_IS_ITEM, 1);
}

ItemTile::~ItemTile()
{
    //WriteToLog(QtDebugMsg, "!<-BGO destroyed->!");
    //if(timer) delete timer;
}



void ItemTile::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
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

void ItemTile::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
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
        if((!scene->lock_tile)&&(!scene->DrawMode)&&(!isLocked))
        {
            contextMenu(mouseEvent);
        }
    }
}

void ItemTile::contextMenu( QGraphicsSceneMouseEvent * mouseEvent )
{
    scene->contextMenuOpened = true; //bug protector
    //Remove selection from non-bgo items
    if(!this->isSelected())
    {
        scene->clearSelection();
        this->setSelected(true);
    }

    this->setSelected(this);
    QMenu ItemMenu;

    QAction *copyTile = ItemMenu.addAction(tr("Copy"));
    QAction *cutTile = ItemMenu.addAction(tr("Cut"));
        ItemMenu.addSeparator();
    QAction *transform = ItemMenu.addAction(tr("Transform into"));
    QAction *transform_all = ItemMenu.addAction(tr("Transform all %1 into").arg("TILE-%1").arg(tileData.id));
        ItemMenu.addSeparator();
    QAction *remove = ItemMenu.addAction(tr("Remove"));

QAction *selected = ItemMenu.exec(mouseEvent->screenPos());

    if(!selected)
    {
        #ifdef _DEBUG_
        WriteToLog(QtDebugMsg, "Context Menu <- NULL");
        #endif
        return;
    }

    if(selected==cutTile)
    {
        MainWinConnect::pMainWin->on_actionCut_triggered();
    }
    else
    if(selected==copyTile)
    {
        MainWinConnect::pMainWin->on_actionCopy_triggered();
    }
    else
    if(selected==transform)
    {
        WorldData HistoryOldData;
        WorldData HistoryNewData;
        int transformTO;
        ItemSelectDialog * tileList = new ItemSelectDialog(scene->pConfigs, ItemSelectDialog::TAB_TILE);
        tileList->removeEmptyEntry(ItemSelectDialog::TAB_TILE);
        util::DialogToCenter(tileList, true);
        if(tileList->exec()==QDialog::Accepted)
        {
            transformTO = tileList->tileID;
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(ITEM_TYPE).toString()=="TILE")
                {
                    HistoryOldData.tiles.push_back( ((ItemTile *) SelItem)->tileData );
                    ((ItemTile *) SelItem)->transformTo(transformTO);
                    HistoryNewData.tiles.push_back( ((ItemTile *) SelItem)->tileData );
                }
            }
        }
        delete tileList;
        if(!HistoryNewData.tiles.isEmpty())
            scene->addTransformHistory(HistoryNewData, HistoryOldData);
    }
    else
    if(selected==transform_all)
    {
        WorldData HistoryOldData;
        WorldData HistoryNewData;
        int transformTO;
        ItemSelectDialog * tileList = new ItemSelectDialog(scene->pConfigs, ItemSelectDialog::TAB_TILE);
        tileList->removeEmptyEntry(ItemSelectDialog::TAB_TILE);
        util::DialogToCenter(tileList, true);
        if(tileList->exec()==QDialog::Accepted)
        {
            transformTO = tileList->tileID;
            unsigned long oldID = tileData.id;
            foreach(QGraphicsItem * SelItem, scene->items() )
            {
                if(SelItem->data(ITEM_TYPE).toString()=="TILE")
                {
                    if(((ItemTile *) SelItem)->tileData.id==oldID)
                    {
                        HistoryOldData.tiles.push_back( ((ItemTile *) SelItem)->tileData );
                        ((ItemTile *) SelItem)->transformTo(transformTO);
                        HistoryNewData.tiles.push_back( ((ItemTile *) SelItem)->tileData );
                    }
                }
            }
        }
        delete tileList;
        if(!HistoryNewData.tiles.isEmpty())
            scene->addTransformHistory(HistoryNewData, HistoryOldData);
    }
    else
    if(selected==remove)
    {
       scene->removeSelectedWldItems();
    }
}


///////////////////MainArray functions/////////////////////////////
//void ItemTile::setLayer(QString layer)
//{
//    foreach(LevelLayers lr, scene->WldData->layers)
//    {
//        if(lr.name==layer)
//        {
//            tileData.layer = layer;
//            this->setVisible(!lr.hidden);
//            arrayApply();
//        break;
//        }
//    }
//}

void ItemTile::transformTo(long target_id)
{
    if(target_id<1) return;

    bool noimage=true;
    long item_i=0;
    long animator=0;
    obj_w_tile mergedSet;

    //Get Tile settings
    scene->getConfig_Tile(target_id, item_i, animator, mergedSet, &noimage);

    if(noimage)
        return;//Don't transform, target item is not found

    tileData.id = target_id;
    setTileData(tileData, &mergedSet, &animator);
    arrayApply();

    if(!scene->opts.animationEnabled)
        scene->update();
}

void ItemTile::arrayApply()
{
    bool found=false;
    tileData.x = qRound(this->scenePos().x());
    tileData.y = qRound(this->scenePos().y());

    if(tileData.index < (unsigned int)scene->WldData->tiles.size())
    { //Check index
        if(tileData.array_id == scene->WldData->tiles[tileData.index].array_id)
        {
            found=true;
        }
    }

    //Apply current data in main array
    if(found)
    { //directlry
        scene->WldData->tiles[tileData.index] = tileData; //apply current tileData
    }
    else
    for(int i=0; i<scene->WldData->tiles.size(); i++)
    { //after find it into array
        if(scene->WldData->tiles[i].array_id == tileData.array_id)
        {
            tileData.index = i;
            scene->WldData->tiles[i] = tileData;
            break;
        }
    }
}

void ItemTile::removeFromArray()
{
    bool found=false;
    if(tileData.index < (unsigned int)scene->WldData->tiles.size())
    { //Check index
        if(tileData.array_id == scene->WldData->tiles[tileData.index].array_id)
        {
            found=true;
        }
    }

    if(found)
    { //directlry
        scene->WldData->tiles.removeAt(tileData.index);
    }
    else
    for(int i=0; i<scene->WldData->tiles.size(); i++)
    {
        if(scene->WldData->tiles[i].array_id == tileData.array_id)
        {
            scene->WldData->tiles.removeAt(i); break;
        }
    }
}

void ItemTile::returnBack()
{
    setPos(tileData.x, tileData.y);
}

int ItemTile::getGridSize()
{
    return gridSize;
}

QPoint ItemTile::sourcePos()
{
    return QPoint(tileData.x, tileData.y);
}


void ItemTile::setTileData(WorldTiles inD, obj_w_tile *mergedSet, long *animator_id)
{
    tileData = inD;
    setPos(tileData.x, tileData.y);

    if(mergedSet)
    {
        localProps = *mergedSet;
        gridSize = localProps.grid;
    }
    if(animator_id)
        setAnimator(*animator_id);

    setData(ITEM_ID, QString::number(tileData.id) );
    setData(ITEM_ARRAY_ID, QString::number(tileData.array_id) );
}


QRectF ItemTile::boundingRect() const
{
    return imageSize;
}

void ItemTile::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if(animatorID<0)
    {
        painter->drawRect(QRect(0,0,1,1));
        return;
    }
    if(scene->animates_Tiles.size()>animatorID)
        painter->drawPixmap(imageSize, scene->animates_Tiles[animatorID]->image(), imageSize);
    else
        painter->drawRect(QRect(0,0,32,32));

    if(this->isSelected())
    {
        painter->setPen(QPen(QBrush(Qt::black), 2, Qt::SolidLine));
        painter->drawRect(1,1,imageSize.width()-2,imageSize.height()-2);
        painter->setPen(QPen(QBrush(Qt::green), 2, Qt::DotLine));
        painter->drawRect(1,1,imageSize.width()-2,imageSize.height()-2);
    }
}

void ItemTile::setScenePoint(WldScene *theScene)
{
    scene = theScene;
}


////////////////Animation///////////////////

void ItemTile::setAnimator(long aniID)
{
    if(aniID<scene->animates_Tiles.size())
    imageSize = QRectF(0,0,
                scene->animates_Tiles[aniID]->image().width(),
                scene->animates_Tiles[aniID]->image().height()
                );

    setData(ITEM_WIDTH, QString::number(qRound(imageSize.width())) ); //width
    setData(ITEM_HEIGHT, QString::number(qRound(imageSize.height())) ); //height
    //WriteToLog(QtDebugMsg, QString("Tile Animator ID: %1").arg(aniID));

    animatorID = aniID;
}
