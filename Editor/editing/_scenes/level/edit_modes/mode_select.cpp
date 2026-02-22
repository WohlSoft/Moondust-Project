/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2026 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <common_features/themes.h>
#include <common_features/item_rectangles.h>

#include <main_window/dock/lvl_warp_props.h>

#include "mode_select.h"
#include "../lvl_history_manager.h"
#include "../lvl_item_placing.h"

LVL_ModeSelect::LVL_ModeSelect(QGraphicsScene *parentScene, QObject *parent)
    : EditMode("Select", parentScene, parent)
{
    sourcePos = QPoint(0, 0);
    gridSize = 0;
    offsetX = 0;
    offsetY = 0; //, gridX, gridY, i=0;
}

LVL_ModeSelect::~LVL_ModeSelect()
{}

void LVL_ModeSelect::set()
{
    set(MoondustBaseScene::MODE_Selecting);
}

void LVL_ModeSelect::set(int editMode)
{
    if(!scene)
        return;

    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    s->resetCursor();
    s->resetResizers();

    s->m_eraserIsEnabled = false;
    s->m_pastingMode = false;
    s->m_busyMode = false;
    s->m_disableMoveItems = false;

    auto *vp = s->curViewPort();

    if(editMode == MoondustBaseScene::MODE_PasteFromClip)
    {
        s->clearSelection();
        vp->setInteractive(true);
        vp->setCursor(Themes::Cursor(Themes::cursor_pasting));
        vp->setDragMode(QGraphicsView::NoDrag);
        s->m_disableMoveItems = true;
    }
    else
    {
        vp->setInteractive(true);
        vp->setCursor(Themes::Cursor(Themes::cursor_normal));
        vp->setDragMode(QGraphicsView::RubberBandDrag);
    }

    if(editMode == MoondustBaseScene::MODE_SelectingOnly)
        s->m_disableMoveItems = true;
}


void LVL_ModeSelect::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    if(s->editMode() == MoondustBaseScene::MODE_PasteFromClip)
    {
        if(mouseEvent->buttons() & Qt::RightButton)
        {
            s->mw()->on_actionSelect_triggered();
            dontCallEvent = true;
            s->m_mouseIsMovedAfterKey = true;
            return;
        }

        s->m_pastingMode = true;
        dontCallEvent = true;
        s->m_mouseIsMovedAfterKey = true;
        return;
    }

    if((s->m_disableMoveItems) && (mouseEvent->buttons() & Qt::LeftButton)
       && (Qt::ControlModifier != QApplication::keyboardModifiers()))
    {
        dontCallEvent = true;
        return;
    }

    if(mouseEvent->buttons() & Qt::MiddleButton)
    {
        if(GlobalSettings::MidMouse_allowSwitchToPlace)
        {
            if(s->selectedItems().size() == 1)
            {
                QGraphicsItem *it = s->selectedItems().first();
                int itp = it->data(LvlScene::ITEM_TYPE_INT).toInt();
                unsigned long itd = (unsigned long)it->data(LvlScene::ITEM_ID).toULongLong();

                switch(itp)
                {
                case ItemTypes::LVL_Block:
                {
                    ItemBlock *blk = qgraphicsitem_cast<ItemBlock *>(it);
                    if(blk) LvlPlacingItems::blockSet = blk->m_data;
                    s->mw()->SwitchPlacingItem(ItemTypes::LVL_Block, itd, true);
                    return;
                }

                case ItemTypes::LVL_BGO:
                {
                    ItemBGO *blk = qgraphicsitem_cast<ItemBGO *>(it);
                    if(blk) LvlPlacingItems::bgoSet = blk->m_data;
                    s->mw()->SwitchPlacingItem(ItemTypes::LVL_BGO, itd, true);
                    return;
                }

                case ItemTypes::LVL_NPC:
                {
                    ItemNPC *blk = qgraphicsitem_cast<ItemNPC *>(it);
                    if(blk) LvlPlacingItems::npcSet = blk->m_data;
                    s->mw()->SwitchPlacingItem(ItemTypes::LVL_NPC, itd, true);
                    return;
                }
                }
            }
        }

        if(GlobalSettings::MidMouse_allowDuplicate)
        {
            if(!s->selectedItems().isEmpty())
            {
                s->m_dataBuffer = s->copy();
                s->m_pastingMode = true;
            }
        }
    }
}


