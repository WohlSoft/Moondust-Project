/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <common_features/main_window_ptr.h>
#include "item_point.h"

void ItemPoint::construct()
{
    this->setData(ITEM_WIDTH, QString::number(32));
    this->setData(ITEM_HEIGHT, QString::number(32));
    m_imageSize.setX(0);
    m_imageSize.setY(0);
    m_imageSize.setWidth(32);
    m_imageSize.setHeight(32);
}

ItemPoint::ItemPoint(QGraphicsItem *parent)
    : WldBaseItem(parent)
{
    construct();
}

ItemPoint::ItemPoint(WldScene *scenePoint, QGraphicsItem *parent)
    : WldBaseItem(scenePoint, parent)
{
    construct();
}

ItemPoint::~ItemPoint()
{}

QRectF ItemPoint::boundingRect() const
{
    return m_imageSize;
}

void ItemPoint::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if(m_scene==NULL)
    {
        painter->setPen(QPen(QBrush(Qt::yellow), 2, Qt::SolidLine));
        painter->setBrush(Qt::yellow);
        painter->setOpacity(0.5);
        painter->drawRect(1,1,m_imageSize.width()-2,m_imageSize.height()-2);
        painter->setOpacity(1);
        painter->setBrush(Qt::transparent);
        painter->drawRect(1,1,m_imageSize.width()-2,m_imageSize.height()-2);
    }
    else
    {
        QPixmap &image = m_scene->m_pointSelector.m_animator.wholeImage();
        painter->drawPixmap(m_imageSize, image, m_scene->m_pointSelector.m_animator.frameRectF());
    }

    if(this->isSelected())
    {
        painter->setPen(QPen(QBrush(Qt::black), 2, Qt::SolidLine));
        painter->drawRect(1,1,m_imageSize.width()-2,m_imageSize.height()-2);
        painter->setPen(QPen(QBrush(Qt::white), 2, Qt::DotLine));
        painter->drawRect(1,1,m_imageSize.width()-2,m_imageSize.height()-2);
    }
}

bool ItemPoint::itemTypeIsLocked()
{
    return true;
}

