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

#include <editing/edit_level/level_edit.h>

#include "lvl_scene.h"
#include "items/item_block.h"
#include "items/item_bgo.h"
#include "items/item_npc.h"
#include "items/item_water.h"
#include "items/item_door.h"

#include "../../../common_features/logger.h"

#include "../../../common_features/timecounter.h"

///
/// \brief cleanCollisionBuffer
/// Remove trash from collision buffer for crash protection
void LvlScene::prepareCollisionBuffer()
{
    for(int i=0; i<collisionCheckBuffer.size(); i++ )
    {
        bool kick=false;
        if(collisionCheckBuffer[i]==NULL)
            kick=true;
        else
        if(collisionCheckBuffer[i]->data(0).toString()=="YellowRectangle")
            kick=true;
        else
        if(collisionCheckBuffer[i]->data(0).toString()=="Space")
            kick=true;
        else
        if(collisionCheckBuffer[i]->data(0).toString()=="Square")
            kick=true;
        else
        if(collisionCheckBuffer[i]->data(0).toString()=="Line")//LineDrawer
            kick=true;
        else
        if(collisionCheckBuffer[i]->data(0).toString()=="LineDrawer")
            kick=true;
        else
        if(collisionCheckBuffer[i]->data(0).toString()=="SectionBorder")
            kick=true;
        else
        if(collisionCheckBuffer[i]->data(0).toString()=="playerPoint")
            kick=true;
        else
        if(collisionCheckBuffer[i]->data(0).toString().startsWith("BackGround"))
            kick=true;

        if(kick) {collisionCheckBuffer.removeAt(i); i--;}
    }
}

