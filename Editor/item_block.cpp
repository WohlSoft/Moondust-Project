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

#include "item_block.h"
#include "logger.h"



ItemBlock::ItemBlock(QGraphicsPixmapItem *parent)
    : QGraphicsPixmapItem(parent)
{
    animated = false;
    frameFirst=0; //from first frame
    frameLast=-1; //to unlimited frameset
    //image = new QGraphicsPixmapItem;
}


ItemBlock::~ItemBlock()
{
 //   WriteToLog(QtDebugMsg, "!<-Block destroyed->!");
}

void ItemBlock::contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
{
    if(scene->lock_block) return;

    scene->clearSelection();
    this->setSelected(1);
    ItemMenu->clear();
    QAction *invis = ItemMenu->addAction("Invisible");
        invis->setCheckable(1);
        invis->setChecked( blockData.invisible );
    QAction *slipp = ItemMenu->addAction("Slippery");
        slipp->setCheckable(1);
        slipp->setChecked( blockData.slippery );

    QAction *resize = ItemMenu->addAction("Resize");
        resize->setVisible( (this->data(3).toString()=="sizeble") );

    QAction *remove = ItemMenu->addAction("Remove");

    QAction *selected = ItemMenu->exec(event->screenPos());

    if(selected==invis)
    {
        setInvisible(invis->isChecked());
    }else
    if(selected==slipp)
    {
        setSlippery(slipp->isChecked());
    }
    else
    if(selected==remove)
    {
        removeFromArray();
        scene->removeItem(this);
    }
}

void ItemBlock::setSlippery(bool slip)
{
    blockData.slippery=slip;
    arrayApply(); //Apply changes into array
}

void ItemBlock::setInvisible(bool inv)
{
    blockData.invisible=inv;
    if(inv)
        this->setOpacity(0.5);
    else
        this->setOpacity(1);

    arrayApply();//Apply changes into array

}

///////////////////MainArray functions/////////////////////////////
void ItemBlock::arrayApply()
{
    bool found=false;
    if(blockData.index < (unsigned int)scene->LvlData->blocks.size())
    { //Check index
        if(blockData.array_id == scene->LvlData->blocks[blockData.index].array_id)
            found=true;
    }

    //Apply current data in main array
    if(found)
    { //directlry
        scene->LvlData->blocks[blockData.index] = blockData; //apply current blockdata
        WriteToLog(QtDebugMsg, "Applay block data by index");
    }
    else
    foreach(LevelBlock blocks, scene->LvlData->blocks)
    { //after find it into array
        if(blocks.array_id == blockData.array_id)
        {
            blocks = blockData;
            WriteToLog(QtDebugMsg, "Applay block data by touch all in array");
            break;
        }
    }
}

void ItemBlock::removeFromArray()
{
    for(int i=0; i<scene->LvlData->blocks.size(); i++)
    {
        if(scene->LvlData->blocks[i].array_id == blockData.array_id)
        {
            scene->LvlData->blocks.remove(i); break;
        }
    }
}

