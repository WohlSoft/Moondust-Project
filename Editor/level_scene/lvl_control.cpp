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
#include "../edit_level/leveledit.h"

#include "item_block.h"
#include "item_bgo.h"
#include "item_npc.h"

#include "../common_features/mainwinconnect.h"


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

                if(!(*it)->isVisible()) continue;  //Invisible items can't be deleted

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
                else
                if( objType=="NPC" )
                {
                    historyBuffer.npc.push_back(((ItemNPC*)(*it))->npcData);
                    ((ItemNPC *)(*it))->removeFromArray();
                    removeItem((*it));
                    deleted=true;
                }
        }
        if(deleted) addRemoveHistory(historyBuffer);

        break;
    case (Qt::Key_Escape):
        if(!IsMoved)
            this->clearSelection();
        if(pResizer!=NULL )
            MainWinConnect::pMainWin->on_cancelResize_clicked();
            //setSectionResizer(false, false);
        break;
    case (Qt::Key_Enter):
    case (Qt::Key_Return):

        if(pResizer!=NULL )
            MainWinConnect::pMainWin->on_applyResize_clicked();
            //setSectionResizer(false, true);
        break;

    default:
        break;
    }
    QGraphicsScene::keyReleaseEvent(keyEvent);
}



void LvlScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
        if(contextMenuOpened) return;

        cursor->setPos(mouseEvent->scenePos());
        cursor->show();

        if(DrawMode)
        {
            QGraphicsScene::mousePressEvent(mouseEvent);
            //Here must be started drawing of Rect ot placed first item
            return;
        }

        haveSelected=(!selectedItems().isEmpty());

        switch(EditingMode) // if Editing Mode = Esaising
        {
        case 1: //Eriser
            //EraserEnabled = true;
            break;
        case 4: //Pasta
            PasteFromBuffer = true;
            break;
        default:
            break;
        }

        if((disableMoveItems) && (mouseEvent->buttons() & Qt::LeftButton)
                && (Qt::ControlModifier != QApplication::keyboardModifiers()))
            { return; }

        QGraphicsScene::mousePressEvent(mouseEvent);

        QList<QGraphicsItem*> selectedList = selectedItems();
        if(EditingMode==1)
        if (!selectedList.isEmpty())
        {
            removeItemUnderCursor();
            EraserEnabled=true;
        }

        /* if (!selectedList.isEmpty())
        {

            for (QList<QGraphicsItem*>::iterator it = selectedList.begin(); it != selectedList.end(); it++)
            {
                // Z.push_back((*it)->zValue());
                // (*it)->setZValue(1000);
            }

        }*/
}


void LvlScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(contextMenuOpened) return;

    cursor->setPos(mouseEvent->scenePos());

    if(DrawMode)
    {
        this->clearSelection();
        QGraphicsScene::mouseMoveEvent(mouseEvent);
        //Here must be drawing functions
        return;
    }

    haveSelected=(!selectedItems().isEmpty());
    if(haveSelected)
    {
        if(!IsMoved)
        {
            /*
            for (QList<QGraphicsItem*>::iterator it = selectedItems().begin(); it != selectedItems().end(); it++)
            {
                Z.push_back((*it)->zValue());
                (*it)->setZValue(1000);
            }*/
            IsMoved = true;
        }
    }

    QGraphicsScene::mouseMoveEvent(mouseEvent);

    if (EraserEnabled)// Remove All items, placed under Cursor
        removeItemUnderCursor();
}

void LvlScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(contextMenuOpened)
    {
        contextMenuOpened = false; //bug protector
        QGraphicsScene::mouseReleaseEvent(mouseEvent);
        return;
    }

    if(DrawMode)
    {
        QGraphicsScene::mouseReleaseEvent(mouseEvent);

        //Here must be ended drawing of item
        return;
    }

            int gridSize=32, offsetX=0, offsetY=0;//, gridX, gridY, i=0;

            QPoint sourcePos;

            cursor->hide();

            haveSelected = false;

            QString ObjType;
            int collisionPassed = false;

            //History
            LevelData historyBuffer; bool deleted=false;
            LevelData historySourceBuffer;

            if(PasteFromBuffer)
            {
                paste( LvlBuffer, mouseEvent->scenePos().toPoint() );
                EditingMode = 0;
                PasteFromBuffer = false;
                IsMoved=false;

                MainWinConnect::pMainWin->activeLvlEditWin()->changeCursor(0);

                //activeLvlEditWin()->scene->wasPasted=false;
                //activeLvlEditWin()->scene->disableMoveItems=false;
                disableMoveItems=false;
                //wasPasted = true; //Set flag for reset pasta cursor to normal select
            }

            QList<QGraphicsItem*> selectedList = selectedItems();

            // check for grid snap
            if ((!selectedList.isEmpty())&&(IsMoved))
            {
                /*
                if(IsMoved)
                {
                    for (QList<QGraphicsItem*>::iterator it = selectedList.begin(); it != selectedList.end(); it++)
                    {
                        if(Z.size()>0)
                        {
                        (*it)->setZValue(Z[0]);
                        Z.pop_front();
                        }
                    }
                }*/

                // correct selected items' coordinates
                for (QList<QGraphicsItem*>::iterator it = selectedList.begin(); it != selectedList.end(); it++)
                {
                    if(EditingMode==1)
                    {

                        if(!(*it)->isVisible()) continue; //Invisible items can't be deleted

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
                        else
                        if( (*it)->data(0).toString()=="NPC" )
                        {
                            historyBuffer.npc.push_back(((ItemNPC*)(*it))->npcData);
                            ((ItemNPC *)(*it))->removeFromArray();
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
                        gridSize = ((ItemNPC *)(*it))->gridSize;
                        offsetX = ((ItemNPC *)(*it))->localProps.grid_offset_x;
                        offsetY = ((ItemNPC *)(*it))->localProps.grid_offset_y;
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

                if((EditingMode==1)&&(deleted))
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
                        sourcePos = QPoint(  ((ItemNPC *)(*it))->npcData.x, ((ItemNPC *)(*it))->npcData.y);
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
                    }

                    //Check position
                    if( sourcePos == QPoint((long)((*it)->scenePos().x()), ((long)(*it)->scenePos().y())))
                    {
                        IsMoved=false;
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
                            historySourceBuffer.blocks.push_back(((ItemBlock *)(*it))->blockData);
                            ((ItemBlock *)(*it))->blockData.x = (long)(*it)->scenePos().x();
                            ((ItemBlock *)(*it))->blockData.y = (long)(*it)->scenePos().y();
                            ((ItemBlock *)(*it))->arrayApply();
                            historyBuffer.blocks.push_back(((ItemBlock *)(*it))->blockData);
                            LvlData->modified = true;
                        }
                        else
                        if( ObjType == "BGO")
                        {
                            //Applay move into main array
                            historySourceBuffer.bgo.push_back(((ItemBGO *)(*it))->bgoData);
                            ((ItemBGO *)(*it))->bgoData.x = (long)(*it)->scenePos().x();
                            ((ItemBGO *)(*it))->bgoData.y = (long)(*it)->scenePos().y();
                            ((ItemBGO *)(*it))->arrayApply();
                            historyBuffer.bgo.push_back(((ItemBGO *)(*it))->bgoData);
                            LvlData->modified = true;
                        }
                        else
                        if( ObjType == "NPC")
                        {
                            //Applay move into main array
                            historySourceBuffer.npc.push_back(((ItemNPC *)(*it))->npcData);
                            ((ItemNPC *)(*it))->npcData.x = (long)(*it)->scenePos().x();
                            ((ItemNPC *)(*it))->npcData.y = (long)(*it)->scenePos().y();
                            ((ItemNPC *)(*it))->arrayApply();
                            historyBuffer.npc.push_back(((ItemNPC *)(*it))->npcData);
                            LvlData->modified = true;
                        }
                    }
                }

                if((EditingMode==0)&&(IsMoved)) addMoveHistory(historySourceBuffer, historyBuffer);

                IsMoved = false;

                QGraphicsScene::mouseReleaseEvent(mouseEvent);
                return;
           }
     EraserEnabled = false;
     QGraphicsScene::mouseReleaseEvent(mouseEvent);
}



