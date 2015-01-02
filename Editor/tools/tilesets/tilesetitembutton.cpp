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

#include <common_features/items.h>
#include <common_features/graphics_funcs.h>

#include "tilesetitembutton.h"

TilesetItemButton::TilesetItemButton(dataconfigs *conf, QGraphicsScene *scene, QWidget *parent) :
    QFrame(parent)
{
    scn = scene;
    m_id = 0;
    m_itemType = ItemTypes::LVL_Block;
    m_config = conf;
    setFrameStyle(QFrame::Panel | QFrame::Raised);
    setLineWidth(2);
}

dataconfigs *TilesetItemButton::config() const
{
    return m_config;
}

void TilesetItemButton::setConfig(dataconfigs *config)
{
    m_config = config;
}

void TilesetItemButton::applyItem(const int &i, const int &id, const int &width, const int &height)
{
    int wid = (width == -1 ? contentsRect().width() : width);
    int hei = (height == -1 ? contentsRect().height() : height);
    QPixmap p = GraphicsHelps::squareImage(Items::getItemGFX(i,id, false, NULL, scn), QSize(wid,hei));//.scaled(wid,hei,Qt::KeepAspectRatio);
    if(p.isNull()){
        m_drawItem = QPixmap(wid,hei);
        return;
    }
    m_drawItem = p;
    m_id = (unsigned int)id;
    m_itemType = static_cast<ItemTypes::itemTypes>(i);
}

void TilesetItemButton::applySize(const int &width, const int &height)
{
    setMinimumSize(width+lineWidth()*2,height+lineWidth()*2);
    setMaximumSize(width+lineWidth()*2,height+lineWidth()*2);
}

void TilesetItemButton::paintEvent(QPaintEvent *ev)
{
    QPainter painter;
    painter.begin(this);
    painter.fillRect(contentsRect(), Qt::darkGray);

    if(!m_drawItem.isNull())
        painter.drawPixmap(contentsRect(),m_drawItem,m_drawItem.rect());

    painter.end();

    QFrame::paintEvent(ev);
}

void TilesetItemButton::mousePressEvent(QMouseEvent *)
{
    if(isItemSet())
        emit clicked(static_cast<int>(m_itemType), (unsigned long)m_id);
    setFrameStyle(QFrame::Panel | QFrame::Sunken);
}

void TilesetItemButton::mouseReleaseEvent(QMouseEvent *)
{
    setFrameStyle(QFrame::Panel | QFrame::Raised);
}
unsigned int TilesetItemButton::id() const
{
    return m_id;
}

bool TilesetItemButton::isItemSet()
{
    return !m_drawItem.isNull() && m_id!=0;
}

ItemTypes::itemTypes TilesetItemButton::itemType() const
{
    return m_itemType;
}


