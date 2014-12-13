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

#include <QtMath>

#include <common_features/item_rectangles.h>
#include <common_features/mainwinconnect.h>
#include <common_features/grid.h>
#include <file_formats/file_formats.h>
#include <editing/edit_world/world_edit.h>

#include "wld_scene.h"
#include "item_tile.h"
#include "item_scene.h"
#include "item_path.h"
#include "item_level.h"
#include "item_music.h"
#include "wld_item_placing.h"

#include "../../../defines.h"

//QPoint sourcePos=QPoint(0,0);
//int gridSize=0, offsetX=0, offsetY=0;//, gridX, gridY, i=0;

//namespace wld_control
//{
//    static QPointF drawStartPos = QPoint(0,0);
//}

// //////////////////////////////////////////////EVENTS START/////////////////////////////////////////////////
void WldScene::keyPressEvent ( QKeyEvent * keyEvent )
{
    if(CurrentMode) CurrentMode->keyPress(keyEvent);
    QGraphicsScene::keyPressEvent(keyEvent);
}

void WldScene::keyReleaseEvent ( QKeyEvent * keyEvent )
{
    if(CurrentMode) CurrentMode->keyRelease(keyEvent);
    QGraphicsScene::keyReleaseEvent(keyEvent);
//    using namespace wld_control;
//    switch(keyEvent->key())
//    {
//    case (Qt::Key_Delete): //Delete action
//        removeSelectedWldItems();
//        break;
//    case (Qt::Key_Escape):
//        if(isSelectionDialog) break; //Disable this key in the point selection dialog

//        if(EditingMode != MODE_SetPoint)
//        {
//            if(!mouseMoved)
//                this->clearSelection();

//            resetResizers();
//        }
//        else
//        {   //Reset mode to selection allows only in "editing" mode from MDI interface
//            if(!isSelectionDialog) MainWinConnect::pMainWin->on_actionSelect_triggered();
//            return;
//        }
//        if(EditingMode == MODE_PlacingNew || EditingMode == MODE_DrawSquare || EditingMode == MODE_Line){
//            item_rectangles::clearArray();
//            MainWinConnect::pMainWin->on_actionSelect_triggered();
//            return;
//        }
//            //setSectionResizer(false, false);
//        break;
//    case (Qt::Key_Enter):
//    case (Qt::Key_Return):

//        if(isSelectionDialog) break; //Disable this key in the point selection dialog
//        if(EditingMode == MODE_SetPoint) return; // Disable this key in the pointSelection mode

//        applyResizers();
//            //setSectionResizer(false, true);
//        break;

//    default:
//        break;
//    }
//    QGraphicsScene::keyReleaseEvent(keyEvent);
}

void WldScene::selectionChanged()
{
    if(this->selectedItems().isEmpty())
    {
        if(EditingMode!=MODE_SetPoint)
            MainWinConnect::pMainWin->WldItemProps(-1, FileFormats::dummyWldLevel());
    }

    #ifdef _DEBUG_
    WriteToLog(QtDebugMsg, "Selection Changed!");
    #endif
}

void WldScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(MousePressEventOnly)
    {
        QGraphicsScene::mousePressEvent(mouseEvent);
        MousePressEventOnly = false;
        return;
    }

    //using namespace wld_control;
    #ifdef _DEBUG_
    WriteToLog(QtDebugMsg, QString("Mouse pressed -> [%1, %2] contextMenuOpened=%3, DrawMode=%4").arg(mouseEvent->scenePos().x()).arg(mouseEvent->scenePos().y())
               .arg(contextMenuOpened).arg(DrawMode));
    #endif

//if(contextMenuOpened) return;
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
    haveSelected=(!selectedItems().isEmpty());

//    switch(EditingMode)
//    {
//        case MODE_PlacingNew:
//        {
//            if( mouseEvent->buttons() & Qt::RightButton )
//            {
//                MainWinConnect::pMainWin->on_actionSelect_triggered();
//                return;
//            }

//            last_tile_arrayID=WldData->tile_array_id;
//            last_scene_arrayID=WldData->scene_array_id;
//            last_path_arrayID=WldData->path_array_id;
//            last_level_arrayID=WldData->level_array_id;
//            last_musicbox_arrayID=WldData->musicbox_array_id;

//            if(cursor){
//                cursor->setPos( QPointF(applyGrid( mouseEvent->scenePos().toPoint()-
//                                                   QPoint(WldPlacingItems::c_offset_x,
//                                                          WldPlacingItems::c_offset_y),
//                                                   WldPlacingItems::gridSz,
//                                                   WldPlacingItems::gridOffset)));
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
//                MainWinConnect::pMainWin->on_actionSelect_triggered();
//                return;
//            }

