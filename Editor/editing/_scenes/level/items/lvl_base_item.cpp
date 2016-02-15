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

#include "lvl_base_item.h"
#include "../lvl_scene.h"

LvlBaseItem::LvlBaseItem(QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    scene=NULL;
    construct();
}

LvlBaseItem::LvlBaseItem(LvlScene *parentScene, QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    scene=parentScene;
    construct();
}

LvlBaseItem::~LvlBaseItem()
{}

void LvlBaseItem::construct()
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

    setData(ITEM_IS_ITEM, 1);
}

void LvlBaseItem::arrayApply()
{}

void LvlBaseItem::returnBack()
{}

void LvlBaseItem::removeFromArray()
{}

void LvlBaseItem::setLayer(QString layer)
{
    Q_UNUSED(layer);
}

int LvlBaseItem::getGridSize()
{
    return 1;
}

QPoint LvlBaseItem::gridOffset()
{
    return QPoint(0,0);
}

QPoint LvlBaseItem::sourcePos()
{
    return QPoint(0,0);
}

void LvlBaseItem::contextMenu(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent);
}

void LvlBaseItem::setLocked(bool lock)
{
    this->setFlag(QGraphicsItem::ItemIsSelectable, !lock);
    this->setFlag(QGraphicsItem::ItemIsMovable, !lock);
    isLocked = lock;
}

bool LvlBaseItem::itemTypeIsLocked()
{
    return false;
}

void LvlBaseItem::setScenePoint(LvlScene *theScene)
{
    scene = theScene;
}

void LvlBaseItem::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent )
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
        mouseEvent->accept();
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

void LvlBaseItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
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

    if( mouseEvent->button()==Qt::LeftButton )
        mouseLeft=false;

    if( mouseEvent->button()==Qt::MiddleButton )
        mouseMid=false;

    if( mouseEvent->button()==Qt::RightButton )
    {
        callContext=true;
        mouseRight=false;
    }

    QGraphicsItem::mouseReleaseEvent(mouseEvent);

    /////////////////////////CONTEXT MENU:///////////////////////////////
    if((callContext)&&(!scene->contextMenuOpened))
    {
        if((!itemTypeIsLocked())&&(!scene->DrawMode)&&(!isLocked))
        {
            contextMenu(mouseEvent);
        }
    }
}