void LVL_ModeSelect::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    if(!(mouseEvent->buttons() & Qt::LeftButton)) return;
    if(s->m_cursorItemImg) s->m_cursorItemImg->setPos(mouseEvent->scenePos());
}

void LVL_ModeSelect::mouseRelease(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    if(mouseEvent->button() != Qt::LeftButton)
    {
        dontCallEvent = true;
        return;
    }

    s->m_cursorItemImg->hide();

    //s->haveSelected = false;

    int ObjType;
    bool collisionPassed = false;

    //History
    LevelData historyBuffer;
    LevelData historySourceBuffer;

    if(s->m_pastingMode)
    {
        s->paste(s->m_dataBuffer, mouseEvent->scenePos().toPoint());
        s->m_pastingMode = false;
        s->m_mouseIsMovedAfterKey = false;
        s->mw()->on_actionSelect_triggered();
        s->Debugger_updateItemList();
    }

    QList<QGraphicsItem *> selectedList = s->selectedItems();

    // check for grid snap
    if((!selectedList.isEmpty()) && (s->m_mouseIsMoved))
    {
        //Set Grid Size/Offset, sourcePosition
        setItemSourceData(selectedList.first(), selectedList.first()->data(LvlScene::ITEM_TYPE_INT).toInt());
        //Check first selected element is it was moved
        if((sourcePos == QPoint(
                (int)(selectedList.first()->scenePos().x()),
                (int)(selectedList.first()->scenePos().y())
            )))
        {
            s->m_mouseIsMoved = false;
            return; //break fetch when items is not moved
        }

        s->applyGroupGrid(selectedList);

        // Check collisions
        //Only if collision ckecking enabled
        if(!s->m_pastingMode)
        {
            if(s->m_opts.collisionsEnabled && s->checkGroupCollisions(selectedList))
            {
                collisionPassed = false;
                s->returnItemBackGroup(selectedList);
            }
            else
            {
                collisionPassed = true;
                //applyArrayForItemGroup(selectedList);
                s->m_data->meta.modified = true;
            }
        }


        if(collisionPassed || !s->m_opts.collisionsEnabled)
        {
            for(QList<QGraphicsItem *>::iterator it = selectedList.begin();
                it != selectedList.end(); it++)
            {
                ////////////////////////SECOND FETCH///////////////////////
                ObjType = (*it)->data(LvlScene::ITEM_TYPE_INT).toInt();

                /////////////////////////GET DATA///////////////
                setItemSourceData((*it), ObjType); //Set Grid Size/Offset, sourcePosition
                /////////////////////////GET DATA/////////////////////

                //Check position
                if((sourcePos == QPoint((long)((*it)->scenePos().x()), ((long)(*it)->scenePos().y()))))
                {
                    s->m_mouseIsMoved = false;
                    break; //break fetch when items is not moved
                }

                if(ObjType == ItemTypes::LVL_Block)
                {
                    auto *item = dynamic_cast<ItemBlock *>(*it);
                    Q_ASSERT(item);
                    //Applay move into main array
                    historySourceBuffer.blocks.push_back(item->m_data);
                    // item->m_data.x = (long)item->scenePos().x();
                    // item->m_data.y = (long)item->scenePos().y();
                    item->arrayApply();
                    historyBuffer.blocks.push_back(item->m_data);
                    s->m_data->meta.modified = true;
                }
                else if(ObjType == ItemTypes::LVL_BGO)
                {
                    auto *item = dynamic_cast<ItemBGO *>(*it);
                    Q_ASSERT(item);
                    //Applay move into main array
                    historySourceBuffer.bgo.push_back(item->m_data);
                    //item->bgoData.x = (long)(*it)->scenePos().x();
                    //item->bgoData.y = (long)(*it)->scenePos().y();
                    item->arrayApply();
                    historyBuffer.bgo.push_back(item->m_data);
                    s->m_data->meta.modified = true;
                }
                else if(ObjType == ItemTypes::LVL_NPC)
                {
                    auto *item = dynamic_cast<ItemNPC *>(*it);
                    Q_ASSERT(item);
                    //Applay move into main array
                    historySourceBuffer.npc.push_back(item->m_data);
                    //item->npcData.x = (long)(*it)->scenePos().x();
                    //item->npcData.y = (long)(*it)->scenePos().y();
                    item->arrayApply();
                    historyBuffer.npc.push_back(item->m_data);
                    s->m_data->meta.modified = true;
                }
                else if(ObjType == ItemTypes::LVL_PhysEnv)
                {
                    auto *item = dynamic_cast<ItemPhysEnv *>(*it);
                    Q_ASSERT(item);
                    //Applay move into main array
                    historySourceBuffer.physez.push_back(item->m_data);
                    //item->waterData.x = (long)(*it)->scenePos().x();
                    //item->waterData.y = (long)(*it)->scenePos().y();
                    item->arrayApply();
                    historyBuffer.physez.push_back(item->m_data);
                    s->m_data->meta.modified = true;
                }
                else if(ObjType == ItemTypes::LVL_META_DoorEnter)
                {
                    auto *item = dynamic_cast<ItemDoor *>(*it);
                    Q_ASSERT(item);
                    //Applay move into main array
                    //historySourceBuffer.water.push_back(dynamic_cast<ItemWater *>(*it)->waterData);
                    LevelDoor oldDoorData = item->m_data;
                    oldDoorData.isSetIn = true;
                    oldDoorData.isSetOut = false;
                    historySourceBuffer.doors.push_back(oldDoorData);
                    item->m_data.ix = (long)(*it)->scenePos().x();
                    item->m_data.iy = (long)(*it)->scenePos().y();

                    if(item->m_data.lvl_i || item->m_data.lvl_o)
                    {
                        item->m_data.ox = (long)(*it)->scenePos().x();
                        item->m_data.oy = (long)(*it)->scenePos().y();
                    }

                    item->arrayApply();
                    LevelDoor newDoorData = item->m_data;
                    newDoorData.isSetIn = true;
                    newDoorData.isSetOut = false;
                    historyBuffer.doors.push_back(newDoorData);
                    //historyBuffer.water.push_back(dynamic_cast<ItemWater *>(*it)->waterData);
                    s->m_data->meta.modified = true;
                }
                else if(ObjType == ItemTypes::LVL_META_DoorExit)
                {
                    auto *item = dynamic_cast<ItemDoor *>(*it);
                    Q_ASSERT(item);
                    //Applay move into main array
                    //historySourceBuffer.water.push_back(dynamic_cast<ItemWater *>(*it)->waterData);
                    LevelDoor oldDoorData = dynamic_cast<ItemDoor *>(*it)->m_data;
                    oldDoorData.isSetIn = false;
                    oldDoorData.isSetOut = true;
                    historySourceBuffer.doors.push_back(oldDoorData);
                    item->m_data.ox = (long)(*it)->scenePos().x();
                    item->m_data.oy = (long)(*it)->scenePos().y();

                    if(item->m_data.lvl_i || item->m_data.lvl_o)
                    {
                        item->m_data.ix = (long)(*it)->scenePos().x();
                        item->m_data.iy = (long)(*it)->scenePos().y();
                    }

                    item->arrayApply();
                    LevelDoor newDoorData = item->m_data;
                    newDoorData.isSetIn = false;
                    newDoorData.isSetOut = true;
                    historyBuffer.doors.push_back(newDoorData);
                    //historyBuffer.water.push_back(dynamic_cast<ItemWater *>(*it)->waterData);
                    s->m_data->meta.modified = true;
                }
                else if(ObjType == ItemTypes::LVL_Player)
                {
                    auto *item = dynamic_cast<ItemPlayerPoint *>(*it);
                    Q_ASSERT(item);
                    historySourceBuffer.players.push_back(item->m_data);
                    //item->pointData.x =(long)(*it)->scenePos().x();
                    //item->pointData.y =(long)(*it)->scenePos().y();
                    item->arrayApply();
                    historyBuffer.players.push_back(item->m_data);
                }
            }////////////////////////SECOND FETCH///////////////////////
        }

        if(s->m_mouseIsMoved)
        {
            /***********If some door items are moved, refresh list!*****************/
            if(!historySourceBuffer.doors.isEmpty())
                s->mw()->dock_LvlWarpProps->setDoorData(-2);
            /***********************************************************************/
            s->m_history->addMove(historySourceBuffer, historyBuffer);
        }

        s->m_mouseIsMoved = false;
    }
}

