/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <common_features/main_window_ptr.h>
#include <common_features/grid.h>
#include <PGE_File_Formats/file_formats.h>
#include <editing/edit_world/world_edit.h>
#include <main_window/dock/wld_item_props.h>
#include <main_window/dock/debugger.h>

#include "wld_scene.h"
#include "items/item_tile.h"
#include "items/item_scene.h"
#include "items/item_path.h"
#include "items/item_level.h"
#include "items/item_music.h"
#include "wld_item_placing.h"

#include "../../../defines.h"


// //////////////////////////////////////////////EVENTS START/////////////////////////////////////////////////
void WldScene::keyPressEvent(QKeyEvent *keyEvent)
{
    if(m_editModeObj) m_editModeObj->keyPress(keyEvent);
    QGraphicsScene::keyPressEvent(keyEvent);
}

void WldScene::keyReleaseEvent(QKeyEvent *keyEvent)
{
    if(m_editModeObj) m_editModeObj->keyRelease(keyEvent);
    QGraphicsScene::keyReleaseEvent(keyEvent);
}

void WldScene::selectionChanged()
{
#ifdef _DEBUG_
    LogDebug("Selection Changed!");
#endif
}

void WldScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(m_skipChildMousePressEvent)
    {
        QGraphicsScene::mousePressEvent(mouseEvent);
        m_skipChildMousePressEvent = false;
        return;
    }

    //using namespace wld_control;
#ifdef _DEBUG_
    LogDebug(QString("Mouse pressed -> [%1, %2] contextMenuOpened=%3, DrawMode=%4").arg(mouseEvent->scenePos().x()).arg(mouseEvent->scenePos().y())
             .arg(contextMenuOpened).arg(DrawMode));
#endif

    //if(contextMenuOpened) return;
    m_contextMenuIsOpened = false;

    m_mouseIsMovedAfterKey = false;

    //Discard multi mouse key
    int mSum = 0;
    if(mouseEvent->buttons() & Qt::LeftButton) mSum++;
    if(mouseEvent->buttons() & Qt::MiddleButton) mSum++;
    if(mouseEvent->buttons() & Qt::RightButton) mSum++;
    if(mSum > 1)
    {
        mouseEvent->accept();
        LogDebug(QString("[MousePress] MultiMouse detected [%2] [edit mode: %1]").arg(m_editMode).arg(QString::number(mSum, 2)));
        return;
    }

    m_mouseIsMoved = false;
    if(mouseEvent->buttons() & Qt::LeftButton)
    {
        m_mouseLeftPressed = true;
        LogDebug(QString("Left mouse button pressed [edit mode: %1]").arg(m_editMode));
    }
    else
        m_mouseLeftPressed = false;

    if(mouseEvent->buttons() & Qt::MiddleButton)
    {
        m_mouseMidPressed = true;
        LogDebug(QString("Middle mouse button pressed [edit mode: %1]").arg(m_editMode));
    }
    else m_mouseMidPressed = false;

    if(mouseEvent->buttons() & Qt::RightButton)
    {
        m_mouseRightPressed = true;
        LogDebug(QString("Right mouse button pressed [edit mode: %1]").arg(m_editMode));
    }
    else m_mouseRightPressed = false;

    LogDebug(QString("Current editing mode %1").arg(m_editMode));

    if(m_editModeObj)
    {
        m_editModeObj->mousePress(mouseEvent);
        if(m_editModeObj->noEvent())
            return;
    }

    if((m_disableMoveItems) && (mouseEvent->buttons() & Qt::LeftButton)
       && (Qt::ControlModifier != QApplication::keyboardModifiers()))
        return;

    if((m_editMode == MODE_Selecting) || (m_editMode == MODE_SelectingOnly))
        MainWinConnect::pMainWin->dock_WldItemProps->hideToolbox();

    QGraphicsScene::mousePressEvent(mouseEvent);
    //haveSelected=(!selectedItems().isEmpty());
}

void WldScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    mousePressEvent(mouseEvent);
}

void WldScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(m_skipChildMouseMoveEvent)
    {
        QGraphicsScene::mouseMoveEvent(mouseEvent);
        m_skipChildMouseMoveEvent = false;
        return;
    }

    MainWinConnect::pMainWin->dock_DebuggerBox->setMousePos(mouseEvent->scenePos().toPoint());

#ifdef _DEBUG_
    LogDebug(QString("Mouse moved -> [%1, %2]").arg(mouseEvent->scenePos().x()).arg(mouseEvent->scenePos().y()));
