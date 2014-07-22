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

#include "wld_scene.h"
#include "../edit_level/level_edit.h"

#include "item_tile.h"
#include "item_scene.h"
#include "item_path.h"
#include "item_level.h"
#include "item_music.h"


QGraphicsItem * WldScene::itemCollidesWith(QGraphicsItem * item)
{
    qlonglong leftA, leftB;
    qlonglong rightA, rightB;
    qlonglong topA, topB;
    qlonglong bottomA, bottomB;
    //qreal betweenZ;

    QList<QGraphicsItem *> collisions = collidingItems(item, Qt::IntersectsItemBoundingRect);
    foreach (QGraphicsItem * it, collisions)
    {
            if (it == item)
                 continue;
            if(!it->isVisible()) continue;

            if(item->data(0).toString()!=it->data(0).toString()) continue;

            if(item->data(0).toString()=="SCENERY")
                if(item->data(1).toInt()!=it->data(1).toInt()) continue;

            leftA = item->scenePos().x();
            rightA = item->scenePos().x()+item->data(9).toLongLong();
            topA = item->scenePos().y();
            bottomA = item->scenePos().y()+item->data(10).toLongLong();

            leftB = it->scenePos().x();
            rightB = it->scenePos().x()+it->data(9).toLongLong();
            topB = it->scenePos().y();
            bottomB = it->scenePos().y()+it->data(10).toLongLong();

            if( bottomA <= topB )
            { continue; }
            if( topA >= bottomB )
            { continue; }
            if( rightA <= leftB )
            { continue; }
            if( leftA >= rightB )
            { continue; }
            return it;
    }
    return NULL;
}

QGraphicsItem * WldScene::itemCollidesCursor(QGraphicsItem * item)
{
    QList<QGraphicsItem *> collisions = collidingItems(item, Qt::IntersectsItemBoundingRect);
    foreach (QGraphicsItem * it, collisions) {
            if (it == item)
                 continue;
            if( (
                    (it->data(0).toString()=="TILE")||
                    (it->data(0).toString()=="SCENERY")||
                    (it->data(0).toString()=="PATH")||
                    (it->data(0).toString()=="LEVEL")||
                    (it->data(0).toString()=="MUSICBOX")
              )&&(it->isVisible() ) )
                return it;
    }
    return NULL;
}