void LVL_ModeSelect::keyPress(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent);
}

void LVL_ModeSelect::keyRelease(QKeyEvent *keyEvent)
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    switch(keyEvent->key())
    {
    case(Qt::Key_Delete):  //Delete action
        s->removeSelectedLvlItems();
        break;
    case(Qt::Key_Escape):
        if(!s->m_mouseIsMoved)
            s->clearSelection();
        break;
    default:
        break;
    }
}



void LVL_ModeSelect::setItemSourceData(QGraphicsItem *it, int ObjType)
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    gridSize = s->m_configs->defaultGrid.general;
    offsetX = 0;
    offsetY = 0;

    if(ObjType == ItemTypes::LVL_NPC)
    {
        sourcePos = QPoint(dynamic_cast<ItemNPC *>(it)->m_data.x, dynamic_cast<ItemNPC *>(it)->m_data.y);
        gridSize = dynamic_cast<ItemNPC *>(it)->m_gridSize;
        offsetX = dynamic_cast<ItemNPC *>(it)->m_localProps.setup.grid_offset_x;
        offsetY = dynamic_cast<ItemNPC *>(it)->m_localProps.setup.grid_offset_y;
    }
    else if(ObjType == ItemTypes::LVL_Block)
    {
        sourcePos = QPoint(dynamic_cast<ItemBlock *>(it)->m_data.x, dynamic_cast<ItemBlock *>(it)->m_data.y);
        gridSize = dynamic_cast<ItemBlock *>(it)->m_gridSize;
        offsetX = dynamic_cast<ItemBlock *>(it)->m_gridOffsetX;
        offsetY = dynamic_cast<ItemBlock *>(it)->m_gridOffsetY;
        //WriteToLog(QtDebugMsg, QString(" >>Check collision for Block"));
    }
    else if(ObjType == ItemTypes::LVL_BGO)
    {
        sourcePos = QPoint(dynamic_cast<ItemBGO *>(it)->m_data.x, dynamic_cast<ItemBGO *>(it)->m_data.y);
        gridSize = dynamic_cast<ItemBGO *>(it)->m_gridSize;
        offsetX = dynamic_cast<ItemBGO *>(it)->m_gridOffsetX;
        offsetY = dynamic_cast<ItemBGO *>(it)->m_gridOffsetY;
    }
    else if(ObjType == ItemTypes::LVL_PhysEnv)
    {
        sourcePos = QPoint(dynamic_cast<ItemPhysEnv *>(it)->m_data.x, dynamic_cast<ItemPhysEnv *>(it)->m_data.y);
        gridSize = qRound(qreal(s->m_configs->defaultGrid.general) / 2);
    }
    else if(ObjType == ItemTypes::LVL_META_DoorEnter)
    {
        sourcePos = QPoint(dynamic_cast<ItemDoor *>(it)->m_data.ix, dynamic_cast<ItemDoor *>(it)->m_data.iy);
        gridSize = qRound(qreal(s->m_configs->defaultGrid.general) / 2);
    }
    else if(ObjType == ItemTypes::LVL_META_DoorExit)
    {
        sourcePos = QPoint(dynamic_cast<ItemDoor *>(it)->m_data.ox, dynamic_cast<ItemDoor *>(it)->m_data.oy);
        gridSize = qRound(qreal(s->m_configs->defaultGrid.general) / 2);
    }
    else if(ObjType == ItemTypes::LVL_Player)
    {
        gridSize = 2 ;
        sourcePos = QPoint(dynamic_cast<ItemPlayerPoint *>(it)->m_data.x, dynamic_cast<ItemPlayerPoint *>(it)->m_data.y);
    }
}
