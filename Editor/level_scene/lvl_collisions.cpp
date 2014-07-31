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

#include "lvlscene.h"
#include "../edit_level/level_edit.h"

#include "item_block.h"
#include "item_bgo.h"
#include "item_npc.h"




QGraphicsItem * LvlScene::itemCollidesWith(QGraphicsItem * item)
{
    qreal leftA, leftB;
    qreal rightA, rightB;
    qreal topA, topB;
    qreal bottomA, bottomB;
    //qreal betweenZ;

    //    QList<QGraphicsItem *> collisions = this->items(
    //                QRectF(item->scenePos().x()+1,item->scenePos().y()+1,
    //                item->data(9).toReal()-2, item->data(10).toReal()-2 ),
    //                Qt::IntersectsItemBoundingRect, Qt::AscendingOrder);

    QList<QGraphicsItem *> collisions = collidingItems(item, Qt::IntersectsItemBoundingRect);

    foreach (QGraphicsItem * it, collisions)
    {
            if (it == item)
                 continue;
            if(!it->isVisible())
                continue;
            if(item->data(0).toString()=="Water")
                return NULL;
            if(item->data(0).toString()=="Door_exit")
                return NULL;
            if(item->data(0).toString()=="Door_enter")
                return NULL;

            if(
               (it->data(0).toString()!="Block")&&
               (it->data(0).toString()!="BGO")&&
               (it->data(0).toString()!="NPC")
                    ) continue;

        leftA = item->scenePos().x();
        rightA = item->scenePos().x()+item->data(9).toReal();
        topA = item->scenePos().y();
        bottomA = item->scenePos().y()+item->data(10).toReal();

        leftB = it->scenePos().x();
        rightB = it->scenePos().x()+it->data(9).toReal();
        topB = it->scenePos().y();
        bottomB = it->scenePos().y()+it->data(10).toReal();

            #ifdef _DEBUG_
            if(it->data(0).toString()=="Block")
                WriteToLog(QtDebugMsg, QString(" >>Collision with block detected"));
            #endif

          if(item->data(0).toString()=="NPC")
          {
              if( item->data(8).toBool() ) // Disabled collisions with other NPCs
              {
                  if(item->data(1).toInt()!=it->data(1).toInt()) continue;
              }
              else
              {
                  if(
                          (
                           (it->data(0).toString()=="Block")
                           &&(!item->data(7).toBool()) //BlockCollision
                           )
                          ||
                          (
                           (it->data(0).toString()=="NPC")
                           &&(it->data(8).toBool()) //NpcCollision
                           )
                          ||
                          ((it->data(0).toString()!="NPC")&&(it->data(0).toString()!="Block"))
                     )
                            continue;
              }

          }
          else
          if(item->data(0).toString()=="Block")
          {
              if(
                      (
                       (it->data(0).toString()=="NPC")
                       &&(!it->data(7).toBool()) //BlockCollision
                       ) ||
                      ((it->data(0).toString()!="NPC")&&(it->data(0).toString()!="Block"))
                 )
                        continue;

          }
          else
                if(item->data(0).toString()!=it->data(0).toString()) continue;

          if(item->data(3).toString()=="sizable")
          {//sizable Block
              #ifdef _DEBUG_
              WriteToLog(QtDebugMsg, QString("sizable block") );
              #endif
              continue;
          }

          if(item->data(0).toString()=="BGO")
            if(item->data(1).toInt()!=it->data(1).toInt()) continue;

             if( bottomA < topB )
             { continue; }
             if( topA > bottomB )
             { continue; }
             if( rightA < leftB )
             { continue; }
             if( leftA > rightB )
             { continue; }

             if(it->data(3).toString()!="sizable")
                return it;
    }
    return NULL;
}

QGraphicsItem * LvlScene::itemCollidesCursor(QGraphicsItem * item)
{
    QList<QGraphicsItem *> collisions = collidingItems(item, Qt::IntersectsItemBoundingRect);
    foreach (QGraphicsItem * it, collisions) {
            if (it == item)
                 continue;
            if( (
                    (it->data(0).toString()=="Block")||
                    (it->data(0).toString()=="BGO")||
                    (it->data(0).toString()=="NPC")||
                    (it->data(0).toString()=="Door_exit")||
                    (it->data(0).toString()=="Door_enter")||
                    (it->data(0).toString()=="Water")||
                    (it->data(0).toString()=="player1")||
                    (it->data(0).toString()=="player2")
              )&&(it->isVisible() ) )
                return it;
    }
    return NULL;
}

/*
QGraphicsItem * LvlScene::itemCollidesMouse(QGraphicsItem * item)
{
    QList<QGraphicsItem *> collisions = collidingItems(item, Qt::ContainsItemShape);
    foreach (QGraphicsItem * it, collisions) {
            if (it == item)
                 continue;
            if(item->data(0).toString()=="Water")
                return NULL;
            if(item->data(0).toString()=="Door_exit")
                return NULL;
            if(item->data(0).toString()=="Door_enter")
                return NULL;

        if( item->data(0).toString() ==  it->data(0).toString() )
            return it;
    }
    return NULL;
}
*/
