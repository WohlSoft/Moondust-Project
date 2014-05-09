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

#include "item_npc.h"
#include "logger.h"



ItemNPC::ItemNPC(QGraphicsPixmapItem *parent)
    : QGraphicsPixmapItem(parent)
{
    animated = false;
    frameFirst=0; //from first frame
    frameLast=-1; //to unlimited frameset
    //image = new QGraphicsPixmapItem;
}


ItemNPC::~ItemNPC()
{
 //   WriteToLog(QtDebugMsg, "!<-Block destroyed->!");
}

void ItemNPC::contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
{
    if(!scene->lock_npc)
    {
        //Remove selection from non-block items
        if(this->isSelected())
        {
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(0).toString()!="Block") SelItem->setSelected(false);
            }
        }
        else
        {
            scene->clearSelection();
            this->setSelected(true);
        }


        this->setSelected(1);
        ItemMenu->clear();
        QAction * LayerName = ItemMenu->addAction(tr("Layer: ")+QString("[%1]").arg(npcData.layer));
            LayerName->setEnabled(false);

        ItemMenu->addSeparator();

        QAction *fri = ItemMenu->addAction("Friendly");
            fri->setCheckable(1);
            fri->setChecked( npcData.friendly );

        QAction *stat = ItemMenu->addAction("Not movable");
            stat->setCheckable(1);
            stat->setChecked( npcData.nomove );

        ItemMenu->addSeparator();

        QAction *boss = ItemMenu->addAction("Legacy boss");
            boss->setCheckable(1);
            boss->setChecked( npcData.legacyboss );

        ItemMenu->addSeparator();

        QAction *copyNpc = ItemMenu->addAction("Copy");
        QAction *cutNpc = ItemMenu->addAction("Cut");
        ItemMenu->addSeparator();
        QAction *remove = ItemMenu->addAction("Remove");

        QAction *selected = ItemMenu->exec(event->screenPos());

        if(selected==cutNpc)
        {
            scene->doCut = true ;
        }
        else
        if(selected==copyNpc)
        {
            scene->doCopy = true ;
        }
        else
        if(selected==fri)
        {
            //apply to all selected items.
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(0).toString()=="NPC")
                    ((ItemNPC *) SelItem)->setFriendly(fri->isChecked());
            }
        }
        else
        if(selected==stat)
        {
            //apply to all selected items.
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(0).toString()=="NPC")
                    ((ItemNPC *) SelItem)->setNoMovable(stat->isChecked());
            }
        }
        else
        if(selected==boss)
        {
            //apply to all selected items.
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(0).toString()=="NPC")
                    ((ItemNPC *) SelItem)->setLegacyBoss(boss->isChecked());
            }
        }
        else
        if(selected==remove)
        {
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(0).toString()=="NPC")
                {
                    ((ItemNPC *)SelItem)->removeFromArray();
                    scene->removeItem(SelItem);
                }
            }
        }
    }
}

//Change arrtibutes
void ItemNPC::setFriendly(bool fri)
{
    npcData.friendly=fri;
    arrayApply(); //Apply changes into array
}

void ItemNPC::setNoMovable(bool stat)
{
    npcData.nomove=stat;
    arrayApply();//Apply changes into array
}

void ItemNPC::setLegacyBoss(bool boss)
{
    npcData.legacyboss=boss;
    arrayApply();//Apply changes into array
}

///////////////////MainArray functions/////////////////////////////
void ItemNPC::arrayApply()
{
    bool found=false;

    if(npcData.index < (unsigned int)scene->LvlData->npc.size())
    { //Check index
        if(npcData.array_id == scene->LvlData->npc[npcData.index].array_id)
            found=true;
    }

    //Apply current data in main array
    if(found)
    { //directlry
        scene->LvlData->npc[npcData.index] = npcData; //apply current npcData
    }
    else
    for(int i=0; i<scene->LvlData->npc.size(); i++)
    { //after find it into array
        if(scene->LvlData->npc[i].array_id == npcData.array_id)
        {
            npcData.index = i;
            scene->LvlData->npc[i] = npcData;
            break;
        }
    }
}

void ItemNPC::removeFromArray()
{
    bool found=false;
    if(npcData.index < (unsigned int)scene->LvlData->npc.size())
    { //Check index
        if(npcData.array_id == scene->LvlData->npc[npcData.index].array_id)
            found=true;
    }
    if(found)
    { //directlry
        scene->LvlData->npc.remove(npcData.index);
    }
    else
    for(int i=0; i<scene->LvlData->npc.size(); i++)
    {
        if(scene->LvlData->npc[i].array_id == npcData.array_id)
        {
            scene->LvlData->npc.remove(i); break;
        }
    }
}

