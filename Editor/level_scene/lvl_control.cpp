/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include "lvlscene.h"
#include "../leveledit.h"

#include "item_block.h"
#include "item_bgo.h"


void LvlScene::keyReleaseEvent ( QKeyEvent * keyEvent )
{
    QList<QGraphicsItem*> selectedList = selectedItems();
    LevelData historyBuffer;
    bool deleted=false;

    QString objType;
    switch(keyEvent->key())
    {
    case (Qt::Key_Delete): //Delete action
        if(selectedList.isEmpty()) break;

        for (QList<QGraphicsItem*>::iterator it = selectedList.begin(); it != selectedList.end(); it++)
        {

                objType=(*it)->data(0).toString();
                //remove data from main array before deletion item from scene
                if( objType=="Block" )
                {
                    historyBuffer.blocks.push_back(((ItemBlock*)(*it))->blockData);
                    ((ItemBlock *)(*it))->removeFromArray();
                    removeItem((*it));
                    deleted=true;
                }
                else
                if( objType=="BGO" )
                {
                    historyBuffer.bgo.push_back(((ItemBGO*)(*it))->bgoData);
                    ((ItemBGO *)(*it))->removeFromArray();
                    removeItem((*it));
                    deleted=true;
                }
        }
        if(deleted) addRemoveHistory(historyBuffer);

        break;

    default:
        break;
    }
    QGraphicsScene::keyReleaseEvent(keyEvent);
}



void LvlScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
        //QList<QGraphicsItem*> selectedList = selectedItems();

        cursor->setPos(mouseEvent->scenePos());
        cursor->show();

        haveSelected=(!selectedItems().isEmpty());

        switch(EditingMode) // if Editing Mode = Esaising
        {
        case 1: //Eriser
            EraserEnabled = true;
            break;
        case 4: //Pasta
            PasteFromBuffer = true;
            break;
        default:
            break;
        }
        /* if (!selectedList.isEmpty())
        {

            for (QList<QGraphicsItem*>::iterator it = selectedList.begin(); it != selectedList.end(); it++)
            {
                // Z.push_back((*it)->zValue());
                // (*it)->setZValue(1000);
            }

        }*/
        QGraphicsScene::mousePressEvent(mouseEvent);
}


void LvlScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    cursor->setPos(mouseEvent->scenePos());
    QGraphicsItem * findItem;
    bool removeIt=true;

    haveSelected=(!selectedItems().isEmpty());
    if(haveSelected) IsMoved = true;

    if (EraserEnabled)
    {   // Remove All items, placed under Cursor
        findItem = itemCollidesCursor(cursor);
        if(findItem)
        {
            if((findItem->data(0).toString()=="Block")&&(lock_block))
                removeIt=false;
            else
            if((findItem->data(0).toString()=="BGO")&&(lock_bgo))
                removeIt=false;
            else
            if((findItem->data(0).toString()=="NPC")&&(lock_npc))
                removeIt=false;
            else
            if((findItem->data(0).toString()=="Water")&&(lock_water))
                removeIt=false;
            else
            if(((findItem->data(0).toString()=="Door_enter")||(findItem->data(0).toString()=="Door_exit"))&&
                    (lock_door))
                removeIt=false;

            if(removeIt)
            {
                LevelData removedItems;
                bool deleted=false;
                //remove data from main array before deletion item from scene
                if( findItem->data(0).toString()=="Block" )
                {
                    removedItems.blocks.push_back(((ItemBlock *)findItem)->blockData);
                    ((ItemBlock *)findItem)->removeFromArray();
                    deleted=true;
                }
                else
                if( findItem->data(0).toString()=="BGO" )
                {
                    removedItems.bgo.push_back(((ItemBGO *)findItem)->bgoData);
                    ((ItemBGO *)findItem)->removeFromArray();
                    deleted=true;
                }
                removeItem(findItem);
                if(deleted)addRemoveHistory(removedItems);
            }
        }
        QGraphicsScene::mouseMoveEvent(mouseEvent);
    } else
        QGraphicsScene::mouseMoveEvent(mouseEvent);
}



void LvlScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
    {
            int gridSize=32, offsetX=0, offsetY=0;//, gridX, gridY, i=0;
            QPoint sourcePos;

            cursor->hide();

            haveSelected = false;

            QString ObjType;
            int collisionPassed = false;

            //History
            LevelData historyBuffer; bool deleted=false;

            if(PasteFromBuffer)
            {
                paste( LvlBuffer, mouseEvent->scenePos().toPoint() );
                EditingMode = 0;
                PasteFromBuffer = false;
                IsMoved=true;
                wasPasted = true; //Set flag for reset pasta cursor to normal select
            }

            QList<QGraphicsItem*> selectedList = selectedItems();

            // check for grid snap
            if ((!selectedList.isEmpty())&&(IsMoved))
            {
                IsMoved = false;
                // correct selected items' coordinates
                for (QList<QGraphicsItem*>::iterator it = selectedList.begin(); it != selectedList.end(); it++)
                {
                    if(EraserEnabled)
                    {

                        //remove data from main array before deletion item from scene
                        if( (*it)->data(0).toString()=="Block" )
                        {
                            historyBuffer.blocks.push_back(((ItemBlock*)(*it))->blockData);
                            ((ItemBlock *)(*it))->removeFromArray();
                            deleted=true;
                        }
                        else
                        if( (*it)->data(0).toString()=="BGO" )
                        {
                            historyBuffer.bgo.push_back(((ItemBGO*)(*it))->bgoData);
                            ((ItemBGO *)(*it))->removeFromArray();
                            deleted=true;
                        }

                        removeItem((*it)); continue;
                    }

                    gridSize = 32;
                    offsetX = 0;
                    offsetY = 0;
                    ObjType = (*it)->data(0).toString();

                    //(*it)->setZValue(Z);
                    if( ObjType == "NPC")
                    {
                        gridSize = 1;
                    }
                    else
                    if( ObjType == "BGO")
                    {
                        gridSize = ((ItemBGO *)(*it))->gridSize;
                        offsetX = ((ItemBGO *)(*it))->gridOffsetX;
                        offsetY = ((ItemBGO *)(*it))->gridOffsetY;
                    }else
                    if( ObjType == "Water")
                    {
                        gridSize = 16;
                    }else
                    if( ObjType == "Door_enter")
                        gridSize = 16 ;
                    else
                    if( ObjType == "Door_exit")
                        gridSize = 16 ;
                    else
                    if( ObjType == "player1")
                    {
                        offsetY = 2;
                        gridSize = 2 ;
                    }
                    else
                    if( ObjType == "player2")
                    {
                        offsetY = 2;
                        gridSize = 2 ;
                    }

                    ////////////////////Apply to GRID/////////////////////////////////
                    (*it)->setPos( QPointF(
                                       applyGrid( (*it)->scenePos().toPoint(),
                                                      gridSize,
                                                      QPoint(offsetX, offsetY)
                                                  )
                                           )
                                  );
                    //////////////////////////////////////////////////////////////////
                }

                if((EraserEnabled)&&(deleted))
                {
                    addRemoveHistory(historyBuffer);
                }
                EraserEnabled = false;


                // Check collisions
                //Only if collision ckecking enabled
                if(!PasteFromBuffer)

                for (QList<QGraphicsItem*>::iterator it = selectedList.begin(); it != selectedList.end(); it++)
                {
                    ObjType = (*it)->data(0).toString();

                    WriteToLog(QtDebugMsg, QString(" >>Check collision with \"%1\"").arg(ObjType));

                    if( ObjType == "NPC")
                    {
                        foreach (LevelNPC findInArr, LvlData->npc)
                        {
                            if(findInArr.array_id==(unsigned)(*it)->data(2).toInt())
                            {
                                sourcePos = QPoint(findInArr.x, findInArr.y); break;
                            }
                        }
                    }
                    else
                    if( ObjType == "Block")
                    {
                        sourcePos = QPoint(  ((ItemBlock *)(*it))->blockData.x, ((ItemBlock *)(*it))->blockData.y);
                        //WriteToLog(QtDebugMsg, QString(" >>Check collision for Block"));
                    }
                    else
                    if( ObjType == "BGO")
                    {
                        sourcePos = QPoint(  ((ItemBGO *)(*it))->bgoData.x, ((ItemBGO *)(*it))->bgoData.y);
                        /*
                        foreach (LevelBGO findInArr, LvlData->bgo)
                        {
                            if(findInArr.array_id==(unsigned)(*it)->data(2).toInt())
                            {
                                sourcePos = QPoint(findInArr.x, findInArr.y); break;
                            }
                        }*/
                    }

                    //Check position
                    if( sourcePos == QPoint((long)((*it)->scenePos().x()), ((long)(*it)->scenePos().y())))
                    {
                        WriteToLog(QtDebugMsg, QString(" >>Collision skiped, posSource=posCurrent"));
                        continue;
                    }

                    if(opts.collisionsEnabled)
                    { //check Available to collisions checking
                        if( itemCollidesWith((*it)) )
                        {
                            collisionPassed = false;
                            (*it)->setPos(QPointF(sourcePos));
                            (*it)->setSelected(false);
                            /*
                            WriteToLog(QtDebugMsg, QString("Moved back %1 %2")
                                       .arg((long)(*it)->scenePos().x())
                                       .arg((long)(*it)->scenePos().y()) );*/
                        }
                        else
                        {
                            collisionPassed = true;
                        }
                    }

                    if((collisionPassed) || (!opts.collisionsEnabled))
                    {
                        if( ObjType == "Block")
                        {
                            //WriteToLog(QtDebugMsg, QString(" >>Collision passed"));
                            //Applay move into main array
                            ((ItemBlock *)(*it))->blockData.x = (long)(*it)->scenePos().x();
                            ((ItemBlock *)(*it))->blockData.y = (long)(*it)->scenePos().y();
                            ((ItemBlock *)(*it))->arrayApply();
                            LvlData->modified = true;
                        }
                        else
                        if( ObjType == "BGO")
                        {
                            //Applay move into main array
                            ((ItemBGO *)(*it))->bgoData.x = (long)(*it)->scenePos().x();
                            ((ItemBGO *)(*it))->bgoData.y = (long)(*it)->scenePos().y();
                            ((ItemBGO *)(*it))->arrayApply();
                            LvlData->modified = true;
                        }
                    }
                }


                QGraphicsScene::mouseReleaseEvent(mouseEvent);
                return;
            }
            EraserEnabled = false;
            QGraphicsScene::mouseReleaseEvent(mouseEvent);
    }
