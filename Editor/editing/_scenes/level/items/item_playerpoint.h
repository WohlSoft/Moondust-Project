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

#ifndef ITEM_PLAYERPOINT_H
#define ITEM_PLAYERPOINT_H

#include <QGraphicsPixmapItem>

#include <PGE_File_Formats/lvl_filedata.h>

#include "../lvl_scene.h"
#include "lvl_base_item.h"

class ItemPlayerPoint : public LvlBaseItem
{
public:
    explicit ItemPlayerPoint(QGraphicsItem *parent = 0);
    explicit ItemPlayerPoint(LvlScene *parentScene, QGraphicsItem *parent = 0);
    ~ItemPlayerPoint();
private:
    void construct();
public:

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);

    void changeDirection(int dir);

    void arrayApply();
    void removeFromArray();

    void returnBack();
    QPoint gridOffset();
    int getGridSize();
    QPoint sourcePos();

    void setPointData(PlayerPoint pnt, bool init=false);

    PlayerPoint pointData;

    void contextMenu(QGraphicsSceneMouseEvent *mouseEvent);

protected:
    int _offset_x;
    int _offset_y;
    QPixmap currentImage;
    QPixmap _cur;
};

#endif // ITEM_PLAYERPOINT_H
