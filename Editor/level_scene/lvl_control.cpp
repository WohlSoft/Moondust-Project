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

#include "lvl_scene.h"
#include "../edit_level/level_edit.h"
#include <QtMath>

#include "item_block.h"
#include "item_bgo.h"
#include "item_npc.h"
#include "item_water.h"
#include "item_door.h"
#include "item_playerpoint.h"

#include "../common_features/mainwinconnect.h"
#include "../common_features/grid.h"
#include "lvl_item_placing.h"

#include "../file_formats/file_formats.h"

#include "../common_features/item_rectangles.h"

#include "../defines.h"


// //////////////////////////////////////////////EVENTS START/////////////////////////////////////////////////

void LvlScene::keyPressEvent ( QKeyEvent * keyEvent )
{
    if(CurrentMode) CurrentMode->keyPress(keyEvent);
    QGraphicsScene::keyPressEvent(keyEvent);
}

void LvlScene::keyReleaseEvent ( QKeyEvent * keyEvent )
{
    if(CurrentMode) CurrentMode->keyRelease(keyEvent);
    QGraphicsScene::keyReleaseEvent(keyEvent);

//    switch(keyEvent->key())
//    {
//    case (Qt::Key_Delete): //Delete action
//        removeSelectedLvlItems();
//        break;
//    case (Qt::Key_Escape):
//        if(!mouseMoved)
//            this->clearSelection();

//        resetResizers();

//        if(EditingMode == MODE_PlacingNew || EditingMode == MODE_DrawSquare || EditingMode == MODE_Line){
//            item_rectangles::clearArray();
//            MainWinConnect::pMainWin->on_actionSelect_triggered();
//            return;
//        }
//            //setSectionResizer(false, false);
//        break;
//    case (Qt::Key_Enter):
//    case (Qt::Key_Return):

//        applyResizers();
//            //setSectionResizer(false, true);
//        break;

//    default:
//        break;
//    }
//    QGraphicsScene::keyReleaseEvent(keyEvent);

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
    if(MousePressEventOnly)
    {
        QGraphicsScene::mousePressEvent(mouseEvent);
        MousePressEventOnly = false;
        return;
    }

    #ifdef _DEBUG_
    WriteToLog(QtDebugMsg, QString("Mouse pressed -> [%1, %2] contextMenuOpened=%3, DrawMode=%4").arg(mouseEvent->scenePos().x()).arg(mouseEvent->scenePos().y())
               .arg(contextMenuOpened).arg(DrawMode));
    #endif

    contextMenuOpened=false;
    IsMoved = false;

    //Discard multi mouse key
    int mSum = 0;
    if( mouseEvent->buttons() & Qt::LeftButton ) mSum++;
    if( mouseEvent->buttons() & Qt::MiddleButton ) mSum++;
    if( mouseEvent->buttons() & Qt::RightButton ) mSum++;
    if( mSum > 1 )
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

    if(CurrentMode) CurrentMode->mousePress(mouseEvent);

    if(CurrentMode->noEvent()) return;

    if((disableMoveItems) && (mouseEvent->buttons() & Qt::LeftButton)
       && (Qt::ControlModifier != QApplication::keyboardModifiers()))
    { return; }

    QGraphicsScene::mousePressEvent(mouseEvent);

    #ifdef _DEBUG_
    WriteToLog(QtDebugMsg, QString("mousePress -> done"));
    #endif
    haveSelected=(!selectedItems().isEmpty());


//    switch(EditingMode)
//    {
//        case MODE_PlacingNew:
//        {
//            if( mouseEvent->buttons() & Qt::RightButton )
//            {
//                item_rectangles::clearArray();
//                MainWinConnect::pMainWin->on_actionSelect_triggered();
//                return;
//            }

//            last_block_arrayID=LvlData->blocks_array_id;
//            last_bgo_arrayID=LvlData->bgo_array_id;
//            last_npc_arrayID=LvlData->npc_array_id;

//            if(cursor){
//                cursor->setPos( QPointF(applyGrid( mouseEvent->scenePos().toPoint()-
//                                                   QPoint(LvlPlacingItems::c_offset_x,
//                                                          LvlPlacingItems::c_offset_y),
//                                                   LvlPlacingItems::gridSz,
//                                                   LvlPlacingItems::gridOffset)));
//            }
//            placeItemUnderCursor();
//            Debugger_updateItemList();
//            QGraphicsScene::mousePressEvent(mouseEvent);
//            return;
//            break;
//        }
//        case MODE_DrawSquare:
//        {
//            if( mouseEvent->buttons() & Qt::RightButton )
//            {
//                item_rectangles::clearArray();
//                MainWinConnect::pMainWin->on_actionSelect_triggered();
//                return;
//            }

//            last_block_arrayID=LvlData->blocks_array_id;
//            last_bgo_arrayID=LvlData->bgo_array_id;
//            last_npc_arrayID=LvlData->npc_array_id;

//            WriteToLog(QtDebugMsg, QString("Square mode %1").arg(EditingMode));
//            if(cursor){
//                drawStartPos = QPointF(applyGrid( mouseEvent->scenePos().toPoint(),
//                                                  LvlPlacingItems::gridSz,
//                                                  LvlPlacingItems::gridOffset));
//                cursor->setPos( drawStartPos );
//                cursor->setVisible(true);

//                QPoint hw = applyGrid( mouseEvent->scenePos().toPoint(),
//                                       LvlPlacingItems::gridSz,
//                                       LvlPlacingItems::gridOffset);

//                QSize hs = QSize( (long)fabs(drawStartPos.x() - hw.x()),  (long)fabs( drawStartPos.y() - hw.y() ) );
//                dynamic_cast<QGraphicsRectItem *>(cursor)->setRect(0,0, hs.width(), hs.height());
//            }

//            QGraphicsScene::mousePressEvent(mouseEvent);
//            return;
//            break;
//        }
//        case MODE_Line:
//        {
//            if( mouseEvent->buttons() & Qt::RightButton )
//            {
//                item_rectangles::clearArray();
//                MainWinConnect::pMainWin->on_actionSelect_triggered();
//                return;
//            }

//            last_block_arrayID=LvlData->blocks_array_id;
//            last_bgo_arrayID=LvlData->bgo_array_id;
//            last_npc_arrayID=LvlData->npc_array_id;

//            WriteToLog(QtDebugMsg, QString("Line mode %1").arg(EditingMode));

//            if(cursor){
//                drawStartPos = QPointF(applyGrid( mouseEvent->scenePos().toPoint()-
//                                                  QPoint(LvlPlacingItems::c_offset_x,
//                                                         LvlPlacingItems::c_offset_y),
//                                                  LvlPlacingItems::gridSz,
//                                                  LvlPlacingItems::gridOffset));
//                //cursor->setPos( drawStartPos );
//                cursor->setVisible(true);

//                QPoint hw = applyGrid( mouseEvent->scenePos().toPoint()-
//                                       QPoint(LvlPlacingItems::c_offset_x,
//                                              LvlPlacingItems::c_offset_y),
//                                       LvlPlacingItems::gridSz,
//                                       LvlPlacingItems::gridOffset);
//                dynamic_cast<QGraphicsLineItem *>(cursor)->setLine(drawStartPos.x(), drawStartPos.y(), hw.x(), hw.y());
//            }

//            QGraphicsScene::mousePressEvent(mouseEvent);
//            return;
//            break;

//        }
//        case MODE_Resizing:
//        {
//            QGraphicsScene::mousePressEvent(mouseEvent);
//            return;
//        }
//        case MODE_Erasing:
//        {
//            if( mouseEvent->buttons() & Qt::RightButton )
//            {
//                MainWinConnect::pMainWin->on_actionSelect_triggered();
//                return;
//            }

//            if(cursor){
//               cursor->show();
//               cursor->setPos(mouseEvent->scenePos());
//            }

//            QGraphicsScene::mousePressEvent(mouseEvent);

//            QList<QGraphicsItem*> selectedList = selectedItems();
//            if (!selectedList.isEmpty())
//            {
//                removeItemUnderCursor();
//                EraserEnabled=true;
//            }
//            break;
//        }
//        case MODE_PasteFromClip: //Pasta
//        {
//            if( mouseEvent->buttons() & Qt::RightButton )
//            {
//                MainWinConnect::pMainWin->on_actionSelect_triggered();
//                return;
//            }
//            PasteFromBuffer = true;
//            break;
//        }
//        default:
//        {

//     if(CurrentMode->noEvent()) return;


//            if((disableMoveItems) && (mouseEvent->buttons() & Qt::LeftButton)
//                && (Qt::ControlModifier != QApplication::keyboardModifiers()))
//            { return; }

//            QGraphicsScene::mousePressEvent(mouseEvent);

////            break;
////        }
////    }
//    #ifdef _DEBUG_
//    WriteToLog(QtDebugMsg, QString("mousePress -> done"));
//    #endif
//    haveSelected=(!selectedItems().isEmpty());

}

void LvlScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(MouseMoveEventOnly)
    {
        QGraphicsScene::mousePressEvent(mouseEvent);
        MouseMoveEventOnly = false;
        return;
    }

    MainWinConnect::pMainWin->Debugger_UpdateMousePosition(mouseEvent->scenePos().toPoint());

    #ifdef _DEBUG_
    WriteToLog(QtDebugMsg, QString("Mouse moved -> [%1, %2]").arg(mouseEvent->scenePos().x()).arg(mouseEvent->scenePos().y()));
    #endif
    //if(contextMenuOpened) return;
    contextMenuOpened=false;
    IsMoved = true;

    if(CurrentMode) CurrentMode->mouseMove(mouseEvent);

    if(CurrentMode->noEvent()) return;

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


//    switch(EditingMode)
//    {
//    case MODE_PlacingNew:
//        {
//            this->clearSelection();

//            if((!LvlPlacingItems::layer.isEmpty() && LvlPlacingItems::layer!="Default")||(mouseEvent->modifiers() & Qt::ControlModifier) )
//                setMessageBoxItem(true, mouseEvent->scenePos(),
//                                           ((!LvlPlacingItems::layer.isEmpty() && LvlPlacingItems::layer!="Default")?
//                                LvlPlacingItems::layer + ", ":"") +
//                                           (cursor?
//                                                (
//                                           QString::number( cursor->scenePos().toPoint().x() ) + "x" +
//                                           QString::number( cursor->scenePos().toPoint().y() )
//                                                )
//                                                    :"")
//                                           );
//            else
//                setMessageBoxItem(false);

//            if(cursor)
//            {
//                       cursor->setPos( QPointF(applyGrid( QPointF(mouseEvent->scenePos()-
//                                                           QPointF(LvlPlacingItems::c_offset_x,
//                                                                  LvlPlacingItems::c_offset_y)).toPoint(),
//                                                         LvlPlacingItems::gridSz,
//                                                         LvlPlacingItems::gridOffset)));
//                       cursor->show();
//            }
//            if( mouseEvent->buttons() & Qt::LeftButton )
//            {
//                placeItemUnderCursor();
//                Debugger_updateItemList();
//            }
//            //QGraphicsScene::mouseMoveEvent(mouseEvent);
//            break;
//        }
//    case MODE_DrawSquare:
//        {


//        if(!LvlPlacingItems::layer.isEmpty() && LvlPlacingItems::layer!="Default")
//            setMessageBoxItem(true, mouseEvent->scenePos(), LvlPlacingItems::layer + ", " +
//                         QString::number( mouseEvent->scenePos().toPoint().x() ) + "x" +
//                         QString::number( mouseEvent->scenePos().toPoint().y() )
//                          );
//        else
//            setMessageBoxItem(false);