//            last_tile_arrayID=WldData->tile_array_id;
//            last_scene_arrayID=WldData->scene_array_id;
//            last_path_arrayID=WldData->path_array_id;
//            last_level_arrayID=WldData->level_array_id;
//            last_musicbox_arrayID=WldData->musicbox_array_id;

//            if(cursor){
//                drawStartPos = QPointF(applyGrid( mouseEvent->scenePos().toPoint(),
//                                                  WldPlacingItems::gridSz,
//                                                  WldPlacingItems::gridOffset));
//                cursor->setPos( drawStartPos );
//                cursor->setVisible(true);

//                QPoint hw = applyGrid( mouseEvent->scenePos().toPoint(),
//                                       WldPlacingItems::gridSz,
//                                       WldPlacingItems::gridOffset);

//                QSize hs = QSize( (long)fabs(drawStartPos.x() - hw.x()),  (long)fabs( drawStartPos.y() - hw.y() ) );
//                ((QGraphicsRectItem *)cursor)->setRect(0,0, hs.width(), hs.height());
//            }

//            QGraphicsScene::mousePressEvent(mouseEvent);
//            return;
//            break;

//        }
//        case MODE_Line:
//        {
//            if( mouseEvent->buttons() & Qt::RightButton )
//            {
//                MainWinConnect::pMainWin->on_actionSelect_triggered();
//                return;
//            }

//            WriteToLog(QtDebugMsg, QString("Line mode %1").arg(EditingMode));

//            last_tile_arrayID=WldData->tile_array_id;
//            last_scene_arrayID=WldData->scene_array_id;
//            last_path_arrayID=WldData->path_array_id;
//            last_level_arrayID=WldData->level_array_id;
//            last_musicbox_arrayID=WldData->musicbox_array_id;

//            if(cursor){
//                drawStartPos = QPointF(applyGrid( mouseEvent->scenePos().toPoint()-
//                                                  QPoint(WldPlacingItems::c_offset_x,
//                                                         WldPlacingItems::c_offset_y),
//                                                  WldPlacingItems::gridSz,
//                                                  WldPlacingItems::gridOffset));
//                //cursor->setPos( drawStartPos );
//                cursor->setVisible(true);

//                QPoint hw = applyGrid( mouseEvent->scenePos().toPoint()-
//                                       QPoint(WldPlacingItems::c_offset_x,
//                                              WldPlacingItems::c_offset_y),
//                                       WldPlacingItems::gridSz,
//                                       WldPlacingItems::gridOffset);
//                ((QGraphicsLineItem *)cursor)->setLine(drawStartPos.x(), drawStartPos.y(), hw.x(), hw.y());
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
//                Debugger_updateItemList();
//                EraserEnabled=true;
//            }
//            break;
//        }
//        case MODE_SetPoint:
//        {
//            if( (!isSelectionDialog) && (mouseEvent->buttons() & Qt::RightButton) )
//            {
//                MainWinConnect::pMainWin->on_actionSelect_triggered();
//                return;
//            }

//            if(cursor){
//                cursor->setPos( QPointF(applyGrid( mouseEvent->scenePos().toPoint()-
//                                                   QPoint(WldPlacingItems::c_offset_x,
//                                                          WldPlacingItems::c_offset_y),
//                                                   WldPlacingItems::gridSz,
//                                                   WldPlacingItems::gridOffset)));
//            }

//            setPoint(cursor->scenePos().toPoint());
//            emit pointSelected(selectedPoint);

//            QGraphicsScene::mousePressEvent(mouseEvent);
//            return;
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
//            if((disableMoveItems) && (mouseEvent->buttons() & Qt::LeftButton)
//                && (Qt::ControlModifier != QApplication::keyboardModifiers()))
//            { return; }

//            if( mouseEvent->buttons() & Qt::MiddleButton )
//            {
//                if(GlobalSettings::MidMouse_allowSwitchToPlace)
//                {
//                    if(selectedItems().size()==1)
//                    {
//                        QGraphicsItem * it = selectedItems().first();
//                        QString itp = it->data(0).toString();
//                        long itd = it->data(1).toInt();
//                        if(itp=="TILE")
//                        {MainWinConnect::pMainWin->SwitchPlacingItem(ItemTypes::WLD_Tile, itd); return;}
//                        else if(itp=="SCENERY")
//                        {MainWinConnect::pMainWin->SwitchPlacingItem(ItemTypes::WLD_Scenery, itd); return;}
//                        else if(itp=="PATH")
//                        {MainWinConnect::pMainWin->SwitchPlacingItem(ItemTypes::WLD_Path, itd); return;}
//                        else if(itp=="LEVEL")
//                        {MainWinConnect::pMainWin->SwitchPlacingItem(ItemTypes::WLD_Level, itd); return;}
//                        else if(itp=="MUSICBOX")
//                        {MainWinConnect::pMainWin->SwitchPlacingItem(ItemTypes::WLD_MusicBox, itd); return;}
//                    }
//                }
//                if(GlobalSettings::MidMouse_allowDuplicate)
//                {
//                    if(!selectedItems().isEmpty())
//                    {
//                        WldBuffer = copy();
//                        PasteFromBuffer=true;
//                    }
//                }
//            }


