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

#include <common_features/mainwinconnect.h>
#include <common_features/logger.h>

#include "item_music.h"

ItemMusic::ItemMusic(QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    gridSize=32;
    gridOffsetX=0;
    gridOffsetY=0;
    isLocked=false;

    animatorID=-1;
    musicTitle = "";
    scene=NULL;
    imageSize = QRectF(0,0,32,32);

    this->setData(ITEM_WIDTH, QString::number( gridSize ) ); //width
    this->setData(ITEM_HEIGHT, QString::number( gridSize ) ); //height

    mouseLeft=false;
    mouseMid=false;
    mouseRight=false;
}


ItemMusic::~ItemMusic()
{
    //WriteToLog(QtDebugMsg, "!<-BGO destroyed->!");
    //if(timer) delete timer;
}

void ItemMusic::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent )
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

void ItemMusic::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
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
        if((!scene->lock_musbox)&&(!scene->DrawMode)&&(!isLocked))
        {
            scene->contextMenuOpened = true;
            //Remove selection from non-bgo items
            if(!this->isSelected())
            {
                scene->clearSelection();
                this->setSelected(true);
            }

            this->setSelected(1);
            ItemMenu->clear();

            if(!musicTitle.isEmpty())
            {
                QAction *title = ItemMenu->addAction(QString("[%1]").arg(musicTitle));
                title->setEnabled(false);
                title->deleteLater();
            }

            QAction *play = ItemMenu->addAction(tr("Play this"));
            play->deleteLater();
            ItemMenu->addSeparator()->deleteLater();
            QAction *copyTile = ItemMenu->addAction(tr("Copy"));
            copyTile->deleteLater();
            QAction *cutTile = ItemMenu->addAction(tr("Cut"));
            cutTile->deleteLater();
            ItemMenu->addSeparator()->deleteLater();
            QAction *remove = ItemMenu->addAction(tr("Remove"));
            remove->deleteLater();

    QAction *selected = ItemMenu->exec(mouseEvent->screenPos());

            if(!selected)
            {
                #ifdef _DEBUG_
                WriteToLog(QtDebugMsg, "Context Menu <- NULL");
                #endif
                return;
            }

            if(selected==play)
            {
                MainWinConnect::pMainWin->activeWldEditWin()->currentMusic = musicData.id;
                MainWinConnect::pMainWin->setMusicButton(true);
                MainWinConnect::pMainWin->on_actionPlayMusic_triggered(true);
            }
            else
            if(selected==cutTile)
            {
                //scene->doCut = true ;
                MainWinConnect::pMainWin->on_actionCut_triggered();
            }
            else
            if(selected==copyTile)
            {
                //scene->doCopy = true ;
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

void ItemMusic::contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
{
    QGraphicsItem::contextMenuEvent(event);
}


///////////////////MainArray functions/////////////////////////////
//void ItemMusic::setLayer(QString layer)
//{
//    foreach(LevelLayers lr, scene->WldData->layers)
//    {
//        if(lr.name==layer)
//        {
//            musicData.layer = layer;
//            this->setVisible(!lr.hidden);
//            arrayApply();
//        break;
//        }
//    }
//}

void ItemMusic::arrayApply()
{
    bool found=false;
    musicData.x = qRound(this->scenePos().x());
    musicData.y = qRound(this->scenePos().y());

    if(musicData.index < (unsigned int)scene->WldData->music.size())
    { //Check index
        if(musicData.array_id == scene->WldData->music[musicData.index].array_id)
        {
            found=true;
        }
    }

    //Apply current data in main array
    if(found)
    { //directlry
        scene->WldData->music[musicData.index] = musicData; //apply current musicData
    }
    else
    for(int i=0; i<scene->WldData->music.size(); i++)
    { //after find it into array
        if(scene->WldData->music[i].array_id == musicData.array_id)
        {
            musicData.index = i;
            scene->WldData->music[i] = musicData;
            break;
        }
    }
}

void ItemMusic::removeFromArray()
{
    bool found=false;
    if(musicData.index < (unsigned int)scene->WldData->music.size())
    { //Check index
        if(musicData.array_id == scene->WldData->music[musicData.index].array_id)
        {
            found=true;
        }
    }

    if(found)
    { //directlry
        scene->WldData->music.remove(musicData.index);
    }
    else
    for(int i=0; i<scene->WldData->music.size(); i++)
    {
        if(scene->WldData->music[i].array_id == musicData.array_id)
        {
            scene->WldData->music.remove(i); break;
        }
    }
}

void ItemMusic::setMusicData(WorldMusic inD)
{
    musicData = inD;
}


QRectF ItemMusic::boundingRect() const
{
    return imageSize;
}

void ItemMusic::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if(scene==NULL)
    {
        painter->setPen(QPen(QBrush(Qt::yellow), 2, Qt::SolidLine));
        painter->setBrush(Qt::yellow);
        painter->setOpacity(0.5);
        painter->drawRect(1,1,imageSize.width()-2,imageSize.height()-2);
        painter->setOpacity(1);
        painter->setBrush(Qt::transparent);
        painter->drawRect(1,1,imageSize.width()-2,imageSize.height()-2);
    }
    else
    {
       painter->drawPixmap(scene->musicBoxImg.rect(), scene->musicBoxImg, scene->musicBoxImg.rect());
    }

    if(this->isSelected())
    {
        painter->setPen(QPen(QBrush(Qt::black), 2, Qt::SolidLine));
        painter->drawRect(1,1,imageSize.width()-2,imageSize.height()-2);
        painter->setPen(QPen(QBrush(Qt::white), 2, Qt::DotLine));
        painter->drawRect(1,1,imageSize.width()-2,imageSize.height()-2);
    }
}

void ItemMusic::setContextMenu(QMenu &menu)
{
    ItemMenu = &menu;
}

void ItemMusic::setScenePoint(WldScene *theScene)
{
    scene = theScene;
}

