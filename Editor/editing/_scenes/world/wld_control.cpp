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

#include <common_features/mainwinconnect.h>
#include <common_features/grid.h>
#include <file_formats/file_formats.h>
#include <editing/edit_world/world_edit.h>

#include "wld_scene.h"
#include "items/item_tile.h"
#include "items/item_scene.h"
#include "items/item_path.h"
#include "items/item_level.h"
#include "items/item_music.h"
#include "wld_item_placing.h"

#include "../../../defines.h"


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
}

void WldScene::selectionChanged()
{
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

    if( ((EditingMode==MODE_Selecting) || (EditingMode==MODE_SelectingOnly)) && (EditingMode!=MODE_SetPoint))
        MainWinConnect::pMainWin->WldItemProps_hide();

    QGraphicsScene::mousePressEvent(mouseEvent);
    haveSelected=(!selectedItems().isEmpty());
}

void WldScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
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
}

void WldScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
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
}

// //////////////////////////////////////////////EVENTS End/////////////////////////////////////////////////







void WldScene::Debugger_updateItemList()
{
    QString itemList=
            tr("Tiles:\t\t%1\n"
               "Sceneries:\t\t\t%2\n"
               "Paths:\t%3\n"
               "Levels:\t%4\n"
               "Music boxes:\t\t%5\n");

    itemList = itemList.arg(
                WldData->tiles.size())
            .arg(WldData->scenery.size())
            .arg(WldData->paths.size())
            .arg(WldData->levels.size())
            .arg(WldData->music.size());

    MainWinConnect::pMainWin->Debugger_UpdateItemList(itemList);
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
        MainWinConnect::pMainWin->WldItemProps_hide();
    }
    else
    {
        MainWinConnect::pMainWin->WldItemProps_hide();
    }

    QGraphicsScene::selectionChanged();
}
