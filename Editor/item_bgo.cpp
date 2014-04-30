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

#include "item_bgo.h"
#include "logger.h"


ItemBGO::ItemBGO(QGraphicsPixmapItem *parent)
    : QGraphicsPixmapItem(parent)
{
    animated = false;
    frameFirst=0; //from first frame
    frameLast=-1; //to unlimited frameset
    gridSize=32;
    gridOffsetX=0;
    gridOffsetY=0;
    //image = new QGraphicsPixmapItem;
}


ItemBGO::~ItemBGO()
{
 //   WriteToLog(QtDebugMsg, "!<-Block destroyed->!");
}

void ItemBGO::contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
{
    if(scene->lock_bgo) return;

    scene->clearSelection();
    this->setSelected(1);
    ItemMenu->clear();

    QAction *remove = ItemMenu->addAction("Remove");

    QAction *selected = ItemMenu->exec(event->screenPos());

    if(selected==remove)
    {
        removeFromArray();
        scene->removeItem(this);
    }
}


///////////////////MainArray functions/////////////////////////////
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

void ItemBGO::setMainPixmap(const QPixmap &pixmap)
{
    mainImage = pixmap;
    this->setPixmap(mainImage);
}

void ItemBGO::setBGOData(LevelBGO inD)
{
    bgoData = inD;
}


QRectF ItemBGO::boundingRect() const
{
    if(!animated)
        return QRectF(0,0,mainImage.width(),mainImage.height());
    else
        return QRectF(0,0,frameWidth,frameSize);
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


void ItemBGO::setAnimation(int frames, int framespeed)
{
    animated = true;
    framesQ = frames;
    frameSpeed = framespeed;

    frameSize = (int)round(mainImage.height()/frames);
    frameWidth = mainImage.width();
    frameHeight = mainImage.height();

    framePos = QPoint(0,0);
    draw();

    setFrame(frameFirst);

    timer = new QTimer(this);
    connect(
                timer, SIGNAL(timeout()),
                this,
                SLOT( nextFrame() ) );
}

void ItemBGO::AnimationStart()
{
    timer->start(frameSpeed);
}

void ItemBGO::AnimationStop()
{
    timer->stop();
    setFrame(frameFirst);
}

void ItemBGO::draw()
{
    currentImage =  mainImage.copy(QRect(framePos.x(), framePos.y(), frameWidth, frameSize ));
}

QPoint ItemBGO::fPos() const
{
    return framePos;
}

void ItemBGO::setFrame(int y)
{
    frameCurrent = frameSize * y;
    if ( ((frameCurrent >= frameHeight )&&(frameLast==-1)) ||
         ((frameCurrent >= frameLast*frameSize )&&(frameLast>-1)) )
        {
        frameCurrent = frameFirst*frameSize;
        framePos.setY( frameFirst * frameSize );
        }
    else
    framePos.setY( frameCurrent );
    draw();
    this->setPixmap(QPixmap(currentImage));
}

void ItemBGO::nextFrame()
{
    frameCurrent += frameSize;
    if ( ((frameCurrent >= frameHeight )&&(frameLast==-1)) ||
         ((frameCurrent >= frameLast*frameSize )&&(frameLast>-1)) )
        {
        frameCurrent = frameFirst*frameSize;
        framePos.setY( frameFirst * frameSize );
        }
    else
    framePos.setY( framePos.y() + frameSize );
    draw();
    this->setPixmap(QPixmap(currentImage));
}