/*
void ItemBlock::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    int gridSize=32, offsetX=0, offsetY=0, gridX, gridY, i;
    QPoint sourcePos;

    sourcePos=QPoint(blockData.x, blockData.y);
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
        blockData.x=(long)this->scenePos().x();
        blockData.y=(long)this->scenePos().y();

         for (i=0;i<scene->LvlData->blocks.size();i++)
            {
                if(scene->LvlData->blocks[i].array_id == blockData.array_id)
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

void ItemBlock::setMainPixmap(const QPixmap &pixmap)
{
    mainImage = pixmap;
    if(!sizeble)
        this->setPixmap(mainImage);
    else
    {
        frameWidth = blockData.w;
        frameSize = blockData.h;
        frameHeight = blockData.h;
        currentImage = drawSizebleBlock(blockData.w, blockData.h, mainImage);
        this->setPixmap(currentImage);
    }
}

void ItemBlock::setBlockData(LevelBlock inD, bool is_sz)
{
    blockData = inD;
    sizeble = is_sz;
}


QRectF ItemBlock::boundingRect() const
{
    if((!animated)&&(!sizeble))
        return QRectF(0,0,mainImage.width(),mainImage.height());
    else
        return QRectF(0,0,frameWidth,frameSize);
}

void ItemBlock::setContextMenu(QMenu &menu)
{
    ItemMenu = &menu;
}

void ItemBlock::setScenePoint(LvlScene *theScene)
{
    scene = theScene;
}


////////////////Animation///////////////////


void ItemBlock::setAnimation(int frames, int framespeed, int algorithm)
{
    animated = true;
    framesQ = frames;
    frameSpeed = framespeed;

    frameSize = (int)round(mainImage.height()/frames);
    frameWidth = mainImage.width();
    frameHeight = mainImage.height();

    framePos = QPoint(0,0);
    draw();

    if(algorithm == 1) // Invisible block
    {
        frameFirst = 5;
        frameLast = 6;
    }
    else if(algorithm == 3) //Player's character block
    {
        frameFirst = 0;
        frameLast = 1;
    }
    else if(algorithm == 4) //Player's character switch
    {
        frameFirst = 0;
        frameLast = 4;
    }
    else //Default block
    {
        frameFirst = 0;
        frameLast = -1;
    }

    setFrame(frameFirst);

    timer = new QTimer(this);
    connect(
                timer, SIGNAL(timeout()),
                this,
                SLOT( nextFrame() ) );
}

void ItemBlock::AnimationStart()
{
    timer->start(frameSpeed);
}

void ItemBlock::AnimationStop()
{
    timer->stop();
}

void ItemBlock::draw()
{
    currentImage =  mainImage.copy(QRect(framePos.x(), framePos.y(), frameWidth, frameSize ));
}

QPoint ItemBlock::fPos() const
{
    return framePos;
}

void ItemBlock::setFrame(int y)
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

void ItemBlock::nextFrame()
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



//Sizeble Block formula
QPixmap ItemBlock::drawSizebleBlock(int w, int h, QPixmap srcimg)
{
    int x,y, i, j;
    int hc, wc;
    QPixmap img;
    QPixmap * sizebleImage;
    QPainter * szblock;
    x=32;
    y=32;

    sizebleImage = new QPixmap(QSize(w, h));
    sizebleImage->fill(Qt::transparent);
    szblock = new QPainter(sizebleImage);

    //L
    hc=0;
    for(i=0; i<((h-2*y) / y); i++ )
    {
        szblock->drawPixmap(0, x+hc, x, y, srcimg.copy(QRect(0, y, x, y)));
            hc+=x;
    }

    //T
    hc=0;
    for(i=0; i<( (w-2*x) / x); i++ )
    {
        szblock->drawPixmap(x+hc, 0, x, y, srcimg.copy(QRect(x, 0, x, y)) );
            hc+=x;
    }

    //B
    hc=0;
    for(i=0; i< ( (w-2*x) / x); i++ )
    {
        szblock->drawPixmap(x+hc, h-y, x, y, srcimg.copy(QRect(x, srcimg.width()-y, x, y )) );
            hc+=x;
    }

    //R
    hc=0;
    for(i=0; i<((h-2*y) / y); i++ )
    {
        szblock->drawPixmap(w-x, y+hc, x, y, srcimg.copy(QRect(srcimg.width()-x, y, x, y)));
            hc+=x;
    }

    //C
    hc=0;
    wc=0;
    for(i=0; i<((h-2*y) / y); i++ )
    {
        hc=0;
        for(j=0; j<((w-2*x) / x); j++ )
        {
        szblock->drawPixmap(x+hc, y+wc, x, y, srcimg.copy(QRect(x, y, x, y)));
            hc+=x;
        }
        wc+=y;
    }

    //Applay Sizeble formula
     //1
    szblock->drawPixmap(0,0,y,x, srcimg.copy(QRect(0,0,y,x)));
     //2
    szblock->drawPixmap(w-y, 0, y, x, srcimg.copy(QRect(srcimg.width()-y, 0, y, x)) );
     //3
    szblock->drawPixmap(w-y, h-x, y, x, srcimg.copy(QRect(srcimg.width()-y, srcimg.height()-x, y, x)) );
     //4
    szblock->drawPixmap(0, h-x, y, x, srcimg.copy(QRect(0, srcimg.height()-x, y, x)) );

    img = QPixmap( * sizebleImage);
    return img;
}
