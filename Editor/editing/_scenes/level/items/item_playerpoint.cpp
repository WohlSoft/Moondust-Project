/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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
    m_scene->addItem(this);
}

void ItemPlayerPoint::construct()
{
    m_gridSize = 4;
    m_offset_x = 0;
    m_offset_y = 0;
    setData(ITEM_TYPE, "playerPoint");
}

QRectF ItemPlayerPoint::boundingRect() const
{
    return QRectF(m_offset_x, m_offset_y, m_currentImage.width(), m_currentImage.height());
}

void ItemPlayerPoint::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->drawPixmap(m_offset_x, m_offset_y, m_cur.width(), m_cur.height(), m_cur);
    if(this->isSelected())
    {
        painter->setPen(QPen(QBrush(Qt::black), 2, Qt::SolidLine));
        painter->drawRect(m_offset_x+1, m_offset_y+1,m_cur.width()-2,m_cur.height()-2);
        painter->setPen(QPen(QBrush(Qt::white), 2, Qt::DotLine));
        painter->drawRect(m_offset_x+1, m_offset_y+1,m_cur.width()-2,m_cur.height()-2);
    }
}

ItemPlayerPoint::~ItemPlayerPoint()
{
    m_scene->unregisterElement(this);
}

void ItemPlayerPoint::contextMenu(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!m_scene->m_busyMode)
    {
        m_scene->m_contextMenuIsOpened = true; //bug protector
    }

    //Remove selection from non-block items
    if(!this->isSelected())
    {
        m_scene->clearSelection();
        this->setSelected(true);
    }

    this->setSelected(true);
    QMenu ItemMenu;

    bool isLvlx = !m_scene->m_data->meta.smbx64strict;

    QMenu * chDir =     ItemMenu.addMenu(
                        tr("Set %1").arg(tr("Direction")) );
        chDir->setEnabled(isLvlx);

    QAction *setLeft =  chDir->addAction( tr("Left"));
        setLeft->setCheckable(true);
        setLeft->setChecked(m_data.direction==-1);

    QAction *setRight = chDir->addAction( tr("Right") );
        setRight->setCheckable(true);
        setRight->setChecked(m_data.direction==1);

                        ItemMenu.addSeparator();

    QAction * remove =  ItemMenu.addAction(tr("Remove"));

    /*****************Waiting for answer************************/
    QAction *selected = ItemMenu.exec(mouseEvent->screenPos());
    /***********************************************************/

    if(!selected)
        return;


    if(selected==remove)
    {
        m_scene->removeSelectedLvlItems();
    }
    else
    if(selected==setLeft)
    {
        //LevelData selData;
        foreach(QGraphicsItem * SelItem, m_scene->selectedItems() )
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
        foreach(QGraphicsItem * SelItem, m_scene->selectedItems() )
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
    m_data.direction = dir;
    if(m_data.direction<0)
        m_cur=QPixmap::fromImage(m_currentImage.toImage().mirrored(true, false));
    else
        m_cur=m_currentImage;
    update();
    arrayApply();
}

void ItemPlayerPoint::setPointData(PlayerPoint pnt, bool init)
{
    m_data = pnt;

    if(!init)
    {
        bool found=false;
        int q=0;
        for(q=0; q < m_scene->m_data->players.size();q++)
        {
             if(m_scene->m_data->players[q].id == m_data.id)
             {
                 found=true;
                 break;
             }
        }

        if(!found)
        {
            q = m_scene->m_data->players.size();
            m_scene->m_data->players.push_back(m_data);
        }
        else
            m_scene->m_data->players[q]=m_data;
    }

    if(((int)m_data.w>=(int)m_gridSize))
        m_gridOffsetX = -1 * qRound( qreal((int)m_data.w % m_gridSize)/2 );
    else
        m_gridOffsetX = qRound( qreal( m_gridSize - (int)m_data.w )/2 ) + (m_gridSize/2);
    m_gridOffsetY = -m_data.h % m_gridSize;

    this->setPos(m_data.x, m_data.y);
    this->setZValue(m_scene->Z_Player);
    this->setData(ITEM_ARRAY_ID, QString::number(m_data.id));
    this->setFlag(QGraphicsItem::ItemIsSelectable, true);
    this->setFlag(QGraphicsItem::ItemIsMovable, true);

    switch(m_data.id)
    {
    case 1:
        m_currentImage = Themes::Image(Themes::player1); break;
    case 2:
        m_currentImage = Themes::Image(Themes::player2); break;
    default:
        m_currentImage = Themes::Image(Themes::player_point); break;
    }

    m_offset_x = qRound(qreal(pnt.w-m_currentImage.width())/2.0);
    m_offset_y = pnt.h-m_currentImage.height();

    this->setData(ITEM_WIDTH, (int)pnt.w);
    this->setData(ITEM_HEIGHT, (int)pnt.h);

    //Apply new image
    changeDirection(m_data.direction);
}



void ItemPlayerPoint::arrayApply()
{

    m_data.x = qRound(this->scenePos().x());
    m_data.y = qRound(this->scenePos().y());

    for(int i=0; i<m_scene->m_data->players.size();i++)
    {
        if(m_data.id==m_scene->m_data->players[i].id)
        {
            m_scene->m_data->players[i] = m_data;
            break;
        }
    }

    //Update R-tree innex
    m_scene->unregisterElement(this);
    m_scene->registerElement(this);
}

void ItemPlayerPoint::removeFromArray()
{
    for(int i=0; i<m_scene->m_data->players.size();i++)
    {
        if(m_data.id==m_scene->m_data->players[i].id)
        {
            m_scene->m_data->players.removeAt(i);
            break;
        }
    }
}

void ItemPlayerPoint::returnBack()
{
    this->setPos(m_data.x, m_data.y);
}

QPoint ItemPlayerPoint::sourcePos()
{
    return QPoint(m_data.x, m_data.y);
}