void LvlScene::removeItemUnderCursor()
{
    if(contextMenuOpened) return;

    QGraphicsItem * findItem;
    bool removeIt=true;
    findItem = itemCollidesCursor(cursor);
    if(findItem)
    {
        if(findItem->data(0).toString()=="Block")
        {
            if((lock_block)|| (((ItemBlock *)findItem)->isLocked) )
            removeIt=false;
        }
        else
        if(findItem->data(0).toString()=="BGO")
        {
            if( (lock_bgo) || ((((ItemBGO *)findItem)->isLocked)) )
            removeIt=false;
        }
        else
        if(findItem->data(0).toString()=="NPC")
        {
            if( (lock_npc) || ((((ItemNPC *)findItem)->isLocked)) )
            removeIt=false;
        }
        else
        if((findItem->data(0).toString()=="Water")&&(lock_water))
            removeIt=false;
        else
        if(((findItem->data(0).toString()=="Door_enter")||(findItem->data(0).toString()=="Door_exit"))&&
                (lock_door))
            removeIt=false;

        if(!findItem->isVisible()) //Invisible items can't be deleted
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
            else
            if( findItem->data(0).toString()=="BGO" )
            {
                removedItems.npc.push_back(((ItemNPC *)findItem)->npcData);
                ((ItemNPC *)findItem)->removeFromArray();
                deleted=true;
            }
            removeItem(findItem);
            if(deleted)addRemoveHistory(removedItems);
        }
    }
}


void LvlScene::setSectionResizer(bool enabled, bool accept)
{
    if((enabled)&&(pResizer==NULL))
    {
        int x = LvlData->sections[LvlData->CurSection].size_left;
        int y = LvlData->sections[LvlData->CurSection].size_top;
        int w = LvlData->sections[LvlData->CurSection].size_right;
        int h = LvlData->sections[LvlData->CurSection].size_bottom;

        pResizer = new ItemResizer( QSize((long)fabs(x-w), (long)fabs(y-h)), Qt::green, 32 );
        this->addItem(pResizer);
        pResizer->setPos(x, y);
        pResizer->type=0;
        pResizer->_minSize = QSizeF(800,600);
        this->setFocus(Qt::ActiveWindowFocusReason);
        DrawMode=true;
        MainWinConnect::pMainWin->activeLvlEditWin()->changeCursor(5);
    }
    else
    {
        if(pResizer!=NULL)
        {
            if(accept)
            {
                WriteToLog(QtDebugMsg, QString("SECTION RESIZE -> to %1 x %2").arg(pResizer->_width).arg(pResizer->_height));
                long l = pResizer->pos().x();
                long t = pResizer->pos().y();
                long r = l+pResizer->_width;
                long b = t+pResizer->_height;
                LvlData->sections[LvlData->CurSection].size_left = l;
                LvlData->sections[LvlData->CurSection].size_right = r;
                LvlData->sections[LvlData->CurSection].size_top = t;
                LvlData->sections[LvlData->CurSection].size_bottom = b;

                ChangeSectionBG(LvlData->sections[LvlData->CurSection].background);
                drawSpace();
            }
            delete pResizer;
            pResizer = NULL;
            MainWinConnect::pMainWin->on_actionSelect_triggered();
            //resetResizingSection=true;
        }
        DrawMode=false;
    }
}