//Checking group collisions. Return true if was found even one passed collision in this group
bool LvlScene::checkGroupCollisions(QList<QGraphicsItem *> *items)
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
                      QSizeF(items->first()->data(9).toInt(),
                            items->first()->data(10).toInt()) );
    //get Zone
    foreach(QGraphicsItem * it, *items)
    {
        if(!it) continue;
        if(it->scenePos().x()-10 < findZone.left()) findZone.setLeft(it->scenePos().x());
        if(it->scenePos().y()-10 < findZone.top()) findZone.setTop(it->scenePos().y());

        if(it->scenePos().x()+it->data(9).toInt() > findZone.right())
            findZone.setRight(it->scenePos().x()+it->data(9).toInt());
        if(it->scenePos().y()+it->data(10).toInt() > findZone.bottom())
            findZone.setBottom(it->scenePos().y()+it->data(10).toInt());
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
                i--;
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

QGraphicsItem * LvlScene::itemCollidesWith(QGraphicsItem * item, QList<QGraphicsItem *> *itemgrp)
{
    qreal leftA, leftB;
    qreal rightA, rightB;
    qreal topA, topB;
    qreal bottomA, bottomB;
    //qreal betweenZ;

    //xxx=!xxx;

    if(item==NULL)
        return NULL;

    if(item->data(ITEM_TYPE).toString()=="YellowRectangle")
        return NULL;
    if(item->data(ITEM_TYPE).toString()=="Water")
        return NULL;
    if(item->data(ITEM_TYPE).toString()=="Door_exit")
        return NULL;
    if(item->data(ITEM_TYPE).toString()=="Door_enter")
        return NULL;

    QList<QGraphicsItem *> collisions;

    //TimeCounter t;
    //t.start();
    //if(xxx)
    // ~15 ms on big maps

    if(itemgrp && !itemgrp->isEmpty())
        collisions = *itemgrp;
    else
        collisions = this->items(
                QRectF(item->scenePos().x()-10, item->scenePos().y()-10,
                item->data(ITEM_WIDTH).toReal()+20, item->data(ITEM_HEIGHT).toReal()+20 ),
                Qt::IntersectsItemBoundingRect);


    //else

        // ~32 ms on big maps
        //collisions = item->collidingItems(Qt::IntersectsItemBoundingRect);

    //WriteToLog(QtDebugMsg, QString("Collision %1 in %2").arg(xxx).arg(t.current()));
    //t.stop();


    foreach (QGraphicsItem * it, collisions)
    {

            if(it == item)
                continue;
            if(it==NULL)
                 continue;
            if(!it->isVisible())
                continue;
            if(it->data(ITEM_TYPE).isNull())
                 continue;
            if(it->data(ITEM_TYPE).toString()=="YellowRectangle")
                continue;
            if(it->data(ITEM_TYPE).toString()=="Space")
                continue;
            if(it->data(ITEM_TYPE).toString()=="Square")
                continue;
            if(it->data(ITEM_TYPE).toString()=="SectionBorder")
                continue;
            if(it->data(ITEM_TYPE).toString()=="PlayerPoint")
                continue;
            if(it->data(ITEM_TYPE).toString().startsWith("BackGround"))
                continue;

            if(
               (it->data(ITEM_TYPE).toString()!="Block")&&
               (it->data(ITEM_TYPE).toString()!="BGO")&&
               (it->data(ITEM_TYPE).toString()!="NPC")
                    ) continue;

          if(item->data(ITEM_TYPE).toString()=="NPC")
          {
              if( item->data(ITEM_NPC_NO_NPC_COLLISION).toBool() ) // Disabled collisions with other NPCs
              {
                  if(item->data(ITEM_ID).toInt()!=it->data(ITEM_ID).toInt()) continue;
              }
              else
              {
                  if(
                          (
                           (it->data(ITEM_TYPE).toString()=="Block")
                           &&(!item->data(ITEM_NPC_BLOCK_COLLISION).toBool()) //BlockCollision
                           )
                          ||
                          (
                           (it->data(ITEM_TYPE).toString()=="NPC")
                           &&(it->data(ITEM_NPC_NO_NPC_COLLISION).toBool()) //NpcCollision
                           )
                          ||
                          ((it->data(ITEM_TYPE).toString()!="NPC")
                           &&(it->data(ITEM_TYPE).toString()!="Block"))
                     )
                            continue;
              }

          }
          else
          if(item->data(0).toString()=="Block")
          {
              if(
                      (
                       (it->data(ITEM_TYPE).toString()=="NPC")
                       &&(!it->data(ITEM_NPC_BLOCK_COLLISION).toBool()) //BlockCollision
                       ) ||
                      ((it->data(ITEM_TYPE).toString()!="NPC")
                       &&(it->data(ITEM_TYPE).toString()!="Block"))
                 )
                        continue;

          }
          else
                if(item->data(ITEM_TYPE).toString()!=it->data(ITEM_TYPE).toString()) continue;

          if(item->data(ITEM_BLOCK_IS_SIZABLE).toString()=="sizable")
          {   // Don't collide with sizable block
              #ifdef _DEBUG_
              WriteToLog(QtDebugMsg, QString("sizable block") );
              #endif
              continue;
          }

          if(it->data(ITEM_BLOCK_IS_SIZABLE).toString()=="sizable") continue; // Don't collide with sizable block

          if(item->data(ITEM_TYPE).toString()=="BGO")
            if(item->data(ITEM_ID).toInt()!=it->data(ITEM_ID).toInt()) continue;

          leftA = item->scenePos().x();
          rightA = item->scenePos().x()+item->data(ITEM_WIDTH).toReal();
          topA = item->scenePos().y();
          bottomA = item->scenePos().y()+item->data(ITEM_HEIGHT).toReal();

          leftB = it->scenePos().x();
          rightB = it->scenePos().x()+it->data(ITEM_WIDTH).toReal();
          topB = it->scenePos().y();
          bottomB = it->scenePos().y()+it->data(ITEM_HEIGHT).toReal();

          #ifdef _DEBUG_
          WriteToLog(QtDebugMsg, QString("Collision check -> Item1 L%1 | T%2 | R%3 |  B%4")
                     .arg(leftA).arg(topA).arg(rightA).arg(bottomA));
          WriteToLog(QtDebugMsg, QString("Collision check -> Item2 R%3 | B%4 | L%1 |  T%2")
                     .arg(leftB).arg(topB).arg(rightB).arg(bottomB));

          WriteToLog(QtDebugMsg, QString("Collision check -> Item1 W%1, H%2")
                     .arg(item->data(ITEM_WIDTH).toReal()).arg(item->data(ITEM_HEIGHT).toReal()));
          WriteToLog(QtDebugMsg, QString("Collision check -> Item2 W%1, H%2")
                     .arg(it->data(ITEM_WIDTH).toReal()).arg(it->data(ITEM_HEIGHT).toReal()));

          WriteToLog(QtDebugMsg, QString("Collision check -> B%1 <= T%2 -> %3")
                     .arg(bottomA).arg(topB).arg(bottomA <= topB));
          WriteToLog(QtDebugMsg, QString("Collision check -> T%1 >= B%2 -> %3")
                     .arg(topA).arg(bottomB).arg(topA >= bottomB));
          WriteToLog(QtDebugMsg, QString("Collision check -> R%1 <= L%2 -> %3")
                     .arg(rightA).arg(leftB).arg(rightA <= leftB));
          WriteToLog(QtDebugMsg, QString("Collision check -> L%1 >= R%2 -> %3")
                     .arg(leftA).arg(rightB).arg(leftA >= rightB));

          //Collision check -> Item1 L-199776 | T-200288 | R-199744 |  B-200256
          //Collision check -> Item2 R-199744 | B-200288 | L-199776 |  T-200320
          #endif

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

QGraphicsItem * LvlScene::itemCollidesCursor(QGraphicsItem * item)
{
    QList<QGraphicsItem *> collisions = collidingItems(item, Qt::IntersectsItemBoundingRect);
    foreach (QGraphicsItem * it, collisions) {
            if (it == item)
                 continue;

            //skip invisible items
            if(!it->isVisible()) continue;

            //skip locked items
            if((it->data(0).toString()=="Block"))
            {
                if((lock_block)|| dynamic_cast<ItemBlock*>(it)->isLocked) continue;
            }
            else
            if((it->data(0).toString()=="BGO"))
            {
                if((lock_bgo)|| dynamic_cast<ItemBGO*>(it)->isLocked) continue;
            }
            else
            if((it->data(0).toString()=="NPC"))
            {
                if((lock_npc)|| dynamic_cast<ItemNPC*>(it)->isLocked) continue;
            }
            else
            if((it->data(0).toString()=="Water"))
            {
                if((lock_water)|| dynamic_cast<ItemWater*>(it)->isLocked) continue;
            }
            else
            if((it->data(0).toString()=="Door_enter")||(it->data(0).toString()=="Door_exit"))
            {
                if((lock_door)|| dynamic_cast<ItemDoor*>(it)->isLocked) continue;
            }

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