//            if(cursor)
//            {
//                if(cursor->isVisible())
//                {
//                QPoint hw = applyGrid( mouseEvent->scenePos().toPoint(),
//                                       LvlPlacingItems::gridSz,
//                                       LvlPlacingItems::gridOffset);

//                QSize hs = QSize( (long)fabs(drawStartPos.x() - hw.x()),  (long)fabs( drawStartPos.y() - hw.y() ) );


//                dynamic_cast<QGraphicsRectItem *>(cursor)->setRect(0,0, hs.width(), hs.height());
//                dynamic_cast<QGraphicsRectItem *>(cursor)->setPos(
//                            ((hw.x() < drawStartPos.x() )? hw.x() : drawStartPos.x()),
//                            ((hw.y() < drawStartPos.y() )? hw.y() : drawStartPos.y())
//                            );

//                if(((placingItem==PLC_Block)&&(!LvlPlacingItems::sizableBlock))||(placingItem==PLC_BGO))
//                {
//                item_rectangles::drawMatrix(this, QRect (dynamic_cast<QGraphicsRectItem *>(cursor)->x(),
//                                                         dynamic_cast<QGraphicsRectItem *>(cursor)->y(),
//                                                         dynamic_cast<QGraphicsRectItem *>(cursor)->rect().width(),
//                                                         dynamic_cast<QGraphicsRectItem *>(cursor)->rect().height()),
//                                            QSize(LvlPlacingItems::itemW, LvlPlacingItems::itemH)
//                                            );
//                }

//                }
//            }
//            break;
//        }
//    case MODE_Line:
//        {

//        if(!LvlPlacingItems::layer.isEmpty() && LvlPlacingItems::layer!="Default")
//            setMessageBoxItem(true, mouseEvent->scenePos(), LvlPlacingItems::layer + ", " +
//                         QString::number( mouseEvent->scenePos().toPoint().x() ) + "x" +
//                         QString::number( mouseEvent->scenePos().toPoint().y() )
//                          );
//        else
//            setMessageBoxItem(false);

//            if(cursor)
//            {
//                if(cursor->isVisible())
//                {
//                    QPoint hs = applyGrid( mouseEvent->scenePos().toPoint()-
//                                           QPoint(LvlPlacingItems::c_offset_x,
//                                                  LvlPlacingItems::c_offset_y),
//                                           LvlPlacingItems::gridSz,
//                                           LvlPlacingItems::gridOffset);

//                    QLineF s = item_rectangles::snapLine(QLineF(drawStartPos.x(),drawStartPos.y(), (qreal)hs.x(), (qreal)hs.y()),
//                                                         QSizeF((qreal)LvlPlacingItems::itemW, (qreal)LvlPlacingItems::itemH) );

//                    QPoint hw = applyGrid( s.p2().toPoint(),
//                                        LvlPlacingItems::gridSz,
//                                        LvlPlacingItems::gridOffset);

//                    s.setP2(QPointF((qreal)hw.x(),(qreal)hw.y()));

//                    dynamic_cast<QGraphicsLineItem *>(cursor)->setLine(s);

//                    item_rectangles::drawLine(this, s,
//                           QSize(LvlPlacingItems::itemW, LvlPlacingItems::itemH)
//                                                );

//                }
//            }
//            break;
//        }
//    case MODE_Resizing:
//        {
//            this->clearSelection();
//            QGraphicsScene::mouseMoveEvent(mouseEvent);
//            return;
//        break;
//        }
//    case MODE_Erasing:
//        {
//            if(cursor) cursor->setPos(mouseEvent->scenePos());
//            if (EraserEnabled)// Remove All items, placed under Cursor
//            {
//                removeItemUnderCursor();
//                Debugger_updateItemList();
//            }
//            break;
//        }
//    default:
        //if(!( mouseEvent->buttons() & Qt::LeftButton )) return;
        //if(cursor) cursor->setPos(mouseEvent->scenePos());
//      break;
//    }

//    if(CurrentMode->noEvent()) return;

//    haveSelected=(!selectedItems().isEmpty());
//    if(haveSelected)
//    {
//        if(!( mouseEvent->buttons() & Qt::LeftButton )) return;
//        if(!mouseMoved)
//        {
//            mouseMoved=true;
//        }
//    }
//        QGraphicsScene::mouseMoveEvent(mouseEvent);
    //WriteToLog(QtDebugMsg, QString("mouseMove -> done"));
}

void LvlScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(MouseReleaseEventOnly)
    {
        QGraphicsScene::mousePressEvent(mouseEvent);
        MouseReleaseEventOnly = false;
        return;
    }

    bool isLeftMouse=false;
    bool isMiddleMouse=false;

    if( mouseEvent->button() == Qt::LeftButton )
    {
        mouseLeft=false;
        isLeftMouse=true;
        WriteToLog(QtDebugMsg, QString("Left mouse button released [edit mode: %1]").arg(EditingMode));
    }
    if( mouseEvent->button() == Qt::MiddleButton )
    {
        mouseMid=false;
        isMiddleMouse=true;
        WriteToLog(QtDebugMsg, QString("Middle mouse button released [edit mode: %1]").arg(EditingMode));
    }
    if( mouseEvent->button() == Qt::RightButton )
    {
        mouseRight=false;
        WriteToLog(QtDebugMsg, QString("Right mouse button released [edit mode: %1]").arg(EditingMode));
    }

    contextMenuOpened=false;
    if(!isLeftMouse)
    {
        if(PasteFromBuffer && GlobalSettings::MidMouse_allowDuplicate && isMiddleMouse &&
                (EditingMode==MODE_Selecting||EditingMode==MODE_SelectingOnly))
        {
            clearSelection();
            paste( LvlBuffer, mouseEvent->scenePos().toPoint() );
            Debugger_updateItemList();
            PasteFromBuffer = false;
        }

        if(GlobalSettings::MidMouse_allowSwitchToDrag && isMiddleMouse &&
                (EditingMode==MODE_Selecting||EditingMode==MODE_SelectingOnly) && selectedItems().isEmpty())
        {
            MainWinConnect::pMainWin->on_actionHandScroll_triggered();
        }

        QGraphicsScene::mouseReleaseEvent(mouseEvent);
        return;
    }

    if(CurrentMode) CurrentMode->mouseRelease(mouseEvent);

    if(!CurrentMode->noEvent())
        QGraphicsScene::mouseReleaseEvent(mouseEvent);