//            QGraphicsScene::mousePressEvent(mouseEvent);

//            break;
//        }
//    }
//    haveSelected=(!selectedItems().isEmpty());

}

void WldScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(MouseMoveEventOnly)
    {
        QGraphicsScene::mousePressEvent(mouseEvent);
        MouseMoveEventOnly = false;
        return;
    }

    //using namespace wld_control;

    MainWinConnect::pMainWin->Debugger_UpdateMousePosition(mouseEvent->scenePos().toPoint());

    //WriteToLog(QtDebugMsg, QString("Mouse moved -> [%1, %2]").arg(mouseEvent->scenePos().x()).arg(mouseEvent->scenePos().y()));
    //if(contextMenuOpened) return;
    contextMenuOpened=false;
    IsMoved=true;

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

//            if(mouseEvent->modifiers() & Qt::ControlModifier )
//                setMessageBoxItem(true, mouseEvent->scenePos(),
//                                       (cursor?
//                                            (
//                                       QString::number( cursor->scenePos().toPoint().x() ) + "x" +
//                                       QString::number( cursor->scenePos().toPoint().y() )
//                                            )
//                                                :""));
//            else
//                setMessageBoxItem(false);

//            if(cursor)
//            {
//                        cursor->setPos( QPointF(applyGrid( mouseEvent->scenePos().toPoint()-
//                                                           QPoint(WldPlacingItems::c_offset_x,
//                                                                  WldPlacingItems::c_offset_y),
//                                                         WldPlacingItems::gridSz,
//                                                         WldPlacingItems::gridOffset)));
//                       cursor->show();
//            }
//            if( mouseEvent->buttons() & Qt::LeftButton )
//            {
//                placeItemUnderCursor();
//                Debugger_updateItemList();
//            }
//            QGraphicsScene::mouseMoveEvent(mouseEvent);
//            break;
//        }
//    case MODE_SetPoint:
//    {
//        if(cursor)
//        {

//                    cursor->setPos( QPointF(applyGrid( mouseEvent->scenePos().toPoint()-
//                                                       QPoint(WldPlacingItems::c_offset_x,
//                                                              WldPlacingItems::c_offset_y),
//                                                     WldPlacingItems::gridSz,
//                                                     WldPlacingItems::gridOffset)));
//                   cursor->show();

//                   setMessageBoxItem(true, mouseEvent->scenePos(),
//                                          (cursor?
//                                               (
//                                          QString::number( cursor->scenePos().toPoint().x() ) + "x" +
//                                          QString::number( cursor->scenePos().toPoint().y() )
//                                               )
//                                                   :""));

//        }
//        QGraphicsScene::mouseMoveEvent(mouseEvent);
//        break;
//    }

//    case MODE_DrawSquare:
//        {
//            if(cursor)
//            {
//                if(cursor->isVisible())
//                {
//                QPoint hw = applyGrid( mouseEvent->scenePos().toPoint(),
//                                       WldPlacingItems::gridSz,
//                                       WldPlacingItems::gridOffset);

//                QSize hs = QSize( (long)fabs(drawStartPos.x() - hw.x()),  (long)fabs( drawStartPos.y() - hw.y() ) );


//                ((QGraphicsRectItem *)cursor)->setRect(0,0, hs.width(), hs.height());
//                ((QGraphicsRectItem *)cursor)->setPos(
//                            ((hw.x() < drawStartPos.x() )? hw.x() : drawStartPos.x()),
//                            ((hw.y() < drawStartPos.y() )? hw.y() : drawStartPos.y())
//                            );

//                item_rectangles::drawMatrix(this, QRect (((QGraphicsRectItem *)cursor)->x(),
//                                                        ((QGraphicsRectItem *)cursor)->y(),
//                                                        ((QGraphicsRectItem *)cursor)->rect().width(),
//                                                        ((QGraphicsRectItem *)cursor)->rect().height()),
//                                            QSize(WldPlacingItems::itemW, WldPlacingItems::itemH)
//                                            );

//                }
//            }
//            break;
//        }
//    case MODE_Line:
//        {
//            if(cursor)
//            {
//                if(cursor->isVisible())
//                {
//                QPoint hs = applyGrid( mouseEvent->scenePos().toPoint()-
//                                       QPoint(WldPlacingItems::c_offset_x,
//                                              WldPlacingItems::c_offset_y),
//                                       WldPlacingItems::gridSz,
//                                       WldPlacingItems::gridOffset);

