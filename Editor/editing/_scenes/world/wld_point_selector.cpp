/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "wld_point_selector.h"
#include "items/item_point.h"

WldPointSelector::WldPointSelector(WldScene *scene, QObject *parent) :
    QObject(parent),
    m_scene(scene),
    m_pointItem(nullptr),
    m_pointImg(QPixmap(":/images/set_point.png")),
    m_pointCoord(0, 0),
    m_pointNotPlaced(true)
{
    m_animator.setSettings(m_pointImg, true, 31, 16);
    m_scene->m_animationTimer.registerAnimation(&m_animator);
}

WldPointSelector::~WldPointSelector()
{
    unserPointSelector();
}


void WldPointSelector::setPoint(QPoint p)
{
    m_pointCoord = p;
    m_pointNotPlaced = false;

    if(!m_pointItem)
    {
        ItemPoint *itemPtr = new ItemPoint(m_scene, nullptr);
        m_pointItem = itemPtr;
        itemPtr->setGridSize(m_scene->m_configs->defaultGrid.general);
        m_scene->addItem(itemPtr);
        itemPtr->setData(ITEM_TYPE, "POINT");
        itemPtr->setZValue(6000);
        //A small trick to avoid non-null pointer if object destroyed
        itemPtr->connect(itemPtr,
                         &ItemPoint::destroyed,
                         [this]()->void{ m_pointItem = nullptr; });
    }
    m_pointItem->setPos(QPointF(p));
}

void WldPointSelector::unserPointSelector()
{
    if(m_pointItem)
        delete m_pointItem;
    m_pointItem = nullptr;
}
