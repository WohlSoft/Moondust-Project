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

#include <editing/edit_level/level_edit.h>

#include "wld_scene.h"
#include "items/item_tile.h"
#include "items/item_scene.h"
#include "items/item_path.h"
#include "items/item_level.h"
#include "items/item_music.h"


///
/// \brief cleanCollisionBuffer
/// Remove trash from collision buffer for crash protection
void WldScene::prepareCollisionBuffer()
{
    for(int i=0; i<collisionCheckBuffer.size(); i++ )
    {
        bool kick=false;
        if(collisionCheckBuffer[i]==NULL)
            kick=true;
        else
        if(collisionCheckBuffer[i]->data(ITEM_IS_ITEM).isNull())
            kick=true;
//        if(collisionCheckBuffer[i]->data(ITEM_TYPE).toString()=="YellowRectangle")
//            kick=true;
//        else
//        if(collisionCheckBuffer[i]->data(ITEM_TYPE).toString()=="Space")
//            kick=true;
//        else
//        if(collisionCheckBuffer[i]->data(ITEM_TYPE).toString()=="Square")
//            kick=true;
//        else
//        if(collisionCheckBuffer[i]->data(ITEM_TYPE).toString()=="Line")
//            kick=true;
//        else
//        if(collisionCheckBuffer[i]->data(ITEM_TYPE).toString()=="LineDrawer")
//            kick=true;
//        else
//        if(collisionCheckBuffer[i]->data(ITEM_TYPE).toString()=="WorldMapPoint")
//            kick=true;
//        else
//        if(collisionCheckBuffer[i]->data(ITEM_TYPE).toString()=="SectionBorder")
//            kick=true;
//        else
//        if(collisionCheckBuffer[i]->data(ITEM_TYPE).toString().startsWith("BackGround"))
//            kick=true;

        if(kick) {collisionCheckBuffer.removeAt(i); i--;}
    }
}


//Checking group collisions. Return true if was found even one passed collision in this group
bool WldScene::checkGroupCollisions(QList<QGraphicsItem *> *items)
{
    if(!items)
        return false;
    if(items->empty())
        return false;
    if(items->size()==1)
    {
        WriteToLog(QtDebugMsg, QString("Collision check: single item"));
        return (itemCollidesWith(items->first(), NULL)!=NULL);
    }

    //9 - width, 10 - height
    QRectF findZone = QRectF(items->first()->scenePos(),
                      QSizeF(items->first()->data(ITEM_WIDTH).toInt(),
                            items->first()->data(ITEM_HEIGHT).toInt()) );
    //get Zone
    foreach(QGraphicsItem * it, *items)
    {
        if(!it) continue;
        if(it->scenePos().x()-10 < findZone.left()) findZone.setLeft(it->scenePos().x());
        if(it->scenePos().y()-10 < findZone.top()) findZone.setTop(it->scenePos().y());

        if(it->scenePos().x()+it->data(ITEM_WIDTH).toInt() > findZone.right())
            findZone.setRight(it->scenePos().x()+it->data(ITEM_WIDTH).toInt());
        if(it->scenePos().y()+it->data(ITEM_HEIGHT).toInt() > findZone.bottom())
            findZone.setBottom(it->scenePos().y()+it->data(ITEM_HEIGHT).toInt());
    }

    findZone.setLeft(findZone.left()-10);
    findZone.setRight(findZone.right()+10);
    findZone.setTop(findZone.top()-10);
    findZone.setBottom(findZone.bottom()+10);

    QList<QGraphicsItem *> CheckZone;
    CheckZone = this->items( findZone, Qt::IntersectsItemBoundingRect);
    WriteToLog(QtDebugMsg, QString("Collision check: found items for check %1").arg(CheckZone.size()));
    WriteToLog(QtDebugMsg, QString("Collision rect: x%1 y%2 w%3 h%4").arg(findZone.x())
               .arg(findZone.y()).arg(findZone.width()).arg(findZone.height()));

    //Don't collide with items which in the group
    for(int i=0;i<CheckZone.size(); i++)
    {
        for(int j=0;j<(*items).size(); j++)
        {
            if(CheckZone[i] == (*items)[j])
            {
                CheckZone.removeOne(CheckZone[i]);
                i--; break;
            }
        }
    }

    foreach(QGraphicsItem * it, *items)
    {
        if(itemCollidesWith(it, &CheckZone)!=NULL)
            return true;
    }
    return false;

}

QGraphicsItem * WldScene::itemCollidesWith(QGraphicsItem * item, QList<QGraphicsItem *> *itemgrp)
{
    qlonglong leftA, leftB;
    qlonglong rightA, rightB;
    qlonglong topA, topB;
    qlonglong bottomA, bottomB;
    //qreal betweenZ;

    if(item==NULL) return NULL;

    if(item->data(0).toString()=="YellowRectangle")
        return NULL;

    QList<QGraphicsItem *> collisions;

    if(itemgrp && !itemgrp->isEmpty())
        collisions = *itemgrp;
    else
        collisions = this->items(
                QRectF(item->scenePos().x()-10, item->scenePos().y()-10,
                item->data(ITEM_WIDTH).toReal()+20, item->data(ITEM_HEIGHT).toReal()+20 ),
                Qt::IntersectsItemBoundingRect);

    //collidingItems(item, Qt::IntersectsItemBoundingRect);
    foreach (QGraphicsItem * it, collisions)
    {
            if(it == item)
                 continue;
            if(it == NULL)
                 continue;
            if(!it->isVisible()) continue;

            if(it->data(ITEM_TYPE).isNull())
                 continue;
            if(it->data(ITEM_IS_ITEM).isNull())
                continue;
//            if(it->data(ITEM_TYPE).toString()=="Space")
//                continue;
//            if(it->data(ITEM_TYPE).toString()=="Square")
//                continue;

            if(item->data(ITEM_TYPE).toString()!=it->data(0).toString()) continue;

            if(item->data(ITEM_TYPE).toString()=="SCENERY")
                if(item->data(ITEM_ID).toInt()!=it->data(ITEM_ID).toInt()) continue;

            leftA = item->scenePos().x();
            rightA = item->scenePos().x()+item->data(ITEM_WIDTH).toLongLong();
            topA = item->scenePos().y();
            bottomA = item->scenePos().y()+item->data(ITEM_HEIGHT).toLongLong();

            leftB = it->scenePos().x();
            rightB = it->scenePos().x()+it->data(ITEM_WIDTH).toLongLong();
            topB = it->scenePos().y();
            bottomB = it->scenePos().y()+it->data(ITEM_HEIGHT).toLongLong();

            if( bottomA <= topB )
            { continue; }
            if( topA >= bottomB )
            { continue; }
            if( rightA <= leftB )
            { continue; }
            if( leftA >= rightB )
            { continue; }

       return it; // Collision found!
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
                    (it->data(ITEM_TYPE).toString()=="TILE")||
                    (it->data(ITEM_TYPE).toString()=="SCENERY")||
                    (it->data(ITEM_TYPE).toString()=="PATH")||
                    (it->data(ITEM_TYPE).toString()=="LEVEL")||
                    (it->data(ITEM_TYPE).toString()=="MUSICBOX")
              )&&(it->isVisible() ) )
                return it;
    }
    return NULL;
}