//    switch(EditingMode)
//    {
//    case MODE_DrawSquare:
//        {

//        if(cursor)
//        {

//            // /////////// Don't draw with zero width or height //////////////
//            if( (dynamic_cast<QGraphicsRectItem *>(cursor)->rect().width()==0) ||
//              (dynamic_cast<QGraphicsRectItem *>(cursor)->rect().height()==0))
//            {
//                cursor->hide();
//                break;
//            }
//            // ///////////////////////////////////////////////////////////////

//            switch(placingItem)
//            {
//            case PLC_Water:
//                {
//                    LvlPlacingItems::waterSet.quicksand = (LvlPlacingItems::waterType==1);

//                    LvlPlacingItems::waterSet.x = cursor->scenePos().x();
//                    LvlPlacingItems::waterSet.y = cursor->scenePos().y();
//                    LvlPlacingItems::waterSet.w = dynamic_cast<QGraphicsRectItem *>(cursor)->rect().width();
//                    LvlPlacingItems::waterSet.h = dynamic_cast<QGraphicsRectItem *>(cursor)->rect().height();
//                    //here define placing water item.
//                    LvlData->physenv_array_id++;

//                    LvlPlacingItems::waterSet.array_id = LvlData->physenv_array_id;
//                    LvlData->physez.push_back(LvlPlacingItems::waterSet);

//                    placeWater(LvlPlacingItems::waterSet, true);
//                    LevelData plWater;
//                    plWater.physez.push_back(LvlPlacingItems::waterSet);
//                    addPlaceHistory(plWater);
//                    Debugger_updateItemList();
//                    break;
//                }
//            case PLC_Block:
//                {
//                    //LvlPlacingItems::waterSet.quicksand = (LvlPlacingItems::waterType==1);
//                    if(LvlPlacingItems::sizableBlock)
//                    {
//                        LvlPlacingItems::blockSet.x = cursor->scenePos().x();
//                        LvlPlacingItems::blockSet.y = cursor->scenePos().y();
//                        LvlPlacingItems::blockSet.w = dynamic_cast<QGraphicsRectItem *>(cursor)->rect().width();
//                        LvlPlacingItems::blockSet.h = dynamic_cast<QGraphicsRectItem *>(cursor)->rect().height();
//                        //here define placing water item.
//                        LvlData->blocks_array_id++;

//                        LvlPlacingItems::blockSet.array_id = LvlData->blocks_array_id;
//                        LvlData->blocks.push_back(LvlPlacingItems::blockSet);

//                        placeBlock(LvlPlacingItems::blockSet, true);
//                        LevelData plSzBlock;
//                        plSzBlock.blocks.push_back(LvlPlacingItems::blockSet);
//                        addPlaceHistory(plSzBlock);
//                        Debugger_updateItemList();
//                        break;
//                    }
//                    else
//                    {
//                        QPointF p = ((QGraphicsRectItem *)cursor)->scenePos();
//                        QSizeF s = ((QGraphicsRectItem *)cursor)->rect().size();

//                        WriteToLog(QtDebugMsg, "Get collision buffer");

//                        collisionCheckBuffer = this->items(QRectF(
//                                    p.x()-10, p.y()-10,
//                                    s.width()+20, s.height()+20),
//                                    Qt::IntersectsItemBoundingRect);
//                        if(collisionCheckBuffer.isEmpty())
//                            emptyCollisionCheck = true;
//                        else
//                            prepareCollisionBuffer();

//                        WriteToLog(QtDebugMsg, "Placing");
//                        placeItemsByRectArray();

//                        WriteToLog(QtDebugMsg, "clear collision buffer");
//                        emptyCollisionCheck = false;
//                        collisionCheckBuffer.clear();
//                        WriteToLog(QtDebugMsg, "Done");

//                        Debugger_updateItemList();
//                        break;
//                    }
//                }
//            case PLC_BGO:
//                {
//                    QPointF p = ((QGraphicsRectItem *)cursor)->scenePos();
//                    QSizeF s = ((QGraphicsRectItem *)cursor)->rect().size();

//                    collisionCheckBuffer = this->items(QRectF(
//                                p.x()-10, p.y()-10,
//                                s.width()+20, s.height()+20),
//                                Qt::IntersectsItemBoundingRect);

//                    if(collisionCheckBuffer.isEmpty())
//                        emptyCollisionCheck = true;
//                    else
//                        prepareCollisionBuffer();

//                    placeItemsByRectArray();

//                    emptyCollisionCheck = false;
//                    collisionCheckBuffer.clear();

//                    Debugger_updateItemList();
//                 break;
//                }
//            }
//            LvlData->modified = true;

//            cursor->hide();
//        }
//        break;
//        }
//    case MODE_Line:
//        {

//        if(cursor)
//        {
//            WriteToLog(QtDebugMsg, "Line tool -> Placing items");
//            placeItemsByRectArray();

//            Debugger_updateItemList();

