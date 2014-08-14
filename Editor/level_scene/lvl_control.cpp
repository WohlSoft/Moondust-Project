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
#include <QtMath>

#include "item_block.h"
#include "item_bgo.h"
#include "item_npc.h"
#include "item_water.h"
#include "item_door.h"

#include "../common_features/mainwinconnect.h"
#include "../common_features/grid.h"
#include "lvl_item_placing.h"

#include "../file_formats/file_formats.h"

#include "../common_features/item_rectangles.h"

QPoint sourcePos=QPoint(0,0);
int gridSize=0, offsetX=0, offsetY=0;//, gridX, gridY, i=0;

namespace lvl_control
{
    bool mouseLeft=false; //Left mouse key is pressed
    bool mouseMid=false;  //Middle mouse key is pressed
    bool mouseRight=false;//Right mouse key is pressed

    bool mouseMoved=false; //Mouse was moved with right mouseKey

    static QPointF drawStartPos = QPoint(0,0); // Stored start point of mouse movement with pressed key

    //the last Array ID's, which used before hold mouse key
    static qlonglong last_block_arrayID=0;
    static qlonglong last_bgo_arrayID=0;
    static qlonglong last_npc_arrayID=0;
}

// //////////////////////////////////////////////EVENTS START/////////////////////////////////////////////////
void LvlScene::keyReleaseEvent ( QKeyEvent * keyEvent )
{
    using namespace lvl_control;
    switch(keyEvent->key())
    {
    case (Qt::Key_Delete): //Delete action
        removeSelectedLvlItems();
        break;
    case (Qt::Key_Escape):
        if(!mouseMoved)
            this->clearSelection();

        resetResizers();

        if(EditingMode == MODE_PlacingNew || EditingMode == MODE_DrawSquare || EditingMode == MODE_Line){
            item_rectangles::clearArray();
            MainWinConnect::pMainWin->on_actionSelect_triggered();
            return;
        }
            //setSectionResizer(false, false);
        break;
    case (Qt::Key_Enter):
    case (Qt::Key_Return):

        applyResizers();
            //setSectionResizer(false, true);
        break;

    default:
        break;
    }
    QGraphicsScene::keyReleaseEvent(keyEvent);
}


// /////////////////////////////Selection was changes////////////////////////////////
void LvlScene::selectionChanged()
{
    if(this->selectedItems().isEmpty())
    {
        MainWinConnect::pMainWin->LvlItemProps(-1, FileFormats::dummyLvlBlock(), FileFormats::dummyLvlBgo(), FileFormats::dummyLvlNpc());
    }

    #ifdef _DEBUG_
        WriteToLog(QtDebugMsg, "Selection Changed!");
    #endif
}

void LvlScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    using namespace lvl_control;

    #ifdef _DEBUG_
    WriteToLog(QtDebugMsg, QString("Mouse pressed -> [%1, %2] contextMenuOpened=%3, DrawMode=%4").arg(mouseEvent->scenePos().x()).arg(mouseEvent->scenePos().y())
               .arg(contextMenuOpened).arg(DrawMode));
    #endif