/*
void ItemNPC::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    int gridSize=32, offsetX=0, offsetY=0, gridX, gridY, i;
    QPoint sourcePos;

    sourcePos=QPoint(npcData.x, npcData.y);
    QPointF itemPos = this->scenePos();

    if((!isSelected())||(sourcePos == itemPos))
    { QGraphicsPixmapItem::mouseReleaseEvent(event); return;}

    if(scene->grid)
    { //ATTACH TO GRID
        gridX = ((int)itemPos.x() - (int)itemPos.x() % gridSize);
        gridY = ((int)itemPos.y() - (int)itemPos.y() % gridSize);

        if((int)itemPos.x()<0)
        {
            if( (int)itemPos.x() < gridX - (int)(gridSize/2) )
                gridX -= gridSize;
        }
        else
        {
            if( (int)itemPos.x() > gridX + (int)(gridSize/2) )
                gridX += gridSize;
        }

        if((int)itemPos.y()<0)
        {if( (int)itemPos.y() < gridY - (int)(gridSize/2) )
            gridY -= gridSize;
        }
        else {if( (int)itemPos.y() > gridY + (int)(gridSize/2) )
         gridY += gridSize;
        }

        this->setPos(QPointF(offsetX+gridX, offsetY+gridY));
    }

    //Check collision
    if( scene->itemCollidesWith(this) )
    {
        this->setPos(QPointF(sourcePos));
        this->setSelected(false);
        WriteToLog(QtDebugMsg, QString("Moved back %1 %2")
                   .arg((long)this->scenePos().x())
                   .arg((long)this->scenePos().y()) );
    }
    else
    {
        npcData.x=(long)this->scenePos().x();
        npcData.y=(long)this->scenePos().y();

         for (i=0;i<scene->LvlData->blocks.size();i++)
            {
                if(scene->LvlData->blocks[i].array_id == npcData.array_id)
                {
                    //Applay move into main array
                    scene->LvlData->blocks[i].x = (long)this->scenePos().x();
                    scene->LvlData->blocks[i].y = (long)this->scenePos().y();
                    scene->LvlData->modyfied = true;
                    break;
                }
            }
    }

}*/

void ItemNPC::setMainPixmap(const QPixmap &pixmap)
{
    mainImage = pixmap;
    this->setPixmap(mainImage);
}

void ItemNPC::setNpcData(LevelNPC inD)
{
    npcData = inD;
}


QRectF ItemNPC::boundingRect() const
{
    if(!animated)
        return QRectF(0,0,mainImage.width(),mainImage.height());
    else
        return QRectF(0,0,frameWidth,frameSize);
}

void ItemNPC::setContextMenu(QMenu &menu)
{
    ItemMenu = &menu;
}

void ItemNPC::setScenePoint(LvlScene *theScene)
{
    scene = theScene;
}


////////////////Animation///////////////////


void ItemNPC::setAnimation(int frames, int framespeed, int framestyle, int direct,
                           bool customAnimate, int frFL, int frEL, int frFR, int frER, bool edit)
{
    animated = true;
    framesQ = frames;
    frameSpeed = framespeed;
    frameStyle = framestyle;
    direction = direct;

    frameSize = (int)round(mainImage.height()/frames);
    frameWidth = mainImage.width();
    frameHeight = mainImage.height();

    framePos = QPoint(0,0);
    draw();

    int dir=direction;

    if(direction==0) //if direction=random
    {
        dir=(((bool)qrand()%2)?-1:1); //set randomly 1 or -1
    }


    if(customAnimate) // User defined spriteSet (example: boss)
    {
        switch(dir)
        {
        case -1: //left
            frameFirst = frFL;
            frameLast = frEL;
            break;
        case 1: //Right
            frameFirst = frFR;
            frameLast = frER;
            break;
        default: break;
        }
    }
    else
    {
        switch(frameStyle)
        {
        case 2: //Left-Right-upper sprite
            switch(dir)
            {
            case -1: //left
                frameFirst = 0;
                frameLast = (int)(frames/4)-1;
                break;
            case 1: //Right
                frameFirst = (int)(frames/4);
                frameLast = (int)(frames/2)-1;
                break;
            default: break;
            }
            break;

        case 1: //Left-Right sprite
            switch(dir)
            {
            case -1: //left
                frameFirst = 0;
                frameLast = (int)(frames/2)-1;
                break;
            case 1: //Right
                frameFirst = (int)(frames/2);
                frameLast = frames;
                break;
            default: break;
            }

            break;

        case 0: //Single sprite
        default:
            frameFirst = 0;
            frameLast = -1;
            break;
        }
    }

    setFrame(frameFirst);

    if(!edit)
    {
        timer = new QTimer(this);
        connect(
                    timer, SIGNAL(timeout()),
                    this,
                    SLOT( nextFrame() ) );
    }
}

void ItemNPC::AnimationStart()
{
    if(!animated) return;
    timer->start(frameSpeed);
}

void ItemNPC::AnimationStop()
{
    if(!animated) return;
    timer->stop();
    setFrame(frameFirst);
}

void ItemNPC::draw()
{
    currentImage =  mainImage.copy(QRect(framePos.x(), framePos.y(), frameWidth, frameSize ));
}

QPoint ItemNPC::fPos() const
{
    return framePos;
}

void ItemNPC::setFrame(int y)
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

void ItemNPC::nextFrame()
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