//                //((QGraphicsLineItem *)cursor)->setLine(drawStartPos.x(),drawStartPos.y(), hw.x(), hw.y());

//                QLineF s = item_rectangles::snapLine(QLineF(drawStartPos.x(),drawStartPos.y(), (qreal)hs.x(), (qreal)hs.y()),
//                                                     QSizeF((qreal)WldPlacingItems::itemW, (qreal)WldPlacingItems::itemH) );

//                QPoint hw = applyGrid( s.p2().toPoint(),
//                                    WldPlacingItems::gridSz,
//                                    WldPlacingItems::gridOffset);

//                s.setP2(QPointF((qreal)hw.x(),(qreal)hw.y()));

//                ((QGraphicsLineItem *)cursor)->setLine(s);

//                item_rectangles::drawLine(this, s,
//                       QSize(WldPlacingItems::itemW, WldPlacingItems::itemH)
//                                            );

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
//        if(cursor) cursor->setPos(mouseEvent->scenePos());
//        break;
//    }

//    haveSelected=(!selectedItems().isEmpty());
//    if(haveSelected)
//    {
//        if(!( mouseEvent->buttons() & Qt::LeftButton )) return;
//        if(!mouseMoved)
//        {
//            mouseMoved=true;
//        }
//    }
//    QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void WldScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(MouseReleaseEventOnly)
    {
        QGraphicsScene::mousePressEvent(mouseEvent);
        MouseReleaseEventOnly = false;
        return;
    }

    //using namespace wld_control;

//    int multimouse=0;
//    if( mouseLeft || (mouseLeft^(mouseEvent->buttons() & Qt::LeftButton)) )
//        multimouse++;
//    if( mouseMid || (mouseMid^(mouseEvent->buttons() & Qt::MiddleButton)) )
//        multimouse++;
//    if( mouseRight || (mouseRight^(mouseEvent->buttons() & Qt::RightButton)) )
//        multimouse++;

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

