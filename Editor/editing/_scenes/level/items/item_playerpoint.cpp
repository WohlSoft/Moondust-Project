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

#include <common_features/themes.h>

#include "item_playerpoint.h"

ItemPlayerPoint::ItemPlayerPoint(QGraphicsItem *parent)
    : LvlBaseItem(parent)
{
    construct();
}

ItemPlayerPoint::ItemPlayerPoint(LvlScene *parentScene, QGraphicsItem *parent)
    : LvlBaseItem(parentScene, parent)
{
    construct();
    if(!parentScene) return;
    setScenePoint(parentScene);
    scene->addItem(this);
}

void ItemPlayerPoint::construct()
{
    _offset_x=0;
    _offset_y=0;
    setData(ITEM_TYPE, "playerPoint");
}

QRectF ItemPlayerPoint::boundingRect() const
{
    return QRectF(_offset_x, _offset_y, currentImage.width(), currentImage.height());
}

void ItemPlayerPoint::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->drawPixmap(_offset_x, _offset_y, _cur.width(), _cur.height(), _cur);
    if(this->isSelected())
    {
        painter->setPen(QPen(QBrush(Qt::black), 2, Qt::SolidLine));
        painter->drawRect(_offset_x+1, _offset_y+1,_cur.width()-2,_cur.height()-2);
        painter->setPen(QPen(QBrush(Qt::white), 2, Qt::DotLine));
        painter->drawRect(_offset_x+1, _offset_y+1,_cur.width()-2,_cur.height()-2);
    }
}

ItemPlayerPoint::~ItemPlayerPoint()
{
    scene->unregisterElement(this);
}

void ItemPlayerPoint::contextMenu(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene->DrawMode)
    {
        scene->contextMenuOpened = true; //bug protector
    }

    //Remove selection from non-block items
    if(!this->isSelected())
    {
        scene->clearSelection();
        this->setSelected(true);
    }

    this->setSelected(true);
    QMenu ItemMenu;

    bool isLvlx = !scene->LvlData->smbx64strict;

    QMenu * chDir =     ItemMenu.addMenu(
                        tr("Set %1").arg(tr("Direction")) );
        chDir->setEnabled(isLvlx);

    QAction *setLeft =  chDir->addAction( tr("Left"));
        setLeft->setCheckable(true);
        setLeft->setChecked(pointData.direction==-1);

    QAction *setRight = chDir->addAction( tr("Right") );
        setRight->setCheckable(true);
        setRight->setChecked(pointData.direction==1);

                        ItemMenu.addSeparator();

    QAction * remove =  ItemMenu.addAction(tr("Remove"));

    /*****************Waiting for answer************************/
    QAction *selected = ItemMenu.exec(mouseEvent->screenPos());
    /***********************************************************/

    if(!selected)
        return;


    if(selected==remove)
    {
        scene->removeSelectedLvlItems();
    }
    else
    if(selected==setLeft)
    {
        //LevelData selData;
        foreach(QGraphicsItem * SelItem, scene->selectedItems() )
        {
            if(SelItem->data(ITEM_TYPE).toString()=="playerPoint")
            {
                //selData.npc.push_back(((ItemNPC *) SelItem)->npcData);
                dynamic_cast<ItemPlayerPoint *>(SelItem)->changeDirection(-1);
            }
        }
        //scene->addChangeSettingsHistory(selData, LvlScene::SETTING_DIRECTION, QVariant(-1));
    }
    else
    if(selected==setRight)
    {
        //LevelData selData;
        foreach(QGraphicsItem * SelItem, scene->selectedItems() )
        {
            if(SelItem->data(ITEM_TYPE).toString()=="playerPoint")
            {
                //selData.npc.push_back(((ItemPlayerPoint *) SelItem)->npcData);
                dynamic_cast<ItemPlayerPoint *>(SelItem)->changeDirection(1);
            }
        }
        //scene->addChangeSettingsHistory(selData, LvlScene::SETTING_DIRECTION, QVariant(1));
    }
}


void ItemPlayerPoint::changeDirection(int dir)
{
    pointData.direction = dir;
    if(pointData.direction<0)
        _cur=QPixmap::fromImage(currentImage.toImage().mirrored(true, false));
    else
        _cur=currentImage;
    update();
    arrayApply();
}

void ItemPlayerPoint::setPointData(PlayerPoint pnt, bool init)
{
    pointData = pnt;

    if(!init)
    {
        bool found=false;
        int q=0;
        for(q=0; q < scene->LvlData->players.size();q++)
        {
             if(scene->LvlData->players[q].id == pointData.id)
             {
                 found=true;
                 break;
             }
        }

        if(!found)
        {
            q = scene->LvlData->players.size();
            scene->LvlData->players.push_back(pointData);
        }
        else
            scene->LvlData->players[q]=pointData;
    }

    this->setPos(pointData.x, pointData.y);
    this->setZValue(scene->Z_Player);
    this->setData(ITEM_ARRAY_ID, QString::number(pointData.id));
    this->setFlag(QGraphicsItem::ItemIsSelectable, true);
    this->setFlag(QGraphicsItem::ItemIsMovable, true);

    switch(pointData.id)
    {
    case 1:
        currentImage = Themes::Image(Themes::player1); break;
    case 2:
        currentImage = Themes::Image(Themes::player2); break;
    default:
        currentImage = Themes::Image(Themes::player_point); break;
    }

    _offset_x = qRound(qreal(pnt.w-currentImage.width())/2.0);
    _offset_y = pnt.h-currentImage.height();

    this->setData(ITEM_WIDTH, (int)pnt.w);
    this->setData(ITEM_HEIGHT, (int)pnt.h);

    //Apply new image
    changeDirection(pointData.direction);
}



void ItemPlayerPoint::arrayApply()
{

    pointData.x = qRound(this->scenePos().x());
    pointData.y = qRound(this->scenePos().y());

    for(int i=0; i<scene->LvlData->players.size();i++)
    {
        if(pointData.id==scene->LvlData->players[i].id)
        {
            scene->LvlData->players[i] = pointData;
            break;
        }
    }

    //Update R-tree innex
    scene->unregisterElement(this);
    scene->registerElement(this);
}

void ItemPlayerPoint::removeFromArray()
{
    for(int i=0; i<scene->LvlData->players.size();i++)
    {
        if(pointData.id==scene->LvlData->players[i].id)
        {
            scene->LvlData->players.removeAt(i);
            break;
        }
    }
}

void ItemPlayerPoint::returnBack()
{
    this->setPos(pointData.x, pointData.y);
}

QPoint ItemPlayerPoint::gridOffset()
{
    return QPoint(0,0);
}

int ItemPlayerPoint::getGridSize()
{
    return 2;
}

QPoint ItemPlayerPoint::sourcePos()
{
    return QPoint(pointData.x, pointData.y);
}
