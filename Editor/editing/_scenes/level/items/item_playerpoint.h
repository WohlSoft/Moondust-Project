/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <file_formats/lvl_filedata.h>

#include "../lvl_scene.h"
#include "lvl_base_item.h"

class ItemPlayerPoint : public QObject,
                        public QGraphicsPixmapItem,
                        public LvlBaseItem
{
    Q_OBJECT
public:
    explicit ItemPlayerPoint(QGraphicsItem *parent = 0);

    void changeDirection(int dir);

    void arrayApply();
    void removeFromArray();

    void returnBack();
    QPoint gridOffset();
    int getGridSize();
    QPoint sourcePos();

    void setScenePoint(LvlScene *theScene);
    void setPointData(PlayerPoint pnt, bool init=false);

    PlayerPoint pointData;

protected:
    bool mouseLeft;
    bool mouseMid;
    bool mouseRight;
    QPixmap currentImage;
    virtual void mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent );
    virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent * mouseEvent);

private:
    QMenu ItemMenu;
    LvlScene * scene;

};

#endif // ITEM_PLAYERPOINT_H