//    if(multimouse>1)
//    {
//        WriteToLog(QtDebugMsg, QString("Multiple mouse keys detected %1").arg(multimouse) );
//        mouseEvent->accept(); return;
//    }

    if(contextMenuOpened)
    {
        contextMenuOpened = false; //bug protector
        QGraphicsScene::mouseReleaseEvent(mouseEvent);
        return;
    }

    contextMenuOpened=false;
    if(!isLeftMouse)
    {
        if(PasteFromBuffer && GlobalSettings::MidMouse_allowDuplicate && isMiddleMouse &&
                (EditingMode==MODE_Selecting||EditingMode==MODE_SelectingOnly))
        {
            clearSelection();
            paste( WldBuffer, mouseEvent->scenePos().toPoint() );
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
//            if( (((QGraphicsRectItem *)cursor)->rect().width()==0) ||
//              (((QGraphicsRectItem *)cursor)->rect().height()==0))
//            {
//                cursor->hide();
//                break;
//            }
//            QPointF p = ((QGraphicsRectItem *)cursor)->scenePos();
//            QSizeF s = ((QGraphicsRectItem *)cursor)->rect().size();

//            collisionCheckBuffer = this->items(QRectF(
//                        p.x()-10, p.y()-10,
//                        s.width()+20, s.height()+20),
//                        Qt::IntersectsItemBoundingRect);

//            if(collisionCheckBuffer.isEmpty())
//                emptyCollisionCheck = true;
//            else
//                prepareCollisionBuffer();

//            placeItemsByRectArray();

//            Debugger_updateItemList();

//            emptyCollisionCheck = false;
//            collisionCheckBuffer.clear();

//            WldData->modified = true;
//            cursor->hide();
//        }
//        break;
//        }
//    case MODE_Line:
//        {

//        if(cursor)
//        {
//            placeItemsByRectArray();
//            Debugger_updateItemList();
//            WldData->modified = true;
//            cursor->hide();
//        }
//        break;
//        }
//    case MODE_PlacingNew:
//        {
//            if(!overwritedItems.tiles.isEmpty()||
//                !overwritedItems.scenery.isEmpty()||
//                !overwritedItems.paths.isEmpty()||
//                !overwritedItems.levels.isEmpty()||
//                !overwritedItems.music.isEmpty() )
//            {
//                addOverwriteHistory(overwritedItems, placingItems);
//                overwritedItems.tiles.clear();
//                overwritedItems.scenery.clear();
//                overwritedItems.paths.clear();
//                overwritedItems.levels.clear();
//                overwritedItems.music.clear();
//                placingItems.tiles.clear();
//                placingItems.paths.clear();
//                placingItems.scenery.clear();
//                placingItems.levels.clear();
//                placingItems.music.clear();
//            }
//            else
//            if(!placingItems.tiles.isEmpty()||
//                    !placingItems.paths.isEmpty()||
//                    !placingItems.scenery.isEmpty()||
//                    !placingItems.levels.isEmpty()||
//                    !placingItems.music.isEmpty()){
//                addPlaceHistory(placingItems);
//                placingItems.tiles.clear();
//                placingItems.paths.clear();
//                placingItems.scenery.clear();
//                placingItems.levels.clear();
//                placingItems.music.clear();
//            }
//            break;
//        }
//    case MODE_SetPoint:
//        if(!isSelectionDialog)
//        {
//            MainWinConnect::pMainWin->WLD_returnPointToLevelProperties(selectedPoint);
//            MainWinConnect::pMainWin->on_actionSelect_triggered();
//            openProps();
//            QGraphicsScene::mouseReleaseEvent(mouseEvent);
//            return;
//        }
//        break;
//    case MODE_Erasing:
//        {
//            if(!overwritedItems.tiles.isEmpty()||
//                !overwritedItems.scenery.isEmpty()||
//                !overwritedItems.paths.isEmpty()||
//                !overwritedItems.levels.isEmpty()||
//                !overwritedItems.music.isEmpty() )
//            {
//                addRemoveHistory(overwritedItems);
//                overwritedItems.tiles.clear();
//                overwritedItems.scenery.clear();
//                overwritedItems.paths.clear();
//                overwritedItems.levels.clear();
//                overwritedItems.music.clear();
//            }
//            break;
//        }
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
//            int collisionPassed = false;

//            //History
//            WorldData historyBuffer; bool deleted=false;
//            WorldData historySourceBuffer;

//            if(PasteFromBuffer)
//            {
//                paste( WldBuffer, mouseEvent->scenePos().toPoint() );
//                Debugger_updateItemList();
//                PasteFromBuffer = false;
//                mouseMoved=false;
//                MainWinConnect::pMainWin->on_actionSelect_triggered();
//            }


//            QList<QGraphicsItem*> deleteList;
//            deleteList.clear();
//            QList<QGraphicsItem*> selectedList = selectedItems();

//            // check for grid snap
//            if ((!selectedList.isEmpty())&&(mouseMoved))
//            {

//                if(EditingMode==MODE_Erasing)
//                {
//                    removeWldItems(selectedList);
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
//                        WldData->modified=true;
//                    }
//                }

//                if((collisionPassed) || (!opts.collisionsEnabled))
//                for (QList<QGraphicsItem*>::iterator it = selectedList.begin(); it != selectedList.end(); it++)
//                { ////////////////////////SECOND FETCH///////////////////////
//                   ObjType = (*it)->data(0).toString();

//                   /////////////////////////GET DATA///////////////
//                    setItemSourceData((*it), (*it)->data(0).toString()); //Set Grid Size/Offset, sourcePosition
//                   /////////////////////////GET DATA/////////////////////

//                    //Check position
//                    if( (sourcePos == QPoint((long)((*it)->scenePos().x()), ((long)(*it)->scenePos().y()))))
//                    {
//                        mouseMoved=false;
//                        break; //break fetch when items is not moved
//                    }

//                    if( ObjType == "TILE")
//                    {
//                        //Applay move into main array
//                        historySourceBuffer.tiles.push_back(((ItemTile *)(*it))->tileData);
//                        ((ItemTile *)(*it))->tileData.x = (long)(*it)->scenePos().x();
//                        ((ItemTile *)(*it))->tileData.y = (long)(*it)->scenePos().y();
//                        ((ItemTile *)(*it))->arrayApply();
//                        historyBuffer.tiles.push_back(((ItemTile *)(*it))->tileData);
//                        WldData->modified = true;
//                    }
//                    else
//                    if( ObjType == "SCENERY")
//                    {
//                        //Applay move into main array
//                        historySourceBuffer.scenery.push_back(((ItemScene *)(*it))->sceneData);
//                        ((ItemScene *)(*it))->sceneData.x = (long)(*it)->scenePos().x();
//                        ((ItemScene *)(*it))->sceneData.y = (long)(*it)->scenePos().y();
//                        ((ItemScene *)(*it))->arrayApply();
//                        historyBuffer.scenery.push_back(((ItemScene *)(*it))->sceneData);
//                        WldData->modified = true;
//                    }
//                    else
//                    if( ObjType == "PATH")
//                    {
//                        //Applay move into main array
//                        historySourceBuffer.paths.push_back(((ItemPath*)(*it))->pathData);
//                        ((ItemPath *)(*it))->pathData.x = (long)(*it)->scenePos().x();
//                        ((ItemPath *)(*it))->pathData.y = (long)(*it)->scenePos().y();
//                        ((ItemPath *)(*it))->arrayApply();
//                        historyBuffer.paths.push_back(((ItemPath *)(*it))->pathData);
//                        WldData->modified = true;
//                    }
//                    else
//                    if( ObjType == "LEVEL")
//                    {
//                        //Applay move into main array
//                        historySourceBuffer.levels.push_back(((ItemLevel *)(*it))->levelData);
//                        ((ItemLevel *)(*it))->levelData.x = (long)(*it)->scenePos().x();
//                        ((ItemLevel *)(*it))->levelData.y = (long)(*it)->scenePos().y();
//                        ((ItemLevel *)(*it))->arrayApply();
//                        historyBuffer.levels.push_back(((ItemLevel *)(*it))->levelData);
//                        WldData->modified = true;
//                    }
//                    else
//                    if( ObjType == "MUSICBOX")
//                    {
//                        //Applay move into main array
//                        historySourceBuffer.music.push_back(((ItemMusic *)(*it))->musicData);
//                        ((ItemMusic *)(*it))->musicData.x = (long)(*it)->scenePos().x();
//                        ((ItemMusic *)(*it))->musicData.y = (long)(*it)->scenePos().y();
//                        ((ItemMusic *)(*it))->arrayApply();
//                        historyBuffer.music.push_back(((ItemMusic *)(*it))->musicData);
//                        WldData->modified = true;
//                    }

//                }
//                ////////////////////////SECOND FETCH///////////////////////

//                if((EditingMode==MODE_Selecting)&&(mouseMoved)) addMoveHistory(historySourceBuffer, historyBuffer);

//                mouseMoved = false;

//                QGraphicsScene::mouseReleaseEvent(mouseEvent);
//                return;
//           }
//     EraserEnabled = false;
//     QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

// //////////////////////////////////////////////EVENTS End/////////////////////////////////////////////////



//void WldScene::setItemSourceData(QGraphicsItem * it, QString ObjType)
//{
//    Q_UNUSED(it);
//    Q_UNUSED(ObjType);
//    gridSize = pConfigs->default_grid;
//    offsetX = 0;
//    offsetY = 0;

//    if( ObjType == "TILE")
//    {
//        sourcePos = QPoint(  ((ItemTile *)it)->tileData.x, ((ItemTile *)it)->tileData.y);
//        gridSize = ((ItemTile *)it)->gridSize;
//        offsetX = 0;
//        offsetY = 0;
//    }
//    else
//    if( ObjType == "SCENERY")
//    {
//        sourcePos = QPoint(  ((ItemScene *)it)->sceneData.x, ((ItemScene *)it)->sceneData.y);
//        gridSize = ((ItemScene *)it)->gridSize;
//        offsetX = 0;
//        offsetY = 0;
//    }
//    else
//    if( ObjType == "PATH")
//    {
//        sourcePos = QPoint(  ((ItemPath *)it)->pathData.x, ((ItemPath *)it)->pathData.y);
//        gridSize = ((ItemPath *)it)->gridSize;
//        offsetX = 0;
//        offsetY = 0;
//    }
//    else
//    if( ObjType == "LEVEL")
//    {
//        sourcePos = QPoint(  ((ItemLevel *)it)->levelData.x, ((ItemLevel*)it)->levelData.y);
//        gridSize = ((ItemLevel *)it)->gridSize;
//        offsetX = 0;
//        offsetY = 0;
//    }
//    else
//    if( ObjType == "MUSICBOX")
//    {
//        sourcePos = QPoint(  ((ItemMusic *)it)->musicData.x, ((ItemMusic*)it)->musicData.y);
//        gridSize = ((ItemMusic *)it)->gridSize;
//        offsetX = 0;
//        offsetY = 0;
//    }
//}

void WldScene::placeItemsByRectArray()
{
    //using namespace wld_control;
    //This function placing items by yellow rectangles
    if(item_rectangles::rectArray.isEmpty()) return;

    QGraphicsItem * backup = cursor;
    while(!item_rectangles::rectArray.isEmpty())
    {
        cursor = item_rectangles::rectArray.first();
        item_rectangles::rectArray.pop_front();

        foreach(dataFlag_w flag, WldPlacingItems::flags)
            cursor->setData(flag.first, flag.second);

        placeItemUnderCursor();

        if(cursor) delete cursor;
    }
    cursor = backup;
    cursor->hide();

    if(!overwritedItems.tiles.isEmpty()||
        !overwritedItems.scenery.isEmpty()||
        !overwritedItems.paths.isEmpty()||
        !overwritedItems.levels.isEmpty()||
        !overwritedItems.music.isEmpty() )
    {
        addOverwriteHistory(overwritedItems, placingItems);
        overwritedItems.tiles.clear();
        overwritedItems.scenery.clear();
        overwritedItems.paths.clear();
        overwritedItems.levels.clear();
        overwritedItems.music.clear();
        placingItems.tiles.clear();
        placingItems.paths.clear();
        placingItems.scenery.clear();
        placingItems.levels.clear();
        placingItems.music.clear();
    }
    else
    if(!placingItems.tiles.isEmpty()||
            !placingItems.paths.isEmpty()||
            !placingItems.scenery.isEmpty()||
            !placingItems.levels.isEmpty()||
            !placingItems.music.isEmpty()){
        addPlaceHistory(placingItems);
        placingItems.tiles.clear();
        placingItems.paths.clear();
        placingItems.scenery.clear();
        placingItems.levels.clear();
        placingItems.music.clear();
    }

}

void WldScene::placeItemUnderCursor()
{
    //using namespace wld_control;
    bool wasPlaced=false;

    if(WldPlacingItems::overwriteMode)
    {   //remove all colliaded items before placing
        QGraphicsItem * xxx;
        while( (xxx=itemCollidesWith(cursor)) != NULL )
        {
            bool removed=false;
            if(xxx->data(0).toString()=="TILE")
            {
                if(xxx->data(2).toLongLong()>last_tile_arrayID) break;
                overwritedItems.tiles.push_back( ((ItemTile *)xxx)->tileData );
                ((ItemTile *)xxx)->removeFromArray();
                delete xxx; removed=true;
            }
            else
            if(xxx->data(0).toString()=="SCENERY")
            {
                if(xxx->data(2).toLongLong()>last_scene_arrayID) break;
                overwritedItems.scenery.push_back( ((ItemScene *)xxx)->sceneData );
                ((ItemScene *)xxx)->removeFromArray();
                delete xxx; removed=true;
            }
            else
            if(xxx->data(0).toString()=="PATH")
            {
                if(xxx->data(2).toLongLong()>last_path_arrayID) break;
                overwritedItems.paths.push_back( ((ItemPath *)xxx)->pathData );
                ((ItemPath *)xxx)->removeFromArray();
                delete xxx; removed=true;
            }
            else
            if(xxx->data(0).toString()=="LEVEL")
            {
                if(xxx->data(2).toLongLong()>last_level_arrayID) break;
                overwritedItems.levels.push_back( ((ItemLevel *)xxx)->levelData );
                ((ItemLevel *)xxx)->removeFromArray();
                delete xxx; removed=true;
            }
            else
            if(xxx->data(0).toString()=="MUSICBOX")
            {
                if(xxx->data(2).toLongLong()>last_musicbox_arrayID) break;
                overwritedItems.music.push_back( ((ItemMusic *)xxx)->musicData );
                ((ItemMusic *)xxx)->removeFromArray();
                delete xxx; removed=true;
            }

            if(removed) //Remove pointer of deleted item
            {
                if(collisionCheckBuffer.contains(xxx))
                    collisionCheckBuffer.removeAt(collisionCheckBuffer.indexOf(xxx));
            }
        }
    }

    QList<QGraphicsItem *> * checkZone;
    if(collisionCheckBuffer.isEmpty())
        checkZone = NULL;
    else
        checkZone = &collisionCheckBuffer;

    if( !emptyCollisionCheck && itemCollidesWith(cursor, checkZone) )
    {
        return;
    }
    else
    {
        if(placingItem == PLC_Tile)
        {
            WldPlacingItems::TileSet.x = cursor->scenePos().x();
            WldPlacingItems::TileSet.y = cursor->scenePos().y();

            WldData->tile_array_id++;
            WldPlacingItems::TileSet.array_id = WldData->tile_array_id;

            WldData->tiles.push_back(WldPlacingItems::TileSet);
            placeTile(WldPlacingItems::TileSet, true);
            placingItems.tiles.push_back(WldPlacingItems::TileSet);
            wasPlaced=true;
        }
        else
        if(placingItem == PLC_Scene)
        {
            WldPlacingItems::SceneSet.x = cursor->scenePos().x();
            WldPlacingItems::SceneSet.y = cursor->scenePos().y();

            WldData->scene_array_id++;
            WldPlacingItems::SceneSet.array_id = WldData->scene_array_id;

            WldData->scenery.push_back(WldPlacingItems::SceneSet);
            placeScenery(WldPlacingItems::SceneSet, true);
            placingItems.scenery.push_back(WldPlacingItems::SceneSet);
            wasPlaced=true;
        }
        else
        if(placingItem == PLC_Path)
        {
            WldPlacingItems::PathSet.x = cursor->scenePos().x();
            WldPlacingItems::PathSet.y = cursor->scenePos().y();

            WldData->path_array_id++;
            WldPlacingItems::PathSet.array_id = WldData->path_array_id;

            WldData->paths.push_back(WldPlacingItems::PathSet);
            placePath(WldPlacingItems::PathSet, true);
            placingItems.paths.push_back(WldPlacingItems::PathSet);
            wasPlaced=true;
        }
        else
        if(placingItem == PLC_Level)
        {
            WldPlacingItems::LevelSet.x = cursor->scenePos().x();
            WldPlacingItems::LevelSet.y = cursor->scenePos().y();

            WldData->level_array_id++;
            WldPlacingItems::LevelSet.array_id = WldData->level_array_id;

            WldData->levels.push_back(WldPlacingItems::LevelSet);
            placeLevel(WldPlacingItems::LevelSet, true);
            placingItems.levels.push_back(WldPlacingItems::LevelSet);
            wasPlaced=true;
        }
        else
        if(placingItem == PLC_Musicbox)
        {
            WldPlacingItems::MusicSet.x = cursor->scenePos().x();
            WldPlacingItems::MusicSet.y = cursor->scenePos().y();

            WldData->musicbox_array_id++;
            WldPlacingItems::MusicSet.array_id = WldData->musicbox_array_id;

            WldData->music.push_back(WldPlacingItems::MusicSet);
            placeMusicbox(WldPlacingItems::MusicSet, true);
            placingItems.music.push_back(WldPlacingItems::MusicSet);
            wasPlaced=true;
        }

    }
    if(wasPlaced)
    {
        WldData->modified = true;
    }

    //if(opts.animationEnabled) stopAnimation();
    //if(opts.animationEnabled) startBlockAnimation();
}

void WldScene::removeItemUnderCursor()
{
    if(contextMenuOpened) return;

    QGraphicsItem * findItem;
    findItem = itemCollidesCursor(cursor);
    removeWldItem(findItem, true);
}


void WldScene::removeSelectedWldItems()
{
    QList<QGraphicsItem*> selectedList = selectedItems();
    if(selectedList.isEmpty()) return;
    removeWldItems(selectedList);
    Debugger_updateItemList();
}

void WldScene::removeWldItem(QGraphicsItem * item, bool globalHistory)
{
    if(!item) return;
    QList<QGraphicsItem * > items;
    items.push_back(item);
    removeWldItems(items, globalHistory);
}

void WldScene::removeWldItems(QList<QGraphicsItem * > items, bool globalHistory)
{
    WorldData historyBuffer;
    bool deleted=false;
    QString objType;

    for (QList<QGraphicsItem*>::iterator it = items.begin(); it != items.end(); it++)
    {
            objType=(*it)->data(0).toString();

            if(!(*it)->isVisible()) continue;  //Invisible items can't be deleted

            //remove data from main array before deletion item from scene
            if( objType=="TILE" )
            {
                historyBuffer.tiles.push_back(((ItemTile *)(*it))->tileData);
                ((ItemTile *)(*it))->removeFromArray();
                if((*it)) delete (*it);
                deleted=true;
            }
            else
            if( objType=="SCENERY" )
            {
                historyBuffer.scenery.push_back(((ItemScene*)(*it))->sceneData);
                ((ItemScene *)(*it))->removeFromArray();
                if((*it)) delete (*it);
                deleted=true;
            }
            else
            if( objType=="PATH" )
            {
                historyBuffer.paths.push_back(((ItemPath*)(*it))->pathData);
                ((ItemPath *)(*it))->removeFromArray();
                if((*it)) delete (*it);
                deleted=true;
            }
            else
            if( objType=="LEVEL" )
            {
                historyBuffer.levels.push_back(((ItemLevel*)(*it))->levelData);
                ((ItemLevel *)(*it))->removeFromArray();
                if((*it)) delete (*it);
                deleted=true;
            }
            else
            if( objType=="MUSICBOX" )
            {
                historyBuffer.music.push_back(((ItemMusic*)(*it))->musicData);
                ((ItemMusic *)(*it))->removeFromArray();
                if((*it)) delete (*it);
                deleted=true;
            }

    }

    if(deleted)
    {
        if(globalHistory)
        {
            overwritedItems.tiles << historyBuffer.tiles;
            overwritedItems.scenery << historyBuffer.scenery;
            overwritedItems.paths << historyBuffer.paths;
            overwritedItems.levels << historyBuffer.levels;
            overwritedItems.music << historyBuffer.music;
        }
        else
            addRemoveHistory(historyBuffer);
    }
}




// /////////////////////////////Open properties window of selected item////////////////////////////////
void WldScene::openProps()
{
    QList<QGraphicsItem * > items = this->selectedItems();
    if(!items.isEmpty())
    {
        if(items.first()->data(0).toString()=="LEVEL")
        {
            MainWinConnect::pMainWin->WldItemProps(0,
                          ((ItemLevel *)items.first())->levelData,
                          false);
        }
        else
        MainWinConnect::pMainWin->WldItemProps(-1,
                                               FileFormats::dummyWldLevel(),
                                               false);
    }
    else
    {
        MainWinConnect::pMainWin->WldItemProps(-1,
                                               FileFormats::dummyWldLevel(),
                                               false );
    }

    QGraphicsScene::selectionChanged();
}