//            LvlData->modified = true;
//            cursor->hide();
//        }
//        break;
//        }
//    case MODE_PlacingNew:
//    {
//        if(placingItem == PLC_Door)
//        {
//            MainWinConnect::pMainWin->on_actionSelect_triggered();
//            QGraphicsScene::mouseReleaseEvent(mouseEvent);
//            return;
//        }else{
//            if(!overwritedItems.blocks.isEmpty()||
//                !overwritedItems.bgo.isEmpty()||
//                !overwritedItems.npc.isEmpty() )
//            {
//                addOverwriteHistory(overwritedItems, placingItems);
//                overwritedItems.blocks.clear();
//                overwritedItems.bgo.clear();
//                overwritedItems.npc.clear();
//                placingItems.blocks.clear();
//                placingItems.bgo.clear();
//                placingItems.npc.clear();
//            }
//            else
//            if(!placingItems.blocks.isEmpty()||
//                    !placingItems.bgo.isEmpty()||
//                    !placingItems.npc.isEmpty())
//            {
//                addPlaceHistory(placingItems);
//                placingItems.blocks.clear();
//                placingItems.bgo.clear();
//                placingItems.npc.clear();
//            }
//        }
//        break;
//    }
//    case MODE_Erasing:
//    {
//        if(!overwritedItems.blocks.isEmpty()||
//            !overwritedItems.bgo.isEmpty()||
//            !overwritedItems.npc.isEmpty() )
//        {
//            addRemoveHistory(overwritedItems);
//            overwritedItems.blocks.clear();
//            overwritedItems.bgo.clear();
//            overwritedItems.npc.clear();
//        }
//        break;
//    }
//    default:
//        break;
//    }


//    if(DrawMode)
//    {
//        QGraphicsScene::mouseReleaseEvent(mouseEvent);
//        return;
//    }

//            cursor->hide();

//            haveSelected = false;

//            QString ObjType;
//            bool collisionPassed = false;

//            //History
//            LevelData historyBuffer; bool deleted=false;
//            LevelData historySourceBuffer;

//            if(PasteFromBuffer)
//            {
//                paste( LvlBuffer, mouseEvent->scenePos().toPoint() );
//                PasteFromBuffer = false;
//                IsMoved=false;
//                MainWinConnect::pMainWin->on_actionSelect_triggered();
//                Debugger_updateItemList();
//            }


//            QList<QGraphicsItem*> selectedList = selectedItems();

//            // check for grid snap
//            if ((!selectedList.isEmpty())&&(mouseMoved))
//            {

//                if(EditingMode==MODE_Erasing)
//                {
//                    removeLvlItems(selectedList);
//                    selectedList = selectedItems();
//                    Debugger_updateItemList();
//                }
//                else
//                    applyGroupGrid(selectedList);


//                if((EditingMode==MODE_Erasing)&&(deleted))
//                {
//                    addRemoveHistory(historyBuffer);
//                }
//                EraserEnabled = false;

//                // Check collisions
//                //Only if collision ckecking enabled
//                if(!PasteFromBuffer)
//                {
//                    if(opts.collisionsEnabled && checkGroupCollisions(&selectedList))
//                    {
//                        collisionPassed = false;
//                        returnItemBackGroup(selectedList);
//                    }
//                    else
//                    {
//                        collisionPassed = true;
//                        //applyArrayForItemGroup(selectedList);
//                        LvlData->modified=true;
//                    }
//                }


//                if((collisionPassed) || (!opts.collisionsEnabled))
//                for (QList<QGraphicsItem*>::iterator it = selectedList.begin(); it != selectedList.end(); it++)
//                { ////////////////////////SECOND FETCH///////////////////////
//                   ObjType = (*it)->data(0).toString();

//                   /////////////////////////GET DATA///////////////
//                   setItemSourceData((*it), (*it)->data(0).toString()); //Set Grid Size/Offset, sourcePosition
//                   /////////////////////////GET DATA/////////////////////

//                   //Check position
//                   if( (sourcePos == QPoint((long)((*it)->scenePos().x()), ((long)(*it)->scenePos().y()))))
//                   {
//                       mouseMoved=false;
//                       break; //break fetch when items is not moved
//                   }