#endif

    m_contextMenuIsOpened = false;
    m_mouseIsMovedAfterKey = true;

    if(m_editModeObj)
    {
        m_editModeObj->mouseMove(mouseEvent);
        if(m_editModeObj->noEvent())
            return;
    }

    bool haveSelected = (!selectedItems().isEmpty());
    if(haveSelected)
    {
        if(!(mouseEvent->buttons() & Qt::LeftButton)) return;
        if(!m_mouseIsMoved)
            m_mouseIsMoved = true;
    }
    QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void WldScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(m_skipChildMousReleaseEvent)
    {
        QGraphicsScene::mouseReleaseEvent(mouseEvent);
        m_skipChildMousReleaseEvent = false;
        return;
    }

    bool isLeftMouse = false;
    bool isMiddleMouse = false;

    if(mouseEvent->button() == Qt::LeftButton)
    {
        m_mouseLeftPressed = false;
        isLeftMouse = true;
        LogDebug(QString("Left mouse button released [edit mode: %1]").arg(m_editMode));
    }
    if(mouseEvent->button() == Qt::MiddleButton)
    {
        m_mouseMidPressed = false;
        isMiddleMouse = true;
        LogDebug(QString("Middle mouse button released [edit mode: %1]").arg(m_editMode));
    }
    if(mouseEvent->button() == Qt::RightButton)
    {
        m_mouseRightPressed = false;
        LogDebug(QString("Right mouse button released [edit mode: %1]").arg(m_editMode));
    }

    if(m_contextMenuIsOpened)
    {
        m_contextMenuIsOpened = false; //bug protector
        QGraphicsScene::mouseReleaseEvent(mouseEvent);
        return;
    }

    m_contextMenuIsOpened = false;
    if(!isLeftMouse)
    {
        if(m_pastingMode && GlobalSettings::MidMouse_allowDuplicate && isMiddleMouse &&
           (m_editMode == MODE_Selecting || m_editMode == MODE_SelectingOnly))
        {
            clearSelection();
            paste(WldBuffer, mouseEvent->scenePos().toPoint());
            Debugger_updateItemList();
            m_pastingMode = false;
        }

        if(GlobalSettings::MidMouse_allowSwitchToDrag && isMiddleMouse &&
           (m_editMode == MODE_Selecting || m_editMode == MODE_SelectingOnly) && selectedItems().isEmpty())
            MainWinConnect::pMainWin->on_actionHandScroll_triggered();

        QGraphicsScene::mouseReleaseEvent(mouseEvent);
        return;
    }

    if(m_editModeObj)
    {
        m_editModeObj->mouseRelease(mouseEvent);
        if(!m_editModeObj->noEvent())
            QGraphicsScene::mouseReleaseEvent(mouseEvent);
    }
}

// //////////////////////////////////////////////EVENTS End/////////////////////////////////////////////////







void WldScene::Debugger_updateItemList()
{
    QString itemList =
        tr("Tiles:\t\t%1\n"
           "Sceneries:\t\t\t%2\n"
           "Paths:\t%3\n"
           "Levels:\t%4\n"
           "Music boxes:\t\t%5\n");

    itemList = itemList.arg(
                   m_data->tiles.size())
               .arg(m_data->scenery.size())
               .arg(m_data->paths.size())
               .arg(m_data->levels.size())
               .arg(m_data->music.size());

    MainWinConnect::pMainWin->dock_DebuggerBox->setItemStat(itemList);
}

// /////////////////////////////Open properties window of selected item////////////////////////////////
void WldScene::openProps()
{
    QList<QGraphicsItem * > items = this->selectedItems();
    if(!items.isEmpty())
    {
        if(items.first()->data(ITEM_TYPE).toString() == "LEVEL")
        {
            MainWinConnect::pMainWin->dock_WldItemProps->openPropertiesFor(0,
                    ((ItemLevel *)items.first())->m_data,
                    false);
        }
        else
            MainWinConnect::pMainWin->dock_WldItemProps->hideToolbox();
    }
    else
        MainWinConnect::pMainWin->dock_WldItemProps->hideToolbox();

    QGraphicsScene::selectionChanged();
}

QPoint WldScene::getViewportPos()
{
    QPoint vpPos(0, 0);
    if(m_viewPort)
    {
        vpPos.setX(this->m_viewPort->horizontalScrollBar()->value());
        vpPos.setY(this->m_viewPort->verticalScrollBar()->value());
    }
    return vpPos;
}

QRect WldScene::getViewportRect()
{
    QRect vpRect(0, 0, 0, 0);
    if(m_viewPort)
    {
        vpRect.setLeft(this->m_viewPort->horizontalScrollBar()->value());
        vpRect.setTop(this->m_viewPort->verticalScrollBar()->value());
        vpRect.setWidth(m_viewPort->viewport()->width());
        vpRect.setHeight(m_viewPort->viewport()->height());
    }
    return vpRect;
}
