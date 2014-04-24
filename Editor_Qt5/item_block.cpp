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


ItemBlock::ItemBlock(QGraphicsPixmapItem *parent)
    : QGraphicsPixmapItem(parent)
{
    animated = false;
    //image = new QGraphicsPixmapItem;
}


ItemBlock::~ItemBlock()
{

}

void ItemBlock::contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
{
    if(animated) nextFrame();

    ItemMenu->exec(event->screenPos());
}

void ItemBlock::setMainPixmap(const QPixmap &pixmap)
{
    mainImage = pixmap;
    this->setPixmap(mainImage);
}


QRectF ItemBlock::boundingRect() const
{
    if(!animated)
        return QRectF(0,0,mainImage.width(),mainImage.height());
    else
        return QRectF(0,0,frameWidth,frameSize);
}

void ItemBlock::setContextMenu(QMenu &menu)
{
    ItemMenu = &menu;
}


////////////////Animation///////////////////


void ItemBlock::setAnimation(QPixmap &sprite, int frames)
{
    animated = true;
    farameSet=sprite;
    framesQ=frames;

    frameSize = (int)round(farameSet.height()/frames);
    frameWidth = farameSet.width();
    frameHeight = farameSet.height();

    framePos = QPoint(0,0);
    draw();
    setFrame(0);

    timer = new QTimer(this);
    connect(
                timer, SIGNAL(timeout()),
                this,
                SLOT( nextFrame() ) );
    timer->start(128);
}

void ItemBlock::draw()
{
    currentImage =  farameSet.copy(QRect(framePos.x(), framePos.y(), frameWidth, frameSize ));
}

QPoint ItemBlock::fPos() const
{
    return framePos;
}

void ItemBlock::setFrame(int y)
{
    frameCurrent = frameSize * y;
    if (frameCurrent >= frameHeight )
        {
        frameCurrent = 0;
        framePos.setY( 0 );
        }
    else
    framePos.setY( frameCurrent );
    draw();
    this->setPixmap(QPixmap(currentImage));
}

void ItemBlock::nextFrame()
{
    frameCurrent += frameSize;
    if (frameCurrent >= frameHeight )
        {
        frameCurrent = 0;
        framePos.setY( 0 );
        }
    else
    framePos.setY( framePos.y() + frameSize );
    draw();
    this->setPixmap(QPixmap(currentImage));
}