//                   if( ObjType == "Block")
//                    {
//                        //WriteToLog(QtDebugMsg, QString(" >>Collision passed"));
//                        //Applay move into main array
//                        historySourceBuffer.blocks.push_back(dynamic_cast<ItemBlock *>(*it)->blockData);
//                        //dynamic_cast<ItemBlock *>(*it)->blockData.x = (long)(*it)->scenePos().x();
//                        //dynamic_cast<ItemBlock *>(*it)->blockData.y = (long)(*it)->scenePos().y();
//                        dynamic_cast<ItemBlock *>(*it)->arrayApply();
//                        historyBuffer.blocks.push_back(dynamic_cast<ItemBlock *>(*it)->blockData);
//                        LvlData->modified = true;
//                    }
//                    else
//                    if( ObjType == "BGO")
//                    {
//                        //Applay move into main array
//                        historySourceBuffer.bgo.push_back(dynamic_cast<ItemBGO *>(*it)->bgoData);
//                        //dynamic_cast<ItemBGO *>(*it)->bgoData.x = (long)(*it)->scenePos().x();
//                        //dynamic_cast<ItemBGO *>(*it)->bgoData.y = (long)(*it)->scenePos().y();
//                        dynamic_cast<ItemBGO *>(*it)->arrayApply();
//                        historyBuffer.bgo.push_back(dynamic_cast<ItemBGO *>(*it)->bgoData);
//                        LvlData->modified = true;
//                    }
//                    else
//                    if( ObjType == "NPC")
//                    {
//                        //Applay move into main array
//                        historySourceBuffer.npc.push_back(dynamic_cast<ItemNPC *>(*it)->npcData);
//                        //dynamic_cast<ItemNPC *>(*it)->npcData.x = (long)(*it)->scenePos().x();
//                        //dynamic_cast<ItemNPC *>(*it)->npcData.y = (long)(*it)->scenePos().y();
//                        dynamic_cast<ItemNPC *>(*it)->arrayApply();
//                        historyBuffer.npc.push_back(dynamic_cast<ItemNPC *>(*it)->npcData);
//                        LvlData->modified = true;
//                    }
//                    else
//                    if( ObjType == "Water")
//                    {
//                        //Applay move into main array
//                        historySourceBuffer.physez.push_back(dynamic_cast<ItemWater *>(*it)->waterData);
//                        //dynamic_cast<ItemWater *>(*it)->waterData.x = (long)(*it)->scenePos().x();
//                        //dynamic_cast<ItemWater *>(*it)->waterData.y = (long)(*it)->scenePos().y();
//                        dynamic_cast<ItemWater *>(*it)->arrayApply();
//                        historyBuffer.physez.push_back(dynamic_cast<ItemWater *>(*it)->waterData);
//                        LvlData->modified = true;
//                    }
//                    else
//                    if( ObjType == "Door_enter")
//                    {
//                        //Applay move into main array
//                        //historySourceBuffer.water.push_back(dynamic_cast<ItemWater *>(*it)->waterData);
//                        LevelDoors oldDoorData = dynamic_cast<ItemDoor *>(*it)->doorData;
//                        oldDoorData.isSetIn = true;
//                        oldDoorData.isSetOut = false;
//                        historySourceBuffer.doors.push_back(oldDoorData);
//                        dynamic_cast<ItemDoor *>(*it)->doorData.ix = (long)(*it)->scenePos().x();
//                        dynamic_cast<ItemDoor *>(*it)->doorData.iy = (long)(*it)->scenePos().y();
//                        if(( dynamic_cast<ItemDoor *>(*it)->doorData.lvl_i)||
//                             dynamic_cast<ItemDoor *>(*it)->doorData.lvl_o)
//                        {
//                            dynamic_cast<ItemDoor *>(*it)->doorData.ox = (long)(*it)->scenePos().x();
//                            dynamic_cast<ItemDoor *>(*it)->doorData.oy = (long)(*it)->scenePos().y();
//                        }

//                        dynamic_cast<ItemDoor *>(*it)->arrayApply();
//                        LevelDoors newDoorData = dynamic_cast<ItemDoor *>(*it)->doorData;
//                        newDoorData.isSetIn = true;
//                        newDoorData.isSetOut = false;
//                        historyBuffer.doors.push_back(newDoorData);
//                        //historyBuffer.water.push_back(dynamic_cast<ItemWater *>(*it)->waterData);
//                        LvlData->modified = true;
//                    }
//                    else
//                    if( ObjType == "Door_exit")
//                    {
//                        //Applay move into main array
//                        //historySourceBuffer.water.push_back(dynamic_cast<ItemWater *>(*it)->waterData);
//                        LevelDoors oldDoorData = dynamic_cast<ItemDoor *>(*it)->doorData;
//                        oldDoorData.isSetIn = false;
//                        oldDoorData.isSetOut = true;
//                        historySourceBuffer.doors.push_back(oldDoorData);
//                        dynamic_cast<ItemDoor *>(*it)->doorData.ox = (long)(*it)->scenePos().x();
//                        dynamic_cast<ItemDoor *>(*it)->doorData.oy = (long)(*it)->scenePos().y();
//                        if(( dynamic_cast<ItemDoor *>(*it)->doorData.lvl_i)||
//                             dynamic_cast<ItemDoor *>(*it)->doorData.lvl_o)
//                        {
//                            dynamic_cast<ItemDoor *>(*it)->doorData.ix = (long)(*it)->scenePos().x();
//                            dynamic_cast<ItemDoor *>(*it)->doorData.iy = (long)(*it)->scenePos().y();
//                        }
//                        dynamic_cast<ItemDoor *>(*it)->arrayApply();
//                        LevelDoors newDoorData = dynamic_cast<ItemDoor *>(*it)->doorData;
//                        newDoorData.isSetIn = false;
//                        newDoorData.isSetOut = true;
//                        historyBuffer.doors.push_back(newDoorData);
//                        //historyBuffer.water.push_back(dynamic_cast<ItemWater *>(*it)->waterData);
//                        LvlData->modified = true;
//                    }
//                    else
//                    if(ObjType == "playerPoint" )
//                    {
//                         historySourceBuffer.players.push_back(dynamic_cast<ItemPlayerPoint *>(*it)->pointData);
//                         //dynamic_cast<ItemPlayerPoint *>(*it)->pointData.x =(long)(*it)->scenePos().x();
//                         //dynamic_cast<ItemPlayerPoint *>(*it)->pointData.y =(long)(*it)->scenePos().y();
//                         dynamic_cast<ItemPlayerPoint *>(*it)->arrayApply();
//                         historyBuffer.players.push_back(dynamic_cast<ItemPlayerPoint *>(*it)->pointData);
//                    }
//                }////////////////////////SECOND FETCH///////////////////////

//                if((EditingMode==MODE_Selecting)&&(mouseMoved)) addMoveHistory(historySourceBuffer, historyBuffer);

//                mouseMoved = false;

//                QGraphicsScene::mouseReleaseEvent(mouseEvent);
//                return;
//            }
//      EraserEnabled = false;
//    if(!CurrentMode->noEvent())
//        QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

// //////////////////////////////////////////////EVENTS END/////////////////////////////////////////////////


//void LvlScene::setItemSourceData(QGraphicsItem * it, QString ObjType)
//{
//    Q_UNUSED(it);
//    Q_UNUSED(ObjType);
//    gridSize = pConfigs->default_grid;
//    offsetX = 0;
//    offsetY = 0;

