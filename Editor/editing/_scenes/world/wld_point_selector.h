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

#ifndef WLDPOINTSELECTOR_H
#define WLDPOINTSELECTOR_H

#include <QObject>
#include <common_features/simple_animator.h>

class WldScene;
class QGraphicsItem;
class WldPointSelector : public QObject
{
    Q_OBJECT
    //! Pointer to parent scene
    WldScene *      m_scene;
    //! Graphics item of point indicator
    QGraphicsItem * m_pointItem;
    //! Sprite of the point indicator
    QPixmap         m_pointImg;
public:
    explicit WldPointSelector(WldScene* scene, QObject* parent=nullptr);
    virtual ~WldPointSelector();

    //! Point indicator animation
    SimpleAnimator  m_animator;

    //! SELECTING Point on the map
    QPoint  m_pointCoord;
    bool    m_pointNotPlaced; //point is not selected (used because QPoint::isNull()
                               //will work in the x0-y0, but this point are usable)
                               //If value true, initial position will be 0x0, else already placed point

    void setPoint(QPoint p);   //Set Point item
    void unserPointSelector(); //remove point item from world map

signals:
    void pointSelected(QPoint point);

};

#endif // WLDPOINTSELECTOR_H