//if(contextMenuOpened) return;
    contextMenuOpened=false;
    IsMoved = false;

    //Discard multi mouse key
    int mSum = (int)( mouseEvent->buttons() );
    if( mSum > 4 || mSum == 3 )
    {
        mouseEvent->accept();
        WriteToLog(QtDebugMsg, QString("[MousePress] MultiMouse detected [%2] [edit mode: %1]").arg(EditingMode).arg(QString::number(mSum, 2)));
        return;
    }

    mouseMoved=false;

    if( mouseEvent->buttons() & Qt::LeftButton )
    {
        mouseLeft=true;
        WriteToLog(QtDebugMsg, QString("Left mouse button pressed [edit mode: %1]").arg(EditingMode));
    }
    else
        mouseLeft=false;

    if( mouseEvent->buttons() & Qt::MiddleButton )
    {
        mouseMid=true;
        WriteToLog(QtDebugMsg, QString("Middle mouse button pressed [edit mode: %1]").arg(EditingMode));
    } else mouseMid=false;

    if( mouseEvent->buttons() & Qt::RightButton )
    {
        mouseRight=true;
        WriteToLog(QtDebugMsg, QString("Right mouse button pressed [edit mode: %1]").arg(EditingMode));
    } else mouseRight=false;

    WriteToLog(QtDebugMsg, QString("Current editing mode %1").arg(EditingMode));

    switch(EditingMode)
    {
        case MODE_PlacingNew:
        {
            if( mouseEvent->buttons() & Qt::RightButton )
            {
                item_rectangles::clearArray();
                MainWinConnect::pMainWin->on_actionSelect_triggered();
                return;
            }

            last_block_arrayID=LvlData->blocks_array_id;
            last_bgo_arrayID=LvlData->bgo_array_id;
            last_npc_arrayID=LvlData->npc_array_id;

            if(cursor){
                cursor->setPos( QPointF(applyGrid( mouseEvent->scenePos().toPoint()-
                                                   QPoint(LvlPlacingItems::c_offset_x,
                                                          LvlPlacingItems::c_offset_y),
                                                   LvlPlacingItems::gridSz,
                                                   LvlPlacingItems::gridOffset)));
            }
            placeItemUnderCursor();
            QGraphicsScene::mousePressEvent(mouseEvent);
            return;
            break;
        }
        case MODE_DrawSquare:
        {
            if( mouseEvent->buttons() & Qt::RightButton )
            {
                item_rectangles::clearArray();
                MainWinConnect::pMainWin->on_actionSelect_triggered();
                return;
            }

            last_block_arrayID=LvlData->blocks_array_id;
            last_bgo_arrayID=LvlData->bgo_array_id;
            last_npc_arrayID=LvlData->npc_array_id;

            WriteToLog(QtDebugMsg, QString("Square mode %1").arg(EditingMode));
            if(cursor){
                drawStartPos = QPointF(applyGrid( mouseEvent->scenePos().toPoint(),
                                                  LvlPlacingItems::gridSz,
                                                  LvlPlacingItems::gridOffset));
                cursor->setPos( drawStartPos );
                cursor->setVisible(true);

                QPoint hw = applyGrid( mouseEvent->scenePos().toPoint(),
                                       LvlPlacingItems::gridSz,
                                       LvlPlacingItems::gridOffset);

                QSize hs = QSize( (long)fabs(drawStartPos.x() - hw.x()),  (long)fabs( drawStartPos.y() - hw.y() ) );
                ((QGraphicsRectItem *)cursor)->setRect(0,0, hs.width(), hs.height());
            }

            QGraphicsScene::mousePressEvent(mouseEvent);
            return;
            break;
        }
        case MODE_Line:
        {
            if( mouseEvent->buttons() & Qt::RightButton )
            {
                item_rectangles::clearArray();
                MainWinConnect::pMainWin->on_actionSelect_triggered();
                return;
            }

            last_block_arrayID=LvlData->blocks_array_id;
            last_bgo_arrayID=LvlData->bgo_array_id;
            last_npc_arrayID=LvlData->npc_array_id;

            WriteToLog(QtDebugMsg, QString("Line mode %1").arg(EditingMode));

            if(cursor){
                drawStartPos = QPointF(applyGrid( mouseEvent->scenePos().toPoint()-
                                                  QPoint(LvlPlacingItems::c_offset_x,
                                                         LvlPlacingItems::c_offset_y),
                                                  LvlPlacingItems::gridSz,
                                                  LvlPlacingItems::gridOffset));
                //cursor->setPos( drawStartPos );
                cursor->setVisible(true);

                QPoint hw = applyGrid( mouseEvent->scenePos().toPoint()-
                                       QPoint(LvlPlacingItems::c_offset_x,
                                              LvlPlacingItems::c_offset_y),
                                       LvlPlacingItems::gridSz,
                                       LvlPlacingItems::gridOffset);
                ((QGraphicsLineItem *)cursor)->setLine(drawStartPos.x(), drawStartPos.y(), hw.x(), hw.y());
            }

            QGraphicsScene::mousePressEvent(mouseEvent);
            return;
            break;

        }
        case MODE_Resizing:
        {
            QGraphicsScene::mousePressEvent(mouseEvent);
            return;
        }
        case MODE_Erasing:
        {
            if( mouseEvent->buttons() & Qt::RightButton )
            {
                MainWinConnect::pMainWin->on_actionSelect_triggered();
                return;
            }

            if(cursor){
               cursor->show();
               cursor->setPos(mouseEvent->scenePos());
            }

            QGraphicsScene::mousePressEvent(mouseEvent);

            QList<QGraphicsItem*> selectedList = selectedItems();
            if (!selectedList.isEmpty())
            {
                removeItemUnderCursor();
                EraserEnabled=true;
            }
            break;
        }
        case MODE_PasteFromClip: //Pasta
        {
            if( mouseEvent->buttons() & Qt::RightButton )
            {
                MainWinConnect::pMainWin->on_actionSelect_triggered();
                return;
            }
            PasteFromBuffer = true;
            break;
        }
        default:
        {
            if((disableMoveItems) && (mouseEvent->buttons() & Qt::LeftButton)
                && (Qt::ControlModifier != QApplication::keyboardModifiers()))
            { return; }

            QGraphicsScene::mousePressEvent(mouseEvent);

            break;
        }
    }
    haveSelected=(!selectedItems().isEmpty());

}

void LvlScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    using namespace lvl_control;

    //WriteToLog(QtDebugMsg, QString("Mouse moved -> [%1, %2]").arg(mouseEvent->scenePos().x()).arg(mouseEvent->scenePos().y()));
    //if(contextMenuOpened) return;
    contextMenuOpened=false;
    IsMoved = true;

    switch(EditingMode)
    {
    case MODE_PlacingNew:
        {
            this->clearSelection();
            if(cursor)
            {
                       cursor->setPos( QPointF(applyGrid( QPointF(mouseEvent->scenePos()-
                                                           QPointF(LvlPlacingItems::c_offset_x,
                                                                  LvlPlacingItems::c_offset_y)).toPoint(),
                                                         LvlPlacingItems::gridSz,
                                                         LvlPlacingItems::gridOffset)));
                       cursor->show();
            }
            if( mouseEvent->buttons() & Qt::LeftButton ) placeItemUnderCursor();
            QGraphicsScene::mouseMoveEvent(mouseEvent);
            break;
        }
    case MODE_DrawSquare:
        {
            if(cursor)
            {
                if(cursor->isVisible())
                {
                QPoint hw = applyGrid( mouseEvent->scenePos().toPoint(),
                                       LvlPlacingItems::gridSz,
                                       LvlPlacingItems::gridOffset);

                QSize hs = QSize( (long)fabs(drawStartPos.x() - hw.x()),  (long)fabs( drawStartPos.y() - hw.y() ) );


                ((QGraphicsRectItem *)cursor)->setRect(0,0, hs.width(), hs.height());
                ((QGraphicsRectItem *)cursor)->setPos(
                            ((hw.x() < drawStartPos.x() )? hw.x() : drawStartPos.x()),
                            ((hw.y() < drawStartPos.y() )? hw.y() : drawStartPos.y())
                            );

                if(((placingItem==PLC_Block)&&(!LvlPlacingItems::sizableBlock))||(placingItem==PLC_BGO))
                {
                item_rectangles::drawMatrix(this, QRect (((QGraphicsRectItem *)cursor)->x(),
                                                        ((QGraphicsRectItem *)cursor)->y(),
                                                        ((QGraphicsRectItem *)cursor)->rect().width(),
                                                        ((QGraphicsRectItem *)cursor)->rect().height()),
                                            QSize(LvlPlacingItems::itemW, LvlPlacingItems::itemH)
                                            );
                }

                }
            }
            break;
        }
    case MODE_Line:
        {
            if(cursor)
            {
                if(cursor->isVisible())
                {
                    QPoint hs = applyGrid( mouseEvent->scenePos().toPoint()-
                                           QPoint(LvlPlacingItems::c_offset_x,
                                                  LvlPlacingItems::c_offset_y),
                                           LvlPlacingItems::gridSz,
                                           LvlPlacingItems::gridOffset);

                    QLineF s = item_rectangles::snapLine(QLineF(drawStartPos.x(),drawStartPos.y(), (qreal)hs.x(), (qreal)hs.y()),
                                                         QSizeF((qreal)LvlPlacingItems::itemW, (qreal)LvlPlacingItems::itemH) );

                    QPoint hw = applyGrid( s.p2().toPoint(),
                                        LvlPlacingItems::gridSz,
                                        LvlPlacingItems::gridOffset);

                    s.setP2(QPointF((qreal)hw.x(),(qreal)hw.y()));

                    ((QGraphicsLineItem *)cursor)->setLine(s);

                    item_rectangles::drawLine(this, s,
                           QSize(LvlPlacingItems::itemW, LvlPlacingItems::itemH)
                                                );

                }
            }
            break;
        }
    case MODE_Resizing:
        {
            this->clearSelection();
            QGraphicsScene::mouseMoveEvent(mouseEvent);
            return;
        break;
        }
    case MODE_Erasing:
        {
            if(cursor) cursor->setPos(mouseEvent->scenePos());
            if (EraserEnabled)// Remove All items, placed under Cursor
                removeItemUnderCursor();
            break;
        }
    default:
        if(!( mouseEvent->buttons() & Qt::LeftButton )) return;
        if(cursor) cursor->setPos(mouseEvent->scenePos());
        break;
    }

    haveSelected=(!selectedItems().isEmpty());
    if(haveSelected)
    {
        if(!( mouseEvent->buttons() & Qt::LeftButton )) return;
        if(!mouseMoved)
        {
            mouseMoved=true;
        }
    }

    QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void LvlScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    using namespace lvl_control;

    int multimouse=0;
    if( mouseLeft || (mouseLeft^(mouseEvent->buttons() & Qt::LeftButton)) )
        multimouse++;
    if( mouseMid || (mouseMid^(mouseEvent->buttons() & Qt::MiddleButton)) )
        multimouse++;
    if( mouseRight || (mouseRight^(mouseEvent->buttons() & Qt::RightButton)) )
        multimouse++;

    bool isLeftMouse=false;

    if( mouseLeft^(mouseEvent->buttons() & Qt::LeftButton) )
    {
        mouseLeft=false;
        isLeftMouse=true;
        WriteToLog(QtDebugMsg, QString("Left mouse button released [edit mode: %1]").arg(EditingMode));
    }
    if( mouseMid^(mouseEvent->buttons() & Qt::MiddleButton) )
    {
        mouseMid=false;
        WriteToLog(QtDebugMsg, QString("Middle mouse button released [edit mode: %1]").arg(EditingMode));
    }
    if( mouseRight^(mouseEvent->buttons() & Qt::RightButton) )
    {
        mouseRight=false;
        WriteToLog(QtDebugMsg, QString("Right mouse button released [edit mode: %1]").arg(EditingMode));
    }

    if(multimouse>1)
    {
        WriteToLog(QtDebugMsg, QString("Multiple mouse keys detected %1").arg(multimouse) );
        mouseEvent->accept(); return;
    }

    contextMenuOpened=false;
    if(!isLeftMouse)
    {
        QGraphicsScene::mouseReleaseEvent(mouseEvent);
        return;
    }

    switch(EditingMode)
    {
    case MODE_DrawSquare:
        {

        if(cursor)
        {

            // /////////// Don't draw with zero width or height //////////////
            if( (((QGraphicsRectItem *)cursor)->rect().width()==0) ||
              (((QGraphicsRectItem *)cursor)->rect().height()==0))
            {
                cursor->hide();
                break;
            }
            // ///////////////////////////////////////////////////////////////

            switch(placingItem)
            {
            case PLC_Water:
                {
                    LvlPlacingItems::waterSet.quicksand = (LvlPlacingItems::waterType==1);

                    LvlPlacingItems::waterSet.x = cursor->scenePos().x();
                    LvlPlacingItems::waterSet.y = cursor->scenePos().y();
                    LvlPlacingItems::waterSet.w = ((QGraphicsRectItem *)cursor)->rect().width();
                    LvlPlacingItems::waterSet.h = ((QGraphicsRectItem *)cursor)->rect().height();
                    //here define placing water item.
                    LvlData->water_array_id++;

                    LvlPlacingItems::waterSet.array_id = LvlData->water_array_id;
                    LvlData->water.push_back(LvlPlacingItems::waterSet);

                    placeWater(LvlPlacingItems::waterSet, true);
                    LevelData plWater;
                    plWater.water.push_back(LvlPlacingItems::waterSet);
                    addPlaceHistory(plWater);
                    break;
                }
            case PLC_Block:
                {
                    //LvlPlacingItems::waterSet.quicksand = (LvlPlacingItems::waterType==1);
                    if(LvlPlacingItems::sizableBlock)
                    {
                        LvlPlacingItems::blockSet.x = cursor->scenePos().x();
                        LvlPlacingItems::blockSet.y = cursor->scenePos().y();
                        LvlPlacingItems::blockSet.w = ((QGraphicsRectItem *)cursor)->rect().width();
                        LvlPlacingItems::blockSet.h = ((QGraphicsRectItem *)cursor)->rect().height();
                        //here define placing water item.
                        LvlData->blocks_array_id++;

                        LvlPlacingItems::blockSet.array_id = LvlData->blocks_array_id;
                        LvlData->blocks.push_back(LvlPlacingItems::blockSet);

                        placeBlock(LvlPlacingItems::blockSet, true);
                        LevelData plSzBlock;
                        plSzBlock.blocks.push_back(LvlPlacingItems::blockSet);
                        addPlaceHistory(plSzBlock);
                        break;
                    }
                    else
                    {
                        placeItemsByRectArray();
                        break;
                    }
                }
            case PLC_BGO:
                {
                 placeItemsByRectArray();
                 break;
                }
            }
            LvlData->modified = true;

            cursor->hide();
        }
        break;
        }
    case MODE_Line:
        {

        if(cursor)
        {
            WriteToLog(QtDebugMsg, "Line tool -> Placing items");
            placeItemsByRectArray();

            LvlData->modified = true;
            cursor->hide();
        }
        break;
        }
    case MODE_PlacingNew:
    {
        if(placingItem == PLC_Door)
        {
            MainWinConnect::pMainWin->on_actionSelect_triggered();
            QGraphicsScene::mouseReleaseEvent(mouseEvent);
            return;
        }else{
            if(!overwritedItems.blocks.isEmpty()||
                !overwritedItems.bgo.isEmpty()||
                !overwritedItems.npc.isEmpty() )
            {
                addOverwriteHistory(overwritedItems, placingItems);
                overwritedItems.blocks.clear();
                overwritedItems.bgo.clear();
                overwritedItems.npc.clear();
                placingItems.blocks.clear();
                placingItems.bgo.clear();
                placingItems.npc.clear();
            }
            else
            if(!placingItems.blocks.isEmpty()||
                    !placingItems.bgo.isEmpty()||
                    !placingItems.npc.isEmpty())
            {
                addPlaceHistory(placingItems);
                placingItems.blocks.clear();
                placingItems.bgo.clear();
                placingItems.npc.clear();
            }
        }
        break;
    }
    case MODE_Erasing:
    {
        if(!overwritedItems.blocks.isEmpty()||
            !overwritedItems.bgo.isEmpty()||
            !overwritedItems.npc.isEmpty() )
        {
            addRemoveHistory(overwritedItems);
            overwritedItems.blocks.clear();
            overwritedItems.bgo.clear();
            overwritedItems.npc.clear();
        }
        break;
    }
    default:
        break;
    }


    if(DrawMode)
    {
        QGraphicsScene::mouseReleaseEvent(mouseEvent);
        return;
    }

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
                PasteFromBuffer = false;
                IsMoved=false;
                MainWinConnect::pMainWin->on_actionSelect_triggered();
            }


            QList<QGraphicsItem*> deleteList;
            deleteList.clear();
            QList<QGraphicsItem*> selectedList = selectedItems();

            // check for grid snap
            if ((!selectedList.isEmpty())&&(mouseMoved))
            {

                if(EditingMode==MODE_Erasing)
                {
                    removeLvlItems(selectedList);
                }
                else
                // correct selected items' coordinates
                for (QList<QGraphicsItem*>::iterator it = selectedList.begin(); it != selectedList.end(); it++)
                { ////////////////////////FIRST FETCH///////////////////////

                    /////////////////////////GET DATA///////////////
                    setItemSourceData((*it), (*it)->data(0).toString()); //Set Grid Size/Offset, sourcePosition
                    /////////////////////////GET DATA/////////////////////

                    //Check position
                    if( (sourcePos == QPoint((long)((*it)->scenePos().x()), ((long)(*it)->scenePos().y()))))
                    {
                        ///SKIP NON-MOVED ITEMS
                        mouseMoved=false;
                        #ifdef _DEBUG_
                        WriteToLog(QtDebugMsg, QString(" >>Collision skiped, posSource=posCurrent"));
                        #endif
                        continue;
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

                } ////////////////////////FIRST FETCH///////////////////////

                selectedList = selectedItems();

                if((EditingMode==MODE_Erasing)&&(deleted))
                {
                    addRemoveHistory(historyBuffer);
                }
                EraserEnabled = false;

                // Check collisions
                //Only if collision ckecking enabled
                if(!PasteFromBuffer)
                for (QList<QGraphicsItem*>::iterator it = selectedList.begin(); it != selectedList.end(); it++)
                { ////////////////////////SECOND FETCH///////////////////////
                    ObjType = (*it)->data(0).toString();

                    #ifdef _DEBUG_
                    WriteToLog(QtDebugMsg, QString(" >>Check collision with \"%1\"").arg(ObjType));
                    #endif

                    setItemSourceData((*it), ObjType); //Set Grid Size/Offset, sourcePosition

                    //Check position
                    if( (sourcePos == QPoint((long)((*it)->scenePos().x()), ((long)(*it)->scenePos().y()))))
                    {
                        ///SKIP NON-MOVED ITEMS
                        mouseMoved=false;
                        #ifdef _DEBUG_
                        WriteToLog(QtDebugMsg, QString(" >>Collision skiped, posSource=posCurrent"));
                        #endif
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
                        else
                        if( ObjType == "Water")
                        {
                            //Applay move into main array
                            historySourceBuffer.water.push_back(((ItemWater *)(*it))->waterData);
                            ((ItemWater *)(*it))->waterData.x = (long)(*it)->scenePos().x();
                            ((ItemWater *)(*it))->waterData.y = (long)(*it)->scenePos().y();
                            ((ItemWater *)(*it))->arrayApply();
                            historyBuffer.water.push_back(((ItemWater *)(*it))->waterData);
                            LvlData->modified = true;
                        }
                        else
                        if( ObjType == "Door_enter")
                        {
                            //Applay move into main array
                            //historySourceBuffer.water.push_back(((ItemWater *)(*it))->waterData);
                            LevelDoors oldDoorData = ((ItemDoor *)(*it))->doorData;
                            oldDoorData.isSetIn = true;
                            oldDoorData.isSetOut = false;
                            historySourceBuffer.doors.push_back(oldDoorData);
                            ((ItemDoor *)(*it))->doorData.ix = (long)(*it)->scenePos().x();
                            ((ItemDoor *)(*it))->doorData.iy = (long)(*it)->scenePos().y();
                            if((((ItemDoor *)(*it))->doorData.lvl_i)||((ItemDoor *)(*it))->doorData.lvl_o)
                            {
                                ((ItemDoor *)(*it))->doorData.ox = (long)(*it)->scenePos().x();
                                ((ItemDoor *)(*it))->doorData.oy = (long)(*it)->scenePos().y();
                            }

                            ((ItemDoor *)(*it))->arrayApply();
                            LevelDoors newDoorData = ((ItemDoor *)(*it))->doorData;
                            newDoorData.isSetIn = true;
                            newDoorData.isSetOut = false;
                            historyBuffer.doors.push_back(newDoorData);
                            //historyBuffer.water.push_back(((ItemWater *)(*it))->waterData);
                            LvlData->modified = true;
                        }
                        else
                        if( ObjType == "Door_exit")
                        {
                            //Applay move into main array
                            //historySourceBuffer.water.push_back(((ItemWater *)(*it))->waterData);
                            LevelDoors oldDoorData = ((ItemDoor *)(*it))->doorData;
                            oldDoorData.isSetIn = false;
                            oldDoorData.isSetOut = true;
                            historySourceBuffer.doors.push_back(oldDoorData);
                            ((ItemDoor *)(*it))->doorData.ox = (long)(*it)->scenePos().x();
                            ((ItemDoor *)(*it))->doorData.oy = (long)(*it)->scenePos().y();
                            if((((ItemDoor *)(*it))->doorData.lvl_i)||((ItemDoor *)(*it))->doorData.lvl_o)
                            {
                                ((ItemDoor *)(*it))->doorData.ix = (long)(*it)->scenePos().x();
                                ((ItemDoor *)(*it))->doorData.iy = (long)(*it)->scenePos().y();
                            }
                            ((ItemDoor *)(*it))->arrayApply();
                            LevelDoors newDoorData = ((ItemDoor *)(*it))->doorData;
                            newDoorData.isSetIn = false;
                            newDoorData.isSetOut = true;
                            historyBuffer.doors.push_back(newDoorData);
                            //historyBuffer.water.push_back(((ItemWater *)(*it))->waterData);
                            LvlData->modified = true;
                        }
                        else
                        if(( ObjType == "player1" ) || ( ObjType == "player2" ))
                        {
                            int plrId=0;
                            if( ObjType == "player1" )
                                plrId=1;
                            if( ObjType == "player2" )
                                plrId=2;

                            for(int g=0; g<LvlData->players.size(); g++)
                            {
                             if(LvlData->players[g].id == (unsigned int)plrId)
                             {
                                 historySourceBuffer.players.push_back(LvlData->players[g]);
                                 LvlData->players[g].x = (long)(*it)->scenePos().x();
                                 LvlData->players[g].y = (long)(*it)->scenePos().y();
                                 historyBuffer.players.push_back(LvlData->players[g]);
                                 break;
                             }
                            }
                        }
                    }
                }////////////////////////SECOND FETCH///////////////////////

                if((EditingMode==MODE_Selecting)&&(mouseMoved)) addMoveHistory(historySourceBuffer, historyBuffer);

                mouseMoved = false;

                QGraphicsScene::mouseReleaseEvent(mouseEvent);
                return;
           }
     EraserEnabled = false;
     QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

// //////////////////////////////////////////////EVENTS END/////////////////////////////////////////////////


void LvlScene::setItemSourceData(QGraphicsItem * it, QString ObjType)
{
    gridSize = pConfigs->default_grid;
    offsetX = 0;
    offsetY = 0;

    if( ObjType == "NPC")
    {
        sourcePos = QPoint(  ((ItemNPC *)it)->npcData.x, ((ItemNPC *)it)->npcData.y);
        gridSize = ((ItemNPC *)it)->gridSize;
        offsetX = ((ItemNPC *)it)->localProps.grid_offset_x;
        offsetY = ((ItemNPC *)it)->localProps.grid_offset_y;
    }
    else
    if( ObjType == "Block")
    {
        sourcePos = QPoint(  ((ItemBlock *)it)->blockData.x, ((ItemBlock *)it)->blockData.y);
        gridSize = ((ItemBlock *)it)->gridSize;
        //WriteToLog(QtDebugMsg, QString(" >>Check collision for Block"));
    }
    else
    if( ObjType == "BGO")
    {
        sourcePos = QPoint(  ((ItemBGO *)it)->bgoData.x, ((ItemBGO *)it)->bgoData.y);
        gridSize = ((ItemBGO *)it)->gridSize;
        offsetX = ((ItemBGO *)it)->gridOffsetX;
        offsetY = ((ItemBGO *)it)->gridOffsetY;
    }
    else
    if( ObjType == "Water")
    {
        sourcePos = QPoint(  ((ItemWater *)it)->waterData.x, ((ItemWater *)it)->waterData.y);
        gridSize = qRound(qreal(pConfigs->default_grid)/2);
    }
    else
    if( ObjType == "Door_enter")
    {
        sourcePos = QPoint(  ((ItemDoor *)it)->doorData.ix, ((ItemDoor *)it)->doorData.iy);
        gridSize = qRound(qreal(pConfigs->default_grid)/2);
    }
    else
    if( ObjType == "Door_exit"){
        sourcePos = QPoint(  ((ItemDoor *)it)->doorData.ox, ((ItemDoor *)it)->doorData.oy);
        gridSize = qRound(qreal(pConfigs->default_grid)/2);
    }
    else
    if(( ObjType == "player1" ) || ( ObjType == "player2" ))
    {
        offsetY = 2;
        gridSize = 2 ;
        int plrId=0;
        if( ObjType == "player1" )
            plrId=1;
        if( ObjType == "player2" )
            plrId=2;

        foreach(PlayerPoint pnt, LvlData->players)
        {
         if(pnt.id == (unsigned int)plrId)
         {
             sourcePos = QPoint(pnt.x, pnt.y);
             break;
         }
        }
    }
}

void LvlScene::placeItemsByRectArray()
{
    //This function placing items by yellow rectangles
    if(item_rectangles::rectArray.isEmpty()) return;

    QGraphicsItem * backup = cursor;
    while(!item_rectangles::rectArray.isEmpty())
    {
        cursor = item_rectangles::rectArray.first();
        item_rectangles::rectArray.pop_front();

        foreach(dataFlag flag, LvlPlacingItems::flags)
            cursor->setData(flag.first, flag.second);

        placeItemUnderCursor();

        if(cursor) delete cursor;
    }
    cursor = backup;
    cursor->hide();

    if(!overwritedItems.blocks.isEmpty()||
        !overwritedItems.bgo.isEmpty()||
        !overwritedItems.npc.isEmpty() )
    {
        addOverwriteHistory(overwritedItems, placingItems);
        overwritedItems.blocks.clear();
        overwritedItems.bgo.clear();
        overwritedItems.npc.clear();
        placingItems.blocks.clear();
        placingItems.bgo.clear();
        placingItems.npc.clear();
    }
    else
    if(!placingItems.blocks.isEmpty()||
            !placingItems.bgo.isEmpty()||
            !placingItems.npc.isEmpty()){
        addPlaceHistory(placingItems);
        placingItems.blocks.clear();
        placingItems.bgo.clear();
        placingItems.npc.clear();
    }

}


void LvlScene::placeItemUnderCursor()
{
    using namespace lvl_control;
    bool wasPlaced=false;

//    last_block_arrayID=LvlData->blocks_array_id;
//    last_bgo_arrayID=LvlData->bgo_array_id;
//    last_npc_arrayID=LvlData->npc_array_id;


    if(LvlPlacingItems::overwriteMode)
    {   //remove all colliaded items before placing
        QGraphicsItem * xxx;
        while( (xxx=itemCollidesWith(cursor)) != NULL )
        {
            if(xxx->data(0).toString()=="Block")
            {
                if(xxx->data(2).toLongLong()>last_block_arrayID) break;
                overwritedItems.blocks.push_back( ((ItemBlock *)xxx)->blockData );
                ((ItemBlock *)xxx)->removeFromArray();
                delete xxx;
            }
            else
            if(xxx->data(0).toString()=="BGO")
            {
                if(xxx->data(2).toLongLong()>last_bgo_arrayID) break;
                overwritedItems.bgo.push_back( ((ItemBGO *)xxx)->bgoData );
                ((ItemBGO *)xxx)->removeFromArray();
                delete xxx;
            }
            else
            if(xxx->data(0).toString()=="NPC")
            {
                if(xxx->data(2).toLongLong()>last_npc_arrayID) break;
                overwritedItems.npc.push_back( ((ItemNPC *)xxx)->npcData );
                ((ItemNPC *)xxx)->removeFromArray();
                delete xxx;
            }
        }
    }

    if( itemCollidesWith(cursor) )
    {
        return;
    }
    else
    {
        if(placingItem == PLC_Block)
        {
            LvlPlacingItems::blockSet.x = cursor->scenePos().x();
            LvlPlacingItems::blockSet.y = cursor->scenePos().y();

            LvlData->blocks_array_id++;
            LvlPlacingItems::blockSet.array_id = LvlData->blocks_array_id;

            LvlData->blocks.push_back(LvlPlacingItems::blockSet);
            placeBlock(LvlPlacingItems::blockSet, true);
            placingItems.blocks.push_back(LvlPlacingItems::blockSet);
            wasPlaced=true;
        }
        else
        if(placingItem == PLC_BGO)
        {
            LvlPlacingItems::bgoSet.x = cursor->scenePos().x();
            LvlPlacingItems::bgoSet.y = cursor->scenePos().y();

            LvlData->bgo_array_id++;
            LvlPlacingItems::bgoSet.array_id = LvlData->bgo_array_id;

            LvlData->bgo.push_back(LvlPlacingItems::bgoSet);
            placeBGO(LvlPlacingItems::bgoSet, true);
            placingItems.bgo.push_back(LvlPlacingItems::bgoSet);
            wasPlaced=true;
        }
        else
        if(placingItem == PLC_NPC)
        {
            LvlPlacingItems::npcSet.x = cursor->scenePos().x();
            LvlPlacingItems::npcSet.y = cursor->scenePos().y();

            LvlData->npc_array_id++;
            LvlPlacingItems::npcSet.array_id = LvlData->npc_array_id;

            LvlData->npc.push_back(LvlPlacingItems::npcSet);

            placeNPC(LvlPlacingItems::npcSet, true);

            placingItems.npc.push_back(LvlPlacingItems::npcSet);

            if(opts.animationEnabled) stopAnimation();
            if(opts.animationEnabled) startBlockAnimation();

            wasPlaced=true;
        }
        else
        if(placingItem == PLC_PlayerPoint)
        {
            foreach(PlayerPoint pnt, LvlData->players)
            {
             if(pnt.id == (unsigned int)LvlPlacingItems::playerID+1)
             {
                 QList<QVariant> oData;
                 oData.push_back(pnt.id);
                 oData.push_back((qlonglong)pnt.x);
                 oData.push_back((qlonglong)pnt.y);
                 oData.push_back((qlonglong)pnt.w);
                 oData.push_back((qlonglong)pnt.h);
                 pnt.x = cursor->scenePos().x();
                 pnt.y = cursor->scenePos().y();
                 pnt.w = 24;
                 if(LvlPlacingItems::playerID==0)
                    pnt.h = 54;
                 else
                    pnt.h = 60;
                 placePlayerPoint(pnt);

                 addPlacePlayerPointHistory(pnt, QVariant(oData));

                 break;
             }
            }

        }
        else
        if(placingItem == PLC_Door)
        {
            foreach(LevelDoors door, LvlData->doors)
            {
             if(door.array_id == (unsigned int)LvlPlacingItems::doorArrayId)
             {
                if(LvlPlacingItems::doorType==LvlPlacingItems::DOOR_Entrance)
                {
                    if(!door.isSetIn)
                    {
                        door.ix = cursor->scenePos().x();
                        door.iy = cursor->scenePos().y();
                        if((door.lvl_i)||(door.lvl_o))
                        {
                            door.ox = door.ix;
                            door.oy = door.iy;
                        }
                        door.isSetIn=true;
                        addPlaceDoorHistory(door.array_id, true, door.ix, door.iy);
                        placeDoorEnter(door, false, false);
                    }
                }
                else
                {
                    if(!door.isSetOut)
                    {
                        door.ox = cursor->scenePos().x();
                        door.oy = cursor->scenePos().y();
                        if((door.lvl_i)||(door.lvl_o))
                        {
                            door.ix = door.ox;
                            door.iy = door.oy;
                        }
                        door.isSetOut=true;
                        addPlaceDoorHistory(door.array_id, false, door.ox, door.oy);
                        placeDoorExit(door, false, false);
                    }
                }
                MainWinConnect::pMainWin->setDoorData(-2);
                break;
             }
            }

        }
    }
    if(wasPlaced)
    {
        LvlData->modified = true;
    }

    //if(opts.animationEnabled) stopAnimation();
    //if(opts.animationEnabled) startBlockAnimation();
}

void LvlScene::removeItemUnderCursor()
{
    if(contextMenuOpened) return;

    QGraphicsItem * findItem;
    //bool removeIt=true;
    findItem = itemCollidesCursor(cursor);
    removeLvlItem(findItem, true);
}


void LvlScene::removeSelectedLvlItems()
{
    QList<QGraphicsItem*> selectedList = selectedItems();
    if(selectedList.isEmpty()) return;
    removeLvlItems(selectedList);
}

void LvlScene::removeLvlItem(QGraphicsItem * item, bool globalHistory)
{
    if(!item) return;
    QList<QGraphicsItem * > items;
    items.push_back(item);
    removeLvlItems(items, globalHistory);
}

void LvlScene::removeLvlItems(QList<QGraphicsItem * > items, bool globalHistory)
{
    LevelData historyBuffer;
    bool deleted=false;
    QString objType;

    for (QList<QGraphicsItem*>::iterator it = items.begin(); it != items.end(); it++)
    {
            objType=(*it)->data(0).toString();

            if(!(*it)->isVisible()) continue;  //Invisible items can't be deleted

            //remove data from main array before deletion item from scene
            if( objType=="Block" )
            {
                if((lock_block)|| (((ItemBlock *)(*it))->isLocked) ) continue;

                historyBuffer.blocks.push_back(((ItemBlock*)(*it))->blockData);
                ((ItemBlock *)(*it))->removeFromArray();
                if((*it)) delete (*it);
                deleted=true;
            }
            else
            if( objType=="BGO" )
            {
                if((lock_bgo)|| (((ItemBGO *)(*it))->isLocked) ) continue;

                historyBuffer.bgo.push_back(((ItemBGO*)(*it))->bgoData);
                ((ItemBGO *)(*it))->removeFromArray();
                if((*it)) delete (*it);
                deleted=true;
            }
            else
            if( objType=="NPC" )
            {
                if((lock_npc)|| (((ItemNPC *)(*it))->isLocked) ) continue;

                historyBuffer.npc.push_back(((ItemNPC*)(*it))->npcData);
                ((ItemNPC *)(*it))->removeFromArray();
                if((*it)) delete (*it);
                deleted=true;
            }
            else
            if( objType=="Water" )
            {
                if((lock_water)|| (((ItemWater *)(*it))->isLocked) ) continue;

                historyBuffer.water.push_back(((ItemWater*)(*it))->waterData);
                ((ItemWater *)(*it))->removeFromArray();
                if((*it)) delete (*it);
                deleted=true;
            }
            else
            if(( objType=="Door_enter" )||( objType=="Door_exit" ))
            {
                if((lock_door)|| (((ItemDoor *)(*it))->isLocked) ) continue;

                bool isEntrance = (objType=="Door_enter");
                LevelDoors doorData = ((ItemDoor *)(*it))->doorData;
                if(isEntrance){
                    doorData.isSetIn = true;
                    doorData.isSetOut = false;
                }else{
                    doorData.isSetIn = false;
                    doorData.isSetOut = true;
                }
                historyBuffer.doors.push_back(doorData);
                ((ItemDoor *)(*it))->removeFromArray();
                if((*it)) delete (*it);
                MainWinConnect::pMainWin->setDoorData(-2);
                deleted=true;
            }
            else
            if(( objType=="player1" )||( objType=="player2" ))
            {
                unsigned long player=1;

                if(objType=="player1")
                    player=1;
                if(objType=="player2")
                    player=2;

                for(int plr=0; plr<LvlData->players.size(); plr++)
                {
                 if(LvlData->players[plr].id == player)
                 {
                     historyBuffer.players.push_back(LvlData->players[plr]);

                     LvlData->players[plr].x = 0;
                     LvlData->players[plr].y = 0;
                     LvlData->players[plr].w = 0;
                     LvlData->players[plr].h = 0;
                     deleted=true;
                     if((*it)) delete (*it);
                     break;
                 }
                }
            }
    }

    if(deleted)
    {
        if(globalHistory)
        {
            overwritedItems.blocks << historyBuffer.blocks;
            overwritedItems.bgo << historyBuffer.bgo;
            overwritedItems.npc << historyBuffer.npc;
            overwritedItems.water << historyBuffer.water;
            overwritedItems.doors << historyBuffer.doors;
            overwritedItems.players << historyBuffer.players;
        }
        else
            addRemoveHistory(historyBuffer);
    }
}



// /////////////////////////////Open properties window of selected item////////////////////////////////
void LvlScene::openProps()
{
    QList<QGraphicsItem * > items = this->selectedItems();
    if(!items.isEmpty())
    {
        if(items.first()->data(0).toString()=="Block")
        {
            MainWinConnect::pMainWin->LvlItemProps(0,
                          ((ItemBlock *)items.first())->blockData,
                          FileFormats::dummyLvlBgo(),
                          FileFormats::dummyLvlNpc(), false);
        }
        else
        if(items.first()->data(0).toString()=="BGO")
        {
            MainWinConnect::pMainWin->LvlItemProps(1,
                              FileFormats::dummyLvlBlock(),
                              ((ItemBGO *)items.first())->bgoData,
                              FileFormats::dummyLvlNpc(), false);
        }
        else
        if(items.first()->data(0).toString()=="NPC")
        {
            MainWinConnect::pMainWin->LvlItemProps(2,
                              FileFormats::dummyLvlBlock(),
                              FileFormats::dummyLvlBgo(),
                              ((ItemNPC *)items.first())->npcData, false);
        }
        else
        MainWinConnect::pMainWin->LvlItemProps(-1,
                                               FileFormats::dummyLvlBlock(),
                                               FileFormats::dummyLvlBgo(),
                                               FileFormats::dummyLvlNpc());
    }
    else
    {
        MainWinConnect::pMainWin->LvlItemProps(-1,
                                               FileFormats::dummyLvlBlock(),
                                               FileFormats::dummyLvlBgo(),
                                               FileFormats::dummyLvlNpc());
    }

    QGraphicsScene::selectionChanged();
}


// ////////////////////Sync settings of warp points with opened warp's settings/////////////////////////
///
/// \brief LvlScene::doorPointsSync
/// \param arrayID        Array ID of warp entry which is a key for found items on the map
/// \param remove         Remove warp points from the map because warp entry will be removed
///
void LvlScene::doorPointsSync(long arrayID, bool remove)
{

    bool doorExist=false;
    bool doorEntranceSynced=false;
    bool doorExitSynced=false;

    int i=0;
    //find doorItem in array
    for(i=0; i<LvlData->doors.size(); i++)
    {
        if(LvlData->doors[i].array_id==(unsigned int)arrayID)
        {
            doorExist=true;
            break;
        }
    }
    if(!doorExist) return;

    //get ItemList
    QList<QGraphicsItem * > items = this->items();

    foreach(QGraphicsItem * item, items)
    {
        if((!LvlData->doors[i].isSetIn)&&(!LvlData->doors[i].isSetOut)) break; //Don't sync door points if not placed

        if((item->data(0).toString()=="Door_enter")&&(item->data(2).toInt()==arrayID))
        {
            if((! (((!LvlData->doors[i].lvl_o) && (!LvlData->doors[i].lvl_i)) ||
                   ((LvlData->doors[i].lvl_o) && (!LvlData->doors[i].lvl_i)))
                )||(remove))
            {
                ((ItemDoor *)item)->doorData = LvlData->doors[i];
                ((ItemDoor *)item)->removeFromArray();
                delete ((ItemDoor *)item);
                doorEntranceSynced = true;
            }
            else
            {
                LvlData->doors[i].isSetIn=true;
                ((ItemDoor *)item)->doorData = LvlData->doors[i];
                doorEntranceSynced = true;
            }
        }

        if((item->data(0).toString()=="Door_exit")&&(item->data(2).toInt()==arrayID))
        {
            if( (! (((!LvlData->doors[i].lvl_o) && (!LvlData->doors[i].lvl_i)) ||
                                      (LvlData->doors[i].lvl_i) ) )||(remove))
            {
                ((ItemDoor *)item)->doorData = LvlData->doors[i];
                ((ItemDoor *)item)->removeFromArray();
                delete ((ItemDoor *)item);
                doorExitSynced = true;
            }
            else
            {
                LvlData->doors[i].isSetOut=true;
                ((ItemDoor *)item)->doorData = LvlData->doors[i];
                doorExitSynced = true;
            }
        }
        if((doorEntranceSynced)&&(doorExitSynced)) break; // stop fetch, because door points was synced
    }


}