//    if( ObjType == "NPC")
//    {
//        sourcePos = QPoint(  dynamic_cast<ItemNPC *>(it)->npcData.x, dynamic_cast<ItemNPC *>(it)->npcData.y);
//        gridSize = dynamic_cast<ItemNPC *>(it)->gridSize;
//        offsetX = dynamic_cast<ItemNPC *>(it)->localProps.grid_offset_x;
//        offsetY = dynamic_cast<ItemNPC *>(it)->localProps.grid_offset_y;
//    }
//    else
//    if( ObjType == "Block")
//    {
//        sourcePos = QPoint(  dynamic_cast<ItemBlock *>(it)->blockData.x, dynamic_cast<ItemBlock *>(it)->blockData.y);
//        gridSize = dynamic_cast<ItemBlock *>(it)->gridSize;
//        //WriteToLog(QtDebugMsg, QString(" >>Check collision for Block"));
//    }
//    else
//    if( ObjType == "BGO")
//    {
//        sourcePos = QPoint(  dynamic_cast<ItemBGO *>(it)->bgoData.x, dynamic_cast<ItemBGO *>(it)->bgoData.y);
//        gridSize = dynamic_cast<ItemBGO *>(it)->gridSize;
//        offsetX = dynamic_cast<ItemBGO *>(it)->gridOffsetX;
//        offsetY = dynamic_cast<ItemBGO *>(it)->gridOffsetY;
//    }
//    else
//    if( ObjType == "Water")
//    {
//        sourcePos = QPoint(  dynamic_cast<ItemWater *>(it)->waterData.x, dynamic_cast<ItemWater *>(it)->waterData.y);
//        gridSize = qRound(qreal(pConfigs->default_grid)/2);
//    }
//    else
//    if( ObjType == "Door_enter")
//    {
//        sourcePos = QPoint(  dynamic_cast<ItemDoor *>(it)->doorData.ix, dynamic_cast<ItemDoor *>(it)->doorData.iy);
//        gridSize = qRound(qreal(pConfigs->default_grid)/2);
//    }
//    else
//    if( ObjType == "Door_exit"){
//        sourcePos = QPoint(  dynamic_cast<ItemDoor *>(it)->doorData.ox, dynamic_cast<ItemDoor *>(it)->doorData.oy);
//        gridSize = qRound(qreal(pConfigs->default_grid)/2);
//    }
//    else
//    if( ObjType == "playerPoint" )
//    {
//        gridSize = 2 ;
//        sourcePos = QPoint(dynamic_cast<ItemPlayerPoint *>(it)->pointData.x, dynamic_cast<ItemPlayerPoint *>(it)->pointData.y);
//    }
//}

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
                overwritedItems.blocks.push_back( dynamic_cast<ItemBlock *>(xxx)->blockData );
                dynamic_cast<ItemBlock *>(xxx)->removeFromArray();
                delete xxx;
            }
            else
            if(xxx->data(0).toString()=="BGO")
            {
                if(xxx->data(2).toLongLong()>last_bgo_arrayID) break;
                overwritedItems.bgo.push_back( dynamic_cast<ItemBGO *>(xxx)->bgoData );
                dynamic_cast<ItemBGO *>(xxx)->removeFromArray();
                delete xxx;
            }
            else
            if(xxx->data(0).toString()=="NPC")
            {
                if(xxx->data(2).toLongLong()>last_npc_arrayID) break;
                overwritedItems.npc.push_back( dynamic_cast<ItemNPC *>(xxx)->npcData );
                dynamic_cast<ItemNPC *>(xxx)->removeFromArray();
                delete xxx;
            }
        }
    }

    QList<QGraphicsItem *> * checkZone;
    if(collisionCheckBuffer.isEmpty())
        checkZone = 0;
    else
        checkZone = &collisionCheckBuffer;

    if( !emptyCollisionCheck && itemCollidesWith(cursor, checkZone) )
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

            if(LvlPlacingItems::npcSpecialAutoIncrement)
            {
                LvlPlacingItems::npcSet.special_data = IncrementingNpcSpecialSpin;
                IncrementingNpcSpecialSpin++;
            }

            LvlData->npc_array_id++;
            LvlPlacingItems::npcSet.array_id = LvlData->npc_array_id;

            LvlData->npc.push_back(LvlPlacingItems::npcSet);

            placeNPC(LvlPlacingItems::npcSet, true);

            placingItems.npc.push_back(LvlPlacingItems::npcSet);
            wasPlaced=true;
        }
        else
        if(placingItem == PLC_PlayerPoint)
        {

                bool found=true;
                QList<QVariant> oData;
                PlayerPoint pnt = FileFormats::dummyLvlPlayerPoint(LvlPlacingItems::playerID+1);
                //Check exists point on map
                foreach(PlayerPoint ptr, LvlData->players)
                {
                    if(ptr.id == pnt.id)
                    {
                        found=true;
                        pnt=ptr;
                    }
                }

                if(!found)
                {
                    pnt.id=0;pnt.x=0;pnt.y=0;pnt.w=0;pnt.h=0;
                }

                oData.push_back(pnt.id);
                oData.push_back((qlonglong)pnt.x);
                oData.push_back((qlonglong)pnt.y);
                oData.push_back((qlonglong)pnt.w);
                oData.push_back((qlonglong)pnt.h);


                pnt = FileFormats::dummyLvlPlayerPoint(LvlPlacingItems::playerID+1);
                pnt.x = cursor->scenePos().x();
                pnt.y = cursor->scenePos().y();

                placePlayerPoint(pnt);

                WriteToLog(QtDebugMsg, QString("Placing player point %1 with position %2 %3, %4")
                           .arg(LvlPlacingItems::playerID+1)
                           .arg(cursor->scenePos().x())
                           .arg(cursor->scenePos().y())
                           );

            addPlacePlayerPointHistory(pnt, QVariant(oData));
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
    Debugger_updateItemList();
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
                if((lock_block)|| (dynamic_cast<ItemBlock *>(*it)->isLocked) ) continue;

                historyBuffer.blocks.push_back(dynamic_cast<ItemBlock *>(*it)->blockData);
                dynamic_cast<ItemBlock *>(*it)->removeFromArray();
                if((*it)) delete (*it);
                deleted=true;
            }
            else
            if( objType=="BGO" )
            {
                if((lock_bgo)|| (dynamic_cast<ItemBGO *>(*it)->isLocked) ) continue;

                historyBuffer.bgo.push_back(dynamic_cast<ItemBGO *>(*it)->bgoData);
                dynamic_cast<ItemBGO *>(*it)->removeFromArray();
                if((*it)) delete (*it);
                deleted=true;
            }
            else
            if( objType=="NPC" )
            {
                if((lock_npc)|| (dynamic_cast<ItemNPC *>(*it)->isLocked) ) continue;

                historyBuffer.npc.push_back(dynamic_cast<ItemNPC *>(*it)->npcData);
                dynamic_cast<ItemNPC *>(*it)->removeFromArray();
                if((*it)) delete (*it);
                deleted=true;
            }
            else
            if( objType=="Water" )
            {
                if((lock_water)|| (dynamic_cast<ItemWater *>(*it)->isLocked) ) continue;

                historyBuffer.physez.push_back(dynamic_cast<ItemWater *>(*it)->waterData);
                dynamic_cast<ItemWater *>(*it)->removeFromArray();
                if((*it)) delete (*it);
                deleted=true;
            }
            else
            if(( objType=="Door_enter" )||( objType=="Door_exit" ))
            {
                if((lock_door)|| (dynamic_cast<ItemDoor *>(*it)->isLocked) ) continue;

                bool isEntrance = (objType=="Door_enter");
                LevelDoors doorData = dynamic_cast<ItemDoor *>(*it)->doorData;
                if(isEntrance){
                    doorData.isSetIn = true;
                    doorData.isSetOut = false;
                }else{
                    doorData.isSetIn = false;
                    doorData.isSetOut = true;
                }
                historyBuffer.doors.push_back(doorData);
                dynamic_cast<ItemDoor *>(*it)->removeFromArray();
                if((*it)) delete (*it);
                MainWinConnect::pMainWin->setDoorData(-2);
                deleted=true;
            }
            else
            if( objType=="playerPoint" )
            {
                 historyBuffer.players.push_back(dynamic_cast<ItemPlayerPoint *>(*it)->pointData);
                 dynamic_cast<ItemPlayerPoint *>(*it)->removeFromArray();
                 if((*it)) delete (*it);
                 deleted=true;
            }
    }

    if(deleted)
    {
        if(globalHistory)
        {
            overwritedItems.blocks << historyBuffer.blocks;
            overwritedItems.bgo << historyBuffer.bgo;
            overwritedItems.npc << historyBuffer.npc;
            overwritedItems.physez << historyBuffer.physez;
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
                          dynamic_cast<ItemBlock *>(items.first())->blockData,
                          FileFormats::dummyLvlBgo(),
                          FileFormats::dummyLvlNpc(), false);
        }
        else
        if(items.first()->data(0).toString()=="BGO")
        {
            MainWinConnect::pMainWin->LvlItemProps(1,
                              FileFormats::dummyLvlBlock(),
                              dynamic_cast<ItemBGO *>(items.first())->bgoData,
                              FileFormats::dummyLvlNpc(), false);
        }
        else
        if(items.first()->data(0).toString()=="NPC")
        {
            MainWinConnect::pMainWin->LvlItemProps(2,
                              FileFormats::dummyLvlBlock(),
                              FileFormats::dummyLvlBgo(),
                              dynamic_cast<ItemNPC *>(items.first())->npcData, false);
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
                dynamic_cast<ItemDoor *>(item)->doorData = LvlData->doors[i];
                dynamic_cast<ItemDoor *>(item)->removeFromArray();
                delete dynamic_cast<ItemDoor *>(item);
                doorEntranceSynced = true;
            }
            else
            {
                LvlData->doors[i].isSetIn=true;
                dynamic_cast<ItemDoor *>(item)->doorData = LvlData->doors[i];
                doorEntranceSynced = true;
            }
        }

        if((item->data(0).toString()=="Door_exit")&&(item->data(2).toInt()==arrayID))
        {
            if( (! (((!LvlData->doors[i].lvl_o) && (!LvlData->doors[i].lvl_i)) ||
                                      (LvlData->doors[i].lvl_i) ) )||(remove))
            {
                dynamic_cast<ItemDoor *>(item)->doorData = LvlData->doors[i];
                dynamic_cast<ItemDoor *>(item)->removeFromArray();
                delete dynamic_cast<ItemDoor *>(item);
                doorExitSynced = true;
            }
            else
            {
                LvlData->doors[i].isSetOut=true;
                dynamic_cast<ItemDoor *>(item)->doorData = LvlData->doors[i];
                doorExitSynced = true;
            }
        }
        if((doorEntranceSynced)&&(doorExitSynced)) break; // stop fetch, because door points was synced
    }


}



bool LvlScene::isInSection(long x, long y, int sectionIndex, int padding)
{
    return (x >= LvlData->sections[sectionIndex].size_left-padding) &&
        (x <= LvlData->sections[sectionIndex].size_right+padding) &&
        (y >= LvlData->sections[sectionIndex].size_top-padding) &&
             (y <= LvlData->sections[sectionIndex].size_bottom+padding);
}

bool LvlScene::isInSection(long x, long y, int width, int height, int sectionIndex, int padding)
{
    return isInSection(x, y, sectionIndex, padding) && isInSection(x+width, y+height, sectionIndex, padding);
}
