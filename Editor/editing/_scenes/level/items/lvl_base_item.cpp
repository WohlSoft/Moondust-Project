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

#include "lvl_base_item.h"
#include "../lvl_scene.h"

LvlBaseItem::LvlBaseItem(QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    m_scene = nullptr;
    construct();
}

LvlBaseItem::LvlBaseItem(LvlScene *parentScene, QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    m_scene = parentScene;
    construct();
}

LvlBaseItem::~LvlBaseItem()
{}

void LvlBaseItem::construct()
{
    m_gridSize = 32;
    m_gridOffsetX = 0;
    m_gridOffsetY = 0;
    m_locked = false;

    m_animatorID = -1;
    m_imageSize = QRectF(0, 0, 10, 10);

    m_mouseLeft = false;
    m_mouseMid = false;
    m_mouseRight = false;

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
    Q_UNUSED(layer)
}

int LvlBaseItem::getGridSize()
{
    return m_gridSize;
}

QPoint LvlBaseItem::gridOffset()
{
    return QPoint(m_gridOffsetX, m_gridOffsetY);
}

QPoint LvlBaseItem::sourcePos()
{
    return QPoint(0, 0);
}

void LvlBaseItem::contextMenu(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent)
}

QPainterPath LvlBaseItem::shape() const
{
    QPainterPath path;
    path.addRect(QRectF(0.0, 0.0,
                        this->data(ITEM_WIDTH).toReal(),
                        this->data(ITEM_HEIGHT).toReal()));
    return path;
}

void LvlBaseItem::setLocked(bool lock)
{
    this->setOpacity(lock ? GlobalSettings::LvlItemDefaults.LockedItemOpacity : 1);
    this->setFlag(QGraphicsItem::ItemIsSelectable, !lock);
    this->setFlag(QGraphicsItem::ItemIsMovable, !lock);
    m_locked = lock;
}

bool LvlBaseItem::itemTypeIsLocked()
{
    return false;
}

void LvlBaseItem::setMetaSignsVisibility(bool)
{}

void LvlBaseItem::setScenePoint(LvlScene *theScene)
{
    m_scene = theScene;
}

void LvlBaseItem::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
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
        mouseEvent->accept();
        return;
    }
    //Discard multi-mouse keys
    if((m_mouseLeft) || (m_mouseMid) || (m_mouseRight))
    {
        mouseEvent->accept();
        return;
    }

    if(mouseEvent->buttons() & Qt::LeftButton)
        m_mouseLeft = true;
    if(mouseEvent->buttons() & Qt::MiddleButton)
        m_mouseMid = true;
    if(mouseEvent->buttons() & Qt::RightButton)
        m_mouseRight = true;

    QGraphicsItem::mousePressEvent(mouseEvent);
}

void LvlBaseItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    int multimouse = 0;
    bool callContext = false;

    if(((m_mouseMid) || (m_mouseRight)) && (m_mouseLeft ^ (mouseEvent->buttons() & Qt::LeftButton)))
        multimouse++;
    if((((m_mouseLeft) || (m_mouseRight))) && (m_mouseMid ^ (mouseEvent->buttons() & Qt::MiddleButton)))
        multimouse++;
    if((((m_mouseLeft) || (m_mouseMid))) && (m_mouseRight ^ (mouseEvent->buttons() & Qt::RightButton)))
        multimouse++;
    if(multimouse > 0)
    {
        mouseEvent->accept();
        return;
    }

    if(mouseEvent->button() == Qt::LeftButton)
        m_mouseLeft = false;

    if(mouseEvent->button() == Qt::MiddleButton)
        m_mouseMid = false;

    if(mouseEvent->button() == Qt::RightButton)
    {
        callContext = true;
        m_mouseRight = false;
    }

    QGraphicsItem::mouseReleaseEvent(mouseEvent);

    /////////////////////////CONTEXT MENU:///////////////////////////////
    if((callContext) && (!m_scene->m_contextMenuIsOpened))
    {
        if((!itemTypeIsLocked()) && (!m_scene->m_busyMode) && (!m_locked))
            contextMenu(mouseEvent);
    }
}
