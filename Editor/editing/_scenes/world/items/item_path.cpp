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

#include <common_features/logger.h>
#include <common_features/mainwinconnect.h>

#include "item_path.h"

ItemPath::ItemPath(QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    construct();
}

ItemPath::ItemPath(WldScene *parentScene, QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    construct();
    if(!parentScene) return;
    setScenePoint(parentScene);
    scene->addItem(this);
    setZValue(scene->pathZ);
}

void ItemPath::construct()
{
    gridSize=32;
    gridOffsetX=0;
    gridOffsetY=0;
    isLocked=false;

    animatorID=-1;
    imageSize = QRectF(0,0,10,10);

    mouseLeft=false;
    mouseMid=false;
    mouseRight=false;

    setData(ITEM_TYPE, "PATH");
    setData(ITEM_IS_ITEM, 1);
}


ItemPath::~ItemPath()
{}

void ItemPath::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent )
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


void ItemPath::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
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
        if((!scene->lock_path)&&(!scene->DrawMode)&&(!isLocked))
        {
            scene->contextMenuOpened = true; //bug protector
            //Remove selection from non-bgo items
            if(!this->isSelected())
            {
                scene->clearSelection();
                this->setSelected(true);
            }

            this->setSelected(true);
            QMenu ItemMenu;

            QAction *copyTile = ItemMenu.addAction(tr("Copy"));
            QAction *cutTile = ItemMenu.addAction(tr("Cut"));
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
            if(selected==remove)
            {
               scene->removeSelectedWldItems();
            }
        }

    }
}

void ItemPath::contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
{
    QGraphicsItem::contextMenuEvent(event);
}


///////////////////MainArray functions/////////////////////////////
//void ItemPath::setLayer(QString layer)
//{
//    foreach(LevelLayers lr, scene->WldData->layers)
//    {
//        if(lr.name==layer)
//        {
//            pathData.layer = layer;
//            this->setVisible(!lr.hidden);
//            arrayApply();
//        break;
//        }
//    }
//}

void ItemPath::transformTo(long target_id)
{
    if(target_id<1) return;

    bool noimage=true;
    long item_i=0;
    long animator=0;
    obj_w_path mergedSet;

    //Get Path settings
    scene->getConfig_Path(target_id, item_i, animator, mergedSet, &noimage);

    if(noimage)
        return;//Don't transform, target item is not found

    pathData.id = target_id;
    setPathData(pathData, &mergedSet, &animator);
    arrayApply();

    if(!scene->opts.animationEnabled)
        scene->update();
}


void ItemPath::arrayApply()
{
    bool found=false;

    pathData.x = qRound(this->scenePos().x());
    pathData.y = qRound(this->scenePos().y());

    if(pathData.index < (unsigned int)scene->WldData->paths.size())
    { //Check index
        if(pathData.array_id == scene->WldData->paths[pathData.index].array_id)
        {
            found=true;
        }
    }

    //Apply current data in main array
    if(found)
    { //directlry
        scene->WldData->paths[pathData.index] = pathData; //apply current pathData
    }
    else
    for(int i=0; i<scene->WldData->paths.size(); i++)
    { //after find it into array
        if(scene->WldData->paths[i].array_id == pathData.array_id)
        {
            pathData.index = i;
            scene->WldData->paths[i] = pathData;
            break;
        }
    }
}

void ItemPath::removeFromArray()
{
    bool found=false;
    if(pathData.index < (unsigned int)scene->WldData->paths.size())
    { //Check index
        if(pathData.array_id == scene->WldData->paths[pathData.index].array_id)
        {
            found=true;
        }
    }

    if(found)
    { //directlry
        scene->WldData->paths.remove(pathData.index);
    }
    else
    for(int i=0; i<scene->WldData->paths.size(); i++)
    {
        if(scene->WldData->paths[i].array_id == pathData.array_id)
        {
            scene->WldData->paths.remove(i); break;
        }
    }
}

void ItemPath::returnBack()
{
    setPos(pathData.x, pathData.y);
}

int ItemPath::getGridSize()
{
    return gridSize;
}

QPoint ItemPath::sourcePos()
{
    return QPoint(pathData.x, pathData.y);
}

void ItemPath::setPathData(WorldPaths inD, obj_w_path *mergedSet, long *animator_id)
{
    pathData = inD;
    setPos(pathData.x, pathData.y);

    setData(ITEM_ID, QString::number(pathData.id) );
    setData(ITEM_ARRAY_ID, QString::number(pathData.array_id) );

    if(mergedSet)
    {
        localProps = *mergedSet;
        gridSize = localProps.grid;
    }
    if(animator_id)
        setAnimator(*animator_id);
}


QRectF ItemPath::boundingRect() const
{
    return imageSize;
}

void ItemPath::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if(animatorID<0)
    {
        painter->drawRect(QRect(0,0,1,1));
        return;
    }
    if(scene->animates_Paths.size()>animatorID)
        painter->drawPixmap(imageSize, scene->animates_Paths[animatorID]->image(), imageSize);
    else
        painter->drawRect(QRect(0,0,32,32));

    if(this->isSelected())
    {
        painter->setPen(QPen(QBrush(Qt::black), 2, Qt::SolidLine));
        painter->drawRect(1,1,imageSize.width()-2,imageSize.height()-2);
        painter->setPen(QPen(QBrush(Qt::blue), 2, Qt::DotLine));
        painter->drawRect(1,1,imageSize.width()-2,imageSize.height()-2);
    }
}

void ItemPath::setScenePoint(WldScene *theScene)
{
    scene = theScene;
}


////////////////Animation///////////////////

void ItemPath::setAnimator(long aniID)
{
    if(aniID<scene->animates_Paths.size())
    imageSize = QRectF(0,0,
                scene->animates_Paths[aniID]->image().width(),
                scene->animates_Paths[aniID]->image().height()
                );

    this->setData(ITEM_WIDTH, QString::number(qRound(imageSize.width())) ); //width
    this->setData(ITEM_HEIGHT, QString::number(qRound(imageSize.height())) ); //height
    //WriteToLog(QtDebugMsg, QString("Tile Animator ID: %1").arg(aniID));

    animatorID = aniID;
}

