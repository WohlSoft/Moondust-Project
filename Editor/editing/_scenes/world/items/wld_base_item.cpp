/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2023 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "wld_base_item.h"
#include "../wld_scene.h"

void WldBaseItem::construct()
{
    m_gridSize = 32;
    m_gridOffsetX = 0;
    m_gridOffsetY = 0;

    m_locked = false;

    m_imgOffsetX = 0;
    m_imgOffsetY = 0;
    m_animatorID = -1;
    m_imageSize = QRectF(0, 0, 10, 10);

    m_mouseLeftPressed = false;
    m_mouseMidPressed = false;
    m_mouseRightPressed = false;

    setData(ITEM_IS_ITEM, 1);
}

WldBaseItem::WldBaseItem(QGraphicsItem *parent) :
    QGraphicsItem(parent), m_scene(nullptr)
{
    construct();
}

WldBaseItem::WldBaseItem(WldScene *parentScene, QGraphicsItem *parent) :
    QGraphicsItem(parent), m_scene(parentScene)
{
    construct();
}

void WldBaseItem::setScenePoint(WldScene *theScene)
{
    m_scene = theScene;
}

WldBaseItem::~WldBaseItem()
{}

void WldBaseItem::arrayApply()
{}

void WldBaseItem::returnBack()
{}

void WldBaseItem::removeFromArray()
{}

void WldBaseItem::setLayer(QString layer)
{
    Q_UNUSED(layer)
}

int WldBaseItem::getGridSize()
{
    return m_gridSize;
}

void WldBaseItem::setGridSize(int grid)
{
    m_gridSize = abs(grid);
}

QPoint WldBaseItem::gridOffset()
{
    return QPoint(m_gridOffsetX, m_gridOffsetY);
}

QPoint WldBaseItem::sourcePos()
{
    return QPoint(0, 0);
}

bool WldBaseItem::isLocked()
{
    return m_locked;
}

void WldBaseItem::setLocked(bool lock)
{
    this->setOpacity(lock ? GlobalSettings::LvlItemDefaults.LockedItemOpacity : 1);
    this->setFlag(QGraphicsItem::ItemIsSelectable, !lock);
    this->setFlag(QGraphicsItem::ItemIsMovable, !lock);
    m_locked = lock;
}

void WldBaseItem::contextMenu(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent)
}

bool WldBaseItem::itemTypeIsLocked()
{
    return false;
}

void WldBaseItem::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if((this->flags()&QGraphicsItem::ItemIsSelectable) == 0)
    {
        QGraphicsItem::mousePressEvent(mouseEvent);
        return;
    }

    if(m_scene->m_busyMode)
    {
        unsetCursor();
        ungrabMouse();
        this->setSelected(false);
        return;
    }

    //Discard multi-mouse keys
    if((m_mouseLeftPressed) || (m_mouseMidPressed) || (m_mouseRightPressed))
    {
        mouseEvent->accept();
        return;
    }

    if(mouseEvent->buttons() & Qt::LeftButton)
        m_mouseLeftPressed = true;
    if(mouseEvent->buttons() & Qt::MiddleButton)
        m_mouseMidPressed = true;
    if(mouseEvent->buttons() & Qt::RightButton)
        m_mouseRightPressed = true;

    QGraphicsItem::mousePressEvent(mouseEvent);
}

void WldBaseItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    int multimouse = 0;
    bool callContext = false;
    if(((m_mouseMidPressed) || (m_mouseRightPressed)) && (m_mouseLeftPressed ^ (mouseEvent->buttons() & Qt::LeftButton)))
        multimouse++;
    if((((m_mouseLeftPressed) || (m_mouseRightPressed))) && (m_mouseMidPressed ^ (mouseEvent->buttons() & Qt::MiddleButton)))
        multimouse++;
    if((((m_mouseLeftPressed) || (m_mouseMidPressed))) && (m_mouseRightPressed ^ (mouseEvent->buttons() & Qt::RightButton)))
        multimouse++;
    if(multimouse > 0)
    {
        mouseEvent->accept();
        return;
    }

    if(mouseEvent->button() == Qt::LeftButton)
        m_mouseLeftPressed = false;

    if(mouseEvent->button() == Qt::MiddleButton)
        m_mouseMidPressed = false;

    if(mouseEvent->button() == Qt::RightButton)
    {
        callContext = true;
        m_mouseRightPressed = false;
    }

    QGraphicsItem::mouseReleaseEvent(mouseEvent);

    /////////////////////////CONTEXT MENU:///////////////////////////////
    if((callContext) && (!m_scene->m_contextMenuIsOpened))
    {
        if((!itemTypeIsLocked()) && (!m_scene->m_busyMode) && (!m_locked))
            contextMenu(mouseEvent);
    }
}
