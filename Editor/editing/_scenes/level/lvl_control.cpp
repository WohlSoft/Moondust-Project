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

#include <mainwindow.h>
#include <main_window/dock/lvl_item_properties.h>
#include <main_window/dock/lvl_sctc_props.h>
#include <main_window/dock/debugger.h>
#include <editing/edit_level/level_edit.h>
#include <PGE_File_Formats/file_formats.h>

#include "../../../defines.h"

#include "lvl_scene.h"
#include "items/item_block.h"
#include "items/item_bgo.h"
#include "items/item_npc.h"
#include "items/item_water.h"
#include "items/item_door.h"
#include "items/item_playerpoint.h"


// //////////////////////////////////////////////EVENTS START/////////////////////////////////////////////////
void LvlScene::keyPressEvent(QKeyEvent *keyEvent)
{
    if(m_editModeObj) m_editModeObj->keyPress(keyEvent);
    QGraphicsScene::keyPressEvent(keyEvent);
}

void LvlScene::keyReleaseEvent(QKeyEvent *keyEvent)
{
    if(m_editModeObj) m_editModeObj->keyRelease(keyEvent);
    QGraphicsScene::keyReleaseEvent(keyEvent);
}


// /////////////////////////////Selection was changes////////////////////////////////
void LvlScene::selectionChanged()
{
#ifdef _DEBUG_
    LogDebug("Selection Changed!");
#endif
}

void LvlScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(m_skipChildMousePressEvent)
    {
        QGraphicsScene::mousePressEvent(mouseEvent);
        m_skipChildMousePressEvent = false;
        return;
    }

#ifdef _DEBUG_
    LogDebug(QString("Mouse pressed -> [%1, %2] contextMenuOpened=%3, DrawMode=%4").arg(mouseEvent->scenePos().x()).arg(mouseEvent->scenePos().y())
             .arg(contextMenuOpened).arg(DrawMode));
#endif

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

    //Close properties box
    if((m_editMode == MODE_Selecting) || (m_editMode == MODE_SelectingOnly))
        m_mw->dock_LvlItemProps->closeProps();

    QGraphicsScene::mousePressEvent(mouseEvent);

    //#ifdef _DEBUG_
    LogDebug(QString("mousePress -> done %1").arg(mouseEvent->isAccepted()));
    //#endif
    //haveSelected=(!selectedItems().isEmpty());
}

void LvlScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    mousePressEvent(mouseEvent);
}


void LvlScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(m_skipChildMouseMoveEvent)
    {
        QGraphicsScene::mouseMoveEvent(mouseEvent);
        m_skipChildMouseMoveEvent = false;
        return;
    }

    m_mw->dock_DebuggerBox->setMousePos(mouseEvent->scenePos().toPoint());

#ifdef _DEBUG_
    LogDebug(QString("Mouse moved -> [%1, %2]").arg(mouseEvent->scenePos().x()).arg(mouseEvent->scenePos().y()));
#endif
    //if(contextMenuOpened) return;
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



void LvlScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(m_skipChildMousReleaseEvent)
    {
        QGraphicsScene::mouseReleaseEvent(mouseEvent);
        m_skipChildMousReleaseEvent = false;
        return;
    }

    bool isLeftMouse = false;
    bool isMiddleMouse = false;
    bool isRightMouse = false;

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
        isRightMouse = true;
        LogDebug(QString("Right mouse button released [edit mode: %1]").arg(m_editMode));
    }

    m_contextMenuIsOpened = false;
    if(!isLeftMouse)
    {
        if(m_pastingMode && GlobalSettings::MidMouse_allowDuplicate && isMiddleMouse &&
           (m_editMode == MODE_Selecting || m_editMode == MODE_SelectingOnly))
        {
            clearSelection();
            paste(m_dataBuffer, mouseEvent->scenePos().toPoint());
            Debugger_updateItemList();
            m_pastingMode = false;
        }

        if(GlobalSettings::MidMouse_allowSwitchToDrag && isMiddleMouse &&
           (m_editMode == MODE_Selecting || m_editMode == MODE_SelectingOnly) && selectedItems().isEmpty())
            m_mw->on_actionHandScroll_triggered();

        QGraphicsScene::mouseReleaseEvent(mouseEvent);

        if(isRightMouse && !m_mouseIsMovedAfterKey && (!mouseEvent->isAccepted()))
        {
            QMenu lmenu;
            //QMenu* jumptoSide = lmenu.addMenu(tr("LEVELSCENE_CONTEXTMENU_JUMP_TO_SIDE", "Jump to side"));
            //QMenu* jumptoSection = lmenu.addMenu(tr("LEVELSCENE_CONTEXTMENU_JUMP_TO_SECTION", "Jump to section"));
            QAction *props = lmenu.addAction(tr("LEVELSCENE_CONTEXTMENU_SectionProperties...", "Section properties..."));
            lmenu.addMenu(m_mw->getLevelCurrSectionMenu());
            lmenu.addMenu(m_mw->getLevelModSectionMenu());
            lmenu.addSeparator();
            lmenu.addMenu(m_mw->getViewMenu());
            QAction *lvlprops = lmenu.addAction(tr("LEVELSCENE_CONTEXTMENU_LevelProperties...", "Level properties..."));

            QAction *answer = lmenu.exec(mouseEvent->screenPos());
            if(answer != nullptr)
            {
                if(answer == props)
                {
                    m_mw->dock_LvlSectionProps->show();
                    m_mw->dock_LvlSectionProps->raise();
                }
                else if(answer == lvlprops)
                    m_mw->on_actionLevelProp_triggered();
            }
        }
        return;
    }

    if(m_editModeObj)
    {
        m_editModeObj->mouseRelease(mouseEvent);
        if(!m_editModeObj->noEvent())
            QGraphicsScene::mouseReleaseEvent(mouseEvent);
    }
}

// //////////////////////////////////////////////EVENTS END/////////////////////////////////////////////////




void LvlScene::Debugger_updateItemList()
{
    QString itemList =
        tr("Player start points:\t\t%1\n"
           "Blocks:\t\t\t%2\n"
           "Background objects's:\t%3\n"
           "Non-playable characters's:\t%4\n"
           "Warp entries:\t\t%5\n"
           "Physical env. zones:\t%6\n");

    itemList = itemList.arg(m_data->players.size())
               .arg(m_data->blocks.size())
               .arg(m_data->bgo.size())
               .arg(m_data->npc.size())
               .arg(m_data->doors.size())
               .arg(m_data->physez.size());

    m_mw->dock_DebuggerBox->setItemStat(itemList);
}



// /////////////////////////////Open properties window of selected item////////////////////////////////
void LvlScene::openProps()
{
    QList<QGraphicsItem * > items = this->selectedItems();
    if(!items.isEmpty())
    {
        if(items.first()->data(ITEM_TYPE).toString() == "Block")
            m_mw->dock_LvlItemProps->openBlockProps(dynamic_cast<ItemBlock *>(items.first())->m_data);
        else if(items.first()->data(ITEM_TYPE).toString() == "BGO")
            m_mw->dock_LvlItemProps->openBgoProps(dynamic_cast<ItemBGO *>(items.first())->m_data);
        else if(items.first()->data(ITEM_TYPE).toString() == "NPC")
            m_mw->dock_LvlItemProps->openNpcProps(dynamic_cast<ItemNPC *>(items.first())->m_data);
        else
            m_mw->dock_LvlItemProps->closeProps();
    }
    else
        m_mw->dock_LvlItemProps->closeProps();

    QGraphicsScene::selectionChanged();
}



QPoint LvlScene::getViewportPos()
{
    QPoint vpPos(0, 0);
    if(m_viewPort)
    {
        vpPos.setX(this->m_viewPort->horizontalScrollBar()->value());
        vpPos.setY(this->m_viewPort->verticalScrollBar()->value());
    }
    return vpPos;
}

QRect LvlScene::getViewportRect()
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
